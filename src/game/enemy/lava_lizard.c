/* LavaLizard: the behaviour of entity type 34 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Lava Lizard. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"
#include "player/player.h"

/* EntitySpawnInfo.spawn_mode: the default spawn, which keeps the think handler with states */
#define LAVA_LIZARD_SPAWN_MODE_DEFAULT 0

/* LavaLizard_RunAtPlayer: Entity.action_step (step 0 is the shared start) */
#define LAVA_LIZARD_RUN_STEP_TURN 1                 /* turns to the player until action_timer runs out or it is pushed */

// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void LavaLizard_Think(Entity *entity, s32 run_now);
static void LavaLizard_ThinkCombat(Entity *entity, s32 run_now);
static void LavaLizard_RunAtPlayer(Entity *entity);
static void LavaLizard_RangedAttack0(Entity *entity);
static void LavaLizard_ThinkNoStates(Entity *entity, s32 run_now);
static void LavaLizard_ThinkDecideShootOrTurn(Entity *entity, s32 run_now);

/* Set-up of a type 34 entity: clears its work area and installs the type's handlers. */
void LavaLizard_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    if (entity->spawn_info->spawn_mode != LAVA_LIZARD_SPAWN_MODE_DEFAULT) {
        ai_state->think = LavaLizard_ThinkNoStates;
    } else {
        ai_state->think = LavaLizard_Think;
    }
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
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

/* The think handler: as EntityAi_Think, with LavaLizard_ThinkCombat for state 2. Counts down
   step_index, the pause before the next LavaLizard_RunAtPlayer (set from EntityParams.far_distance
   while flag bit 45, 47 or 48 is set). */
static void LavaLizard_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (entity->flags & (ENTITY_FLAG_WALL_PUSH | ENTITY_FLAG_OBJECT_PUSH | ENTITY_FLAG_ENTITY_PUSH)) {
        ai_state->step_index = entity->params->far_distance;
    }
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            case ENTITY_STATE_SEARCHING:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                LavaLizard_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
    if (ai_state->step_index > 0) {
        ai_state->step_index--;
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but the ranged attack is
   LavaLizard_RangedAttack0, and instead of closing in it starts LavaLizard_RunAtPlayer when the
   player is farther than EntityParams.action_frames, within 60 degrees ahead, and the pause is over. */
static void LavaLizard_ThinkCombat(Entity *entity, s32 run_now) {
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
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
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
                EntityAi_StartAction(entity, LavaLizard_RangedAttack0, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->step_index == 0 && ABS(angle) < DEG_TO_RAD(60) &&
                (f32)ENTITY_CONTROLLER(entity)->action_frames < entity->distance_to_player) {
                EntityAi_StartAction(entity, LavaLizard_RunAtPlayer, run_now);
                return;
            }
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
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* An action (busy bit 0x1000): plays animation 4, turning to the player, for EntityParams.near_distance
   frames or until it is pushed; starts EntityAi_MeleeAttack with record 0 as soon as that record
   reaches the player. */
static void LavaLizard_RunAtPlayer(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];

    entity->busy_flags |= ENTITY_BUSY_1000;
    if (EntityAi_CanUseMelee(entity, 0)) {
        EntityAi_StartAttack(entity, EntityAi_MeleeAttack, 1, 0, 0);
        return;
    }
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_timer = entity->params->near_distance;
        entity->action_step++;
        break;
    case LAVA_LIZARD_RUN_STEP_TURN:
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        entity->turn_delta_yaw += entity->turn_delta_yaw;
        if (entity->action_timer == 0 || EntityAi_TurnFromPush(entity, direction) != 0) {
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

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame 50 to 150 launches the
   record's shots every other frame. */
static void LavaLizard_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (entity->model->level_tables->frame >= 50 && entity->model->level_tables->frame <= 150 && (frame_counter & 1)) {
            EntityAi_LaunchShots(entity, unk3D8, 50, 150);
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

/* The think handler of a spawn whose EntitySpawnInfo.spawn_mode is set: no states; while the entity is
   not busy it calls LavaLizard_ThinkDecideShootOrTurn. */
static void LavaLizard_ThinkNoStates(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        LavaLizard_ThinkDecideShootOrTurn(entity, run_now);
    }
    if (ai_state->step_index > 0) {
        ai_state->step_index--;
    }
}

/* Decides for LavaLizard_ThinkNoStates: guards the player's swing, starts LavaLizard_RangedAttack0
   when EntityAi_CanUseRanged finds the player in reach, else turns on the spot towards the player from time
   to time. */
static void LavaLizard_ThinkDecideShootOrTurn(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    if (entity->attack_delay_frames == 0 && EntityAi_CanUseRanged(entity, 1)) {
        EntityAi_StartAttack(entity, LavaLizard_RangedAttack0, run_now, 1, 0);
        return;
    }
    if (entity->action_timer == 0) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (ABS(angle) > DEG_TO_RAD(30)) {
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
        } else {
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = MOVE_CHOICE_NONE;
        }
    }
    if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
        entity->move_command = EntityAi_TurnCommand(entity);
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}
