/* MalformedGiant: the behaviour of entity type 41 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Malformed Giant. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include "player/player.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void MalformedGiant_Think(Entity *entity, s32 run_now);
static void MalformedGiant_ThinkCombat(Entity *entity, s32 run_now);
static void MalformedGiant_ThinkIdle(Entity *entity, s32 run_now);
static void MalformedGiant_ThinkPursue(Entity *entity, s32 run_now);
static void MalformedGiant_PlayAnim0ToFrame80(Entity *entity);

/* Set-up of a type 41 entity: clears its work area and installs the type's handlers. */
void MalformedGiant_Init(Entity *entity) {
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
    ai_state->think = MalformedGiant_Think;
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

/* The think handler: as EntityAi_Think with the type's own functions for the three states. Counts
   the frames with flag bit 41 set in work byte unk7A and at 10 starts MalformedGiant_PlayAnim0ToFrame80. */
static void MalformedGiant_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (ENTITY_FLAG_BITS(entity)->raise_attack_weight == 1) {
        ai_state->step_index++;
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ai_state->step_index >= 10) {
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        ENTITY_UNK5C(entity)->as_s32 = 0;
        ai_state->step_index = 0;
        EntityAi_StartAction(entity, MalformedGiant_PlayAnim0ToFrame80, run_now);
        return;
    } else if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
    } else {
        switch (entity->state) {
        case ENTITY_STATE_UNAWARE:
            MalformedGiant_ThinkIdle(entity, run_now);
            break;
        case ENTITY_STATE_SEARCHING:
            MalformedGiant_ThinkPursue(entity, run_now);
            break;
        case ENTITY_STATE_NOTICED:
            MalformedGiant_ThinkCombat(entity, run_now);
            break;
        default:
            MalformedGiant_ThinkIdle(entity, run_now);
            break;
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but when it would start closing in it
   starts MalformedGiant_PlayAnim0ToFrame80 instead with a chance of EntityParams.action_frames in 100, and closing
   in never runs. */
static void MalformedGiant_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
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
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            if (rand() % 100 < ENTITY_CONTROLLER(entity)->action_frames) {
                entity->busy_flags = 0;
                entity->move_command = 0;
                entity->action_timer = 0;
                entity->target = NULL;
                EntityAi_StartAction(entity, MalformedGiant_PlayAnim0ToFrame80, run_now);
                ai_state->step_index = 0;
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

/* The think function of state 0: as EntityAi_ThinkIdle, but it picks no point to go to: it only
   gives the command towards the target it has. */
static void MalformedGiant_ThinkIdle(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[2];
    s32 turn;
    void (*handler)(Entity *);

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (turn < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    handler = ai_state->move;
    EntityAi_StartAction(entity, handler, run_now);
}

/* The think function of state 1: as EntityAi_ThinkPursue, but each time the timer Entity.action_timer is
   at 0 it starts MalformedGiant_PlayAnim0ToFrame80 with a chance of EntityParams.action_frames in 100. */
static void MalformedGiant_ThinkPursue(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    if (entity->action_timer == 0) {
        if (rand() % 100 < ENTITY_CONTROLLER(entity)->action_frames) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            entity->target = NULL;
            EntityAi_StartAction(entity, MalformedGiant_PlayAnim0ToFrame80, run_now);
            ai_state->step_index = 0;
            return;
        }
        entity->target = &player_state;
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (turn < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* An action (busy bit 0x1000): stands in animation 0 with flag bit 9 clear until the animation is
   past frame 80, then thinks. The think functions start it, and clear work byte unk7A. */
static void MalformedGiant_PlayAnim0ToFrame80(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame > 0x50) {
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
