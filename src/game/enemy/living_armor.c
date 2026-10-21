/* LivingArmor: the behaviour of entity type 36 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Living Armor. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/effect.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"
#include "player/player.h"

/* Entity.animation_index 0x11: the pose where part 3 turns to aim at the player */
#define LIVING_ARMOR_MOTION_AIM 0x11

#define ENTITY_WORK_AREA_SIZE 0xA0

static void LivingArmor_Think(Entity *entity, s32 run_now);
static void LivingArmor_ThinkCombat(Entity *entity, s32 run_now);
static void LivingArmor_AimPart3AtPlayer(Entity *actor);
static void LivingArmor_Animate(Entity *entity);
static void LivingArmor_OnHit(Entity *entity);
static void LivingArmor_ReactionHeavy(Entity *entity);
static void LivingArmor_WaitToStrike(Entity *entity);
static void LivingArmor_MeleeAttack1(Entity *entity);
static void LivingArmor_Dormant(Entity *entity);
static void LivingArmor_MeleeAttack0(Entity *entity);

/* Set-up of a type 36 entity: clears its work area and installs the type's handlers. */
void LivingArmor_Init(Entity *entity) {
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
    ai_state->think = LivingArmor_Think;
    ai_state->animate = LivingArmor_Animate;
    ai_state->on_hit = LivingArmor_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
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
    ai_state->pause_timer = 0;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with LivingArmor_ThinkCombat for state 2. */
static void LivingArmor_Think(Entity *entity, s32 run_now) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    switch (entity->state) {
    case ENTITY_STATE_UNAWARE:
        EntityAi_ThinkIdle(entity, run_now);
        return;
    case ENTITY_STATE_SEARCHING:
        EntityAi_ThinkPursue(entity, run_now);
        return;
    case ENTITY_STATE_NOTICED:
        LivingArmor_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but melee record 0 has the type's own
   action, LivingArmor_MeleeAttack0, and the head does not follow the target (flag bit 9 clear). */
static void LivingArmor_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = 0;
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                EntityAi_StartAction(entity, LivingArmor_MeleeAttack0, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->turn_in_place == 0) {
                ai_state->turn_in_place = ABS(angle) > DEG_TO_RAD(30);
            }
            if (ai_state->turn_in_place != 0) {
                entity->move_command = EntityAi_TurnCommand(entity);
                if (entity->move_command == 0) {
                    ai_state->turn_in_place = 0;
                }
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            if (turn > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
            if ((s16)entity->params->action_frames != 0) {
                entity->move_command |= MOVE_COMMAND_RUN;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* Helper of LivingArmor_Animate: in animation 17 turns model part 3 to look at the player
   (Bone_TurnPartToLook, the angles kept in work area unk30) and then takes 120 degrees off the part's
   angle unk50. */
static void LivingArmor_AimPart3AtPlayer(Entity *actor) {
    char pad[0x10]; /* unused, but retail reserves the 0x10 bytes */
    Qword pair_copy;
    ModelInstance *model;
    ModelPart *joint;
    float angle;

    pair_copy = living_armor_look_limits;
    model = ENTITY_MODEL(actor);
    joint = &model->parts[3];
    if (joint != NULL && actor->animation_index == LIVING_ARMOR_MOTION_AIM) {
        Bone_TurnPartToLook(model, (int)joint, (int)actor->ai_state.target_point, 1, &player_state.position, 0, 10.0f);
        angle = joint->rotation_x - DEG_TO_RAD(120);
        joint->rotation_x = angle;
        joint->rotation_x = EntityMath_WrapAngle(angle);
    }
}

/* The animate handler: as EntityAi_Animate without the head turn; during an attack (busy bit 1) in
   animation 17 LivingArmor_AimPart3AtPlayer turns model part 3, otherwise work float unk48 is put to
   0. */
static void LivingArmor_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;
    char unused[0x10]; /* retail reserves the 0x10 bytes */

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FIXED) {
            entity->flags = flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        }
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        if ((entity->busy_flags & ENTITY_BUSY_ATTACK) && entity->animation_index == LIVING_ARMOR_MOTION_AIM) {
            LivingArmor_AimPart3AtPlayer(entity);
            return;
        }
        ai_state->part_angle_x = 0.0f;
    }
}

/* The on_hit handler: every hit starts LivingArmor_ReactionHeavy, whatever its strength. Sets flag
   bits 18, 20 and 21, and work word unk68 to half of EntityParams.pitch_limit_deg plus a random number below
   unk430. */
static void LivingArmor_OnHit(Entity *entity) {
    EntityAiState *sub = &entity->ai_state;
    int rnd = rand();

    sub->pause_timer = ENTITY_CONTROLLER(entity)->pitch_limit_deg / 2 + rnd % ENTITY_CONTROLLER(entity)->pitch_limit_deg;
    ENTITY_FLAG_BITS(entity)->bit20 = 1;
    ENTITY_FLAG_BITS(entity)->bit18 = 1;
    ENTITY_FLAG_BITS(entity)->bit21 = 1;
    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    EntityAi_SetReaction(entity, LivingArmor_ReactionHeavy, 0);
    EntityAi_SetState(entity, 2);
}

/* The reaction to a hit of either strength (busy bit 4): as EntityAi_ReactionHeavy with animation
   16; when it has ended it starts animation 1 and the action LivingArmor_WaitToStrike. */
static void LivingArmor_ReactionHeavy(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = FALSE;
        ENTITY_FLAG_BITS(entity)->guarding = FALSE;
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        }
        entity->reaction_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->reaction_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
            EntityAi_SetReaction(entity, NULL, 0);
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            Motion_Start(entity, 1, 0, 5, 1);
            EntityAi_StartAction(entity, LivingArmor_WaitToStrike, 1);
        }
        break;
    }
}

