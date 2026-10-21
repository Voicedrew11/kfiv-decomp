/* WiddaQueen: the behaviour of entity type 46 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Widda Queen. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include <stdlib.h>
#include "model/motion.h"
#include "player/player.h"

/* Entity.action_step of WiddaQueen_MeleeAttack1 past the shared ones */
#define WIDDA_QUEEN_MELEE_STEP_SWING  2   /* swings with its hit windows until frame 61, then ends */
#include "entity/entity_ai.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

static void WiddaQueen_Animate(Entity *entity);
static void WiddaQueen_TurnPart9ToPlayer(Entity *entity, s32 mode);
static void WiddaQueen_Think(Entity *entity, s32 run_now);
static void WiddaQueen_ThinkDecide(Entity *entity, s32 run_now);
static void WiddaQueen_MeleeAttack0(Entity *entity);
static void WiddaQueen_MeleeAttack1(Entity *entity);
static void WiddaQueen_RangedAttack0(Entity *entity);
static void WiddaQueen_RangedAttack1(Entity *entity);
static void WiddaQueen_Move(Entity *entity);
static s32 WiddaQueen_CanAttack(Entity *entity);
static s32 WiddaQueen_PickAttack(Entity *entity);
static void WiddaQueen_Death(Entity *entity);

/* Set-up of a type 46 entity: clears its work area and installs the type's handlers. */
void WiddaQueen_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *choice_weights = entity->choice_weights;
    u8 *attack_weights = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = ENTITY_WORK_BYTES(entity);

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = WiddaQueen_Think;
    ai_state->animate = WiddaQueen_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = WiddaQueen_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = WiddaQueen_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: as EntityAi_Animate, but the motion never moves the entity, and
   WiddaQueen_TurnPart9ToPlayer turns model part 9. */
static void WiddaQueen_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        WiddaQueen_TurnPart9ToPlayer(entity, ai_state->step_index);
        look_at_target = ENTITY_FLAG_BITS(entity)->look_at_target;
        limits[1] = DEG_TO_RAD(entity->params->pitch_limit_deg);
        limits[0] = DEG_TO_RAD(-entity->params->pitch_limit_deg);
        limits[3] = DEG_TO_RAD(entity->params->yaw_limit_deg);
        limits[2] = DEG_TO_RAD(-entity->params->yaw_limit_deg);
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* Helper of WiddaQueen_Animate: in mode 1 turns model part 9 towards the player, at most 90
   degrees to either side, at the rate EntityParams.near_distance; in mode 2 holds it; else puts the angle
   (work float unk48) back to 0. The mode is work byte unk7A. */
static void WiddaQueen_TurnPart9ToPlayer(Entity *entity, s32 mode) {
    EntityAiState *ai_state;
    EntityModelPart80 *part;
    f32 rate;
    f32 angle;

    ai_state = &entity->ai_state;
    rate = (f32)entity->params->near_distance;
    part = &((EntityModelPart80 *)entity->model->parts)[9];
    if (entity->ailment_3_frames != 0) {
        rate += rate;
    }
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (HALF_PI < angle) {
        angle = HALF_PI;
    }
    if (angle < -HALF_PI) {
        angle = -HALF_PI;
    }
    if (mode == 1) {
        EntityMath_EaseAngleToward(&ai_state->part_angle_x, angle, rate);
        part->part_angles[1] = ai_state->part_angle_x;
    } else if (mode == 2) {
        part->part_angles[1] = ai_state->part_angle_x;
    } else {
        ai_state->part_angle_x = 0.0f;
    }
}

/* The think handler: no states; every tenth frame updates what the entity knows of the player, and
   while it is not busy calls WiddaQueen_ThinkDecide. */
static void WiddaQueen_Think(Entity *entity, s32 run_now) {
    u32 phase;

    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->busy_flags == 0) {
        EntityAi_StepState(entity);
        WiddaQueen_ThinkDecide(entity, run_now);
    }
}

/* Decides for WiddaQueen_Think: guards the player's swing; when an attack reaches
   (WiddaQueen_CanAttack) starts the one WiddaQueen_PickAttack draws; else starts the move
   action. */
static void WiddaQueen_ThinkDecide(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_ShouldGuard(entity, 300.0f, PI)) {
        // match: called as returning int; only then is the 2 below loaded into $3
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        ai_state->step_index = 2;
        return;
    }
    ai_state->step_index = 1;
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    if (entity->attack_delay_frames == 0 && !(s32)(entity->flags >> 63) && WiddaQueen_CanAttack(entity)) {
        if (WiddaQueen_PickAttack(entity)) {
            if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                EntityAi_StartAction(entity, WiddaQueen_RangedAttack0, run_now);
            } else {
                EntityAi_StartAction(entity, WiddaQueen_RangedAttack1, run_now);
            }
        } else {
            if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                EntityAi_StartAction(entity, WiddaQueen_MeleeAttack0, run_now);
            } else {
                EntityAi_StartAction(entity, WiddaQueen_MeleeAttack1, run_now);
            }
        }
        return;
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The melee attack with record 0 (busy bit 1): animation 20, its hits checked along the heading of
   the turned part (work float unk48). */
