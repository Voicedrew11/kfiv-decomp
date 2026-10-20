/* Sentinel: the behaviour of entity type 33 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Sentinel. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "player/player.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void Sentinel_Think(Entity *entity, s32 unknown);
static void Sentinel_ThinkCombat(Entity *entity, s32 unknown);
static void Sentinel_MeleeAttack1(Entity *entity);
static void Sentinel_PlayAnim12(Entity *entity);
static void Sentinel_Animate(Entity *entity);

/* Set-up of a type 33 entity: clears its work area and installs the type's handlers. */
void Sentinel_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Sentinel_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
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
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with Sentinel_ThinkCombat for state 2. */
static void Sentinel_Think(Entity *entity, s32 unknown) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        Sentinel_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but starts Sentinel_MeleeAttack1 when
   the player is within 500 and more than 140 degrees off its heading. */
static void Sentinel_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    if (entity->distance_to_player < 500.0f) {
        if (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > DEG_TO_RAD(140) ||
            EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) < -DEG_TO_RAD(140)) {
            EntityAi_StartAction(entity, Sentinel_MeleeAttack1, unknown);
            return;
        }
    }
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case 0:
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = -1;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    if (ai_state->move_choice == 0) {
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
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The melee attack with record 1 (busy bit 1): animation 21, its hits checked towards the back (the
   heading plus 180 degrees); Sentinel_PlayAnim12 follows it. */
static void Sentinel_MeleeAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[1];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x15, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + PI));
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + PI));
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + PI));
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
            EntityAi_StartAction(entity, Sentinel_PlayAnim12, 1);
        }
        break;
    }
}

/* An action (busy bit 0x1000) that follows Sentinel_MeleeAttack1: plays animation 12 to its
   end. */
static void Sentinel_PlayAnim12(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
        ai_state->turn_rate = 0.0f;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An animate handler the set-up function does not install (it installs EntityAi_Animate): as
   EntityAi_Animate, but it also steps the frame count EntityModelChannel.unk82 while a blend is on.
   */
static void Sentinel_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;
    f32 rate;
    s16 step;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FIXED) {
            entity->flags = flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        }
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            if (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) {
                rate = ai_state->pose_blend_weight;
            } else {
                rate = 1.0f / (f32)blend_frames;
                step = entity->model->level_tables->frame;
                if (step < EntityAi_AnimationFrameCount(entity, entity->animation_index) - 1) {
                    entity->model->level_tables->frame++;
                }
            }
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), rate);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
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