/* An action (busy bit 0x1000) that LivingArmor_ReactionHeavy and LivingArmor_MeleeAttack1 start with animation
   1 playing: it starts LivingArmor_MeleeAttack1 when the player comes within EntityParams.action_frames (plus the
   radius and 100) with nothing in the way, and goes back to LivingArmor_Dormant once work word unk68 has
   counted down. */
static void LivingArmor_WaitToStrike(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    if (entity->distance_to_player < (s16)entity->params->action_frames + entity->radius + 100.0f) {
        if (!EntitySight_PropInWay(entity) && !EntitySight_ObjectInWay(entity)) {
            EntityAi_StartAction(entity, LivingArmor_MeleeAttack1, 1);
        }
    }
    if (ai_state->pause_timer-- <= 0 && entity->action_step == ACTION_STEP_START) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = FALSE;
        ENTITY_FLAG_BITS(entity)->guarding = FALSE;
        EntityAi_StartAction(entity, LivingArmor_Dormant, 1);
    }
}

/* The melee attack with record 1 (busy bit 1), which LivingArmor_WaitToStrike starts: animation 17, its hits
   checked at the angle Entity.heading_to_player (EntityAi_CheckMeleeHitAt); on frames 10 to 22 it starts effect 0x222
   along the path of the point Model_GetSkinnedBonePoint gives for 9. At its end, EntityParams.yaw_limit_deg times in
   100, it goes back to animation 1 and LivingArmor_WaitToStrike with a new count in work word unk68; else it
   starts LivingArmor_Dormant. */
static void LivingArmor_MeleeAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[1];
    f32 position[4];
    f32 angles[4];
    f32 *tip;
    f32 *base;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
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
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        tip = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), 9);
        base = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), 8);
        if (tip != NULL && base != NULL) {
            if (entity->model->level_tables->frame >= 10 && entity->model->level_tables->frame <= 22) {
                delta_x = base[0] - tip[0];
                delta_y = base[1] - tip[1];
                delta_z = base[2] - tip[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                angles[2] = angles[3] = 0.0f;
                delta_x = tip[0] - ai_state->part_angle_y;
                delta_y = tip[1] - ai_state->part_angle_z;
                delta_z = tip[2] - ai_state->pitch_speed;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = ai_state->part_angle_y + delta_x * i / count;
                    position[1] = ai_state->part_angle_z + delta_y * i / count;
                    position[2] = ai_state->pitch_speed + delta_z * i / count;
                    Effect_StartSpawner(0x222, position, angles, entity);
                }
            }
            ai_state->part_angle_y = tip[0];
            ai_state->part_angle_z = tip[1];
            ai_state->pitch_speed = tip[2];
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->heading_to_player));
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->heading_to_player));
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->heading_to_player));
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            if (rand() % 100 < entity->params->yaw_limit_deg) {
                entity->busy_flags = 0;
                entity->move_command = 0;
                entity->action_timer = 0;
                ai_state->pause_timer = entity->params->pitch_limit_deg / 2 + rand() % entity->params->pitch_limit_deg;
                ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
                Motion_Start(entity, 1, 0, 5, 1);
                EntityAi_StartAction(entity, LivingArmor_WaitToStrike, 1);
            } else {
                entity->busy_flags = 0;
                entity->move_command = 0;
                entity->action_timer = 0;
                EntityAi_SetAttackDelay(entity, unk240->attack_delay);
                ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
                ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
                ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
                EntityAi_StartAction(entity, LivingArmor_Dormant, 1);
            }
        }
        break;
    }
}

/* A dormant action (busy bit 0x20) that LivingArmor_WaitToStrike and LivingArmor_MeleeAttack1 start; the
   dormant slot holds EntityAi_Dormant. It is the second half of that one: plays animation 2 to its
   end, goes to state 2, clears flag bits 18, 20 and 21 and thinks. */
static void LivingArmor_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->hit_strength = 0;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        ENTITY_FLAG_BITS(entity)->attacking = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
                ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            }
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The melee attack with record 0 (busy bit 1): animation 20, as EntityAi_MeleeAttack; on frames 10
   to 22 it starts effect 0x222 along the path of the point Model_GetSkinnedBonePoint gives for 9. */
static void LivingArmor_MeleeAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];
    f32 position[4];
    f32 angles[4];
    f32 *tip;
    f32 *base;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        tip = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), 9);
        base = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), 8);
        if (tip != NULL && base != NULL) {
            if (entity->model->level_tables->frame >= 10 && entity->model->level_tables->frame <= 22) {
                delta_x = base[0] - tip[0];
                delta_y = base[1] - tip[1];
                delta_z = base[2] - tip[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                angles[2] = angles[3] = 0.0f;
                delta_x = tip[0] - ai_state->part_angle_y;
                delta_y = tip[1] - ai_state->part_angle_z;
                delta_z = tip[2] - ai_state->pitch_speed;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = ai_state->part_angle_y + delta_x * i / count;
                    position[1] = ai_state->part_angle_z + delta_y * i / count;
                    position[2] = ai_state->pitch_speed + delta_z * i / count;
                    Effect_StartSpawner(0x222, position, angles, entity);
                }
            }
            ai_state->part_angle_y = tip[0];
            ai_state->part_angle_z = tip[1];
            ai_state->pitch_speed = tip[2];
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->level_tables->frame);
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