static void WiddaQueen_MeleeAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            ai_state->step_index = 1;
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            ai_state->step_index = 1;
        } else {
            ai_state->step_index = 2;
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + ai_state->part_angle_x));
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + ai_state->part_angle_x));
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + ai_state->part_angle_x));
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The melee attack with record 1 (busy bit 1): animation 21 from frame 10; parts 0 and 1 are
   checked 45 degrees to either side; it ends at frame 61. */
static void WiddaQueen_MeleeAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[1];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x15, 10, (s16)entity->params->action_frames, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        ai_state->step_index = 0;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case WIDDA_QUEEN_MELEE_STEP_SWING:
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->frame, entity->model->rotation[1] - DEG_TO_RAD(45));
            if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
                ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->frame, entity->model->rotation[1] + DEG_TO_RAD(45));
            }
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
        }
        if (entity->model->level_tables->frame >= 0x3D) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23, its shots aimed at the player
   (EntityShot_LaunchAimed); calls EntityEvent_StartEffectAtPoint with 0x373 on frame 22. */
static void WiddaQueen_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->ranged_attacks[0];
    f32 target[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            ai_state->step_index = 1;
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            ai_state->step_index = 1;
        } else {
            ai_state->step_index = 2;
        }
        if (entity->model->level_tables->frame == 0x16 && (entity->ailment_4_frames == 0 || unk3D8->ignores_ailment_4 != 0)) {
            EntityEvent_StartEffectAtPoint(entity, 0x373, 4, entity->model->rotation);
            EntityEvent_StartEffectAtPoint(entity, 0x373, 8, entity->model->rotation);
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(target, &player_state.position);
        target[1] -= 190.0f;
        EntityShot_LaunchAimed(entity, (EntityParamsUnk3D8 *)unk3D8, elapsed, entity->model->level_tables->frame, target, HALF_PI, HALF_PI);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 1 (busy bit 1): animation 24; from frame EntityParams.far_distance to
   type_param_0 launches the record's shots on two frames in three. */
static void WiddaQueen_RangedAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x18, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            ai_state->step_index = 1;
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            ai_state->step_index = 1;
        } else {
            ai_state->step_index = 2;
        }
        if (entity->model->level_tables->frame >= entity->params->far_distance && entity->model->level_tables->frame <= entity->params->type_param_0) {
            if (frame_counter % 3U != 0) {
                EntityAi_LaunchShots(entity, unk3D8, entity->params->far_distance, entity->params->type_param_0);
            }
        }
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The move handler: this type stays where it is; keeps animation 0. */
static void WiddaQueen_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* Whether the player is in reach of any of the six attack records (EntityAi_CanUseMeleeFacing for the melee
   ones, EntityAi_CanUseRangedFacing for the ranged). */
static s32 WiddaQueen_CanAttack(Entity *entity) {
    f32 heading = EntityMath_WrapAngle(entity->model->rotation[1] + entity->ai_state.part_angle_x);

    if (EntityAi_CanUseMeleeFacing(entity, heading, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, entity->model->rotation[1], 1)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, heading, 2)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, heading, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, heading, 1)) {
        return TRUE;
    }
    return EntityAi_CanUseRangedFacing(entity, heading, 2) != 0;
}

/* Draws one of the six attacks by weight among those that reach the player, sets flag bits 57 to 59
   to it and returns whether it is a ranged one: EntityAi_PickAttack with the heading of the turned part.
   */
static s32 WiddaQueen_PickAttack(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 chance;
    s32 i;
    s32 roll;
    s32 sum;
    f32 angle;

    angle = EntityMath_WrapAngle(entity->model->rotation[1] + entity->ai_state.part_angle_x);
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 0) ? (entity->attack_weights[0] * entity->params->melee_attacks[0].attack_weight) >> 6 : 0;
    chances[0] = chance;
    total += chance;
    chance = EntityAi_CanUseMeleeFacing(entity, entity->model->rotation[1], 1) ? (entity->attack_weights[1] * entity->params->melee_attacks[1].attack_weight) >> 6 : 0;
    chances[1] = chance;
    total += chance;
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 2) ? (entity->attack_weights[2] * ((EntityParamsUnk240 *)entity->params->melee_attack_third)->attack_weight) >> 6 : 0;
    chances[2] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 0) ? (entity->attack_weights[3] * ENTITY_PARAMS_UNK3D8(entity->params, 0)->ranged_weight) >> 6 : 0;
    chances[3] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 1) ? (entity->attack_weights[4] * ENTITY_PARAMS_UNK3D8(entity->params, 1)->ranged_weight) >> 6 : 0;
    chances[4] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 2) ? (entity->attack_weights[5] * ENTITY_PARAMS_UNK3D8(entity->params, 2)->ranged_weight) >> 6 : 0;
    chances[5] = chance;
    total += chance;
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->attack_index = choice;
        ENTITY_FLAG_BITS(entity)->ranged_attack = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->attack_index = choice - 3;
        ENTITY_FLAG_BITS(entity)->ranged_attack = 1;
    }
    EntityAi_LowerAttackWeight(entity);
    return (s32)(entity->flags >> 57) & ENTITY_FLAG_ACTIVE_LIST;
}

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings. */
static void WiddaQueen_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags = (entity->flags & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_DYING_1;
        entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= ENTITY_FLAG_DYING_MASK;
            entity->flags &= ~ENTITY_FLAG_LIVE;
        }
        break;
    }
}
