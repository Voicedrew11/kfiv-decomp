/* Mummy: the behaviour of entity type 39 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Mummy. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_math.h"
#include "player/player.h"

/* EntityAiState.move_choice 1 (0 is MOVE_CHOICE_CLOSE_IN): backs off from the player */
#define MUMMY_MOVE_CHOICE_BACK 1


static void Mummy_Think(Entity *entity, s32 run_now);
static void Mummy_ThinkCombat(Entity *entity, s32 run_now);
static void Mummy_OnHit(Entity *entity);
static s32 Mummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach);

/* Set-up of a type 39 entity: clears its work area and installs the type's handlers. */
void Mummy_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Mummy_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = Mummy_OnHit;
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

/* The think handler: as EntityAi_Think, with Mummy_ThinkCombat for state 2. Sets work byte
   unk7A once Entity.hit_points is below half of EntityParams.hit_points. */
static void Mummy_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->hit_points < entity->params->hit_points >> 1 && ai_state->step_index == 0) {
        ai_state->step_index = 1;
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
                Mummy_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but closing in runs once ai_state.step_index
   is set. When the player is near, the entity is within 40 degrees of the player's facing and the
   player is 40 to 90 degrees off its heading, it goes backwards for 90 to 149 frames instead (work
   byte unk78 is 1), with the command Mummy_BackwardsCommand gives. */
static void Mummy_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;
    s32 mode;

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
    if (ai_state->move_choice != MUMMY_MOVE_CHOICE_BACK) {
        angle = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));
        if (entity->distance_to_player_3d - entity->radius - 100.0f < 200.0f && ABS(angle) < DEG_TO_RAD(40)) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(angle) > DEG_TO_RAD(40) && ABS(angle) < HALF_PI) {
                ai_state->move_choice = MUMMY_MOVE_CHOICE_BACK;
                entity->target = &player_state;
                entity->combat_choice = COMBAT_CHOICE_CLOSE_IN;
                entity->action_timer = rand() % 60 + 90;
            }
        }
    }
    mode = entity->combat_choice;
    if (mode == 1) {
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
            switch (ai_state->move_choice) {
            case MOVE_CHOICE_CLOSE_IN:
                if (ai_state->step_index != 0) {
                    if (turn > 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
                    } else if (turn < 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
                    } else {
                        entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                    }
                } else {
                    if (turn > 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                    } else if (turn < 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                    } else {
                        entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                    }
                }
                break;
            case MUMMY_MOVE_CHOICE_BACK:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_LEFT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_RIGHT);
                } else {
                    // match: the caller passes an integer argument the callee does not take
                    entity->move_command = Mummy_BackwardsCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit forgets the target and starts a
   reaction (EntityAi_ReactionHeavy). */
static void Mummy_OnHit(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_LowerWeightsAfterHit(entity);
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->target = NULL;
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
}

/* The movement command for going backwards: a turn bit (0x100 / 0x200) while the target is off the heading by
   more than the turn limit, and command bit 2 (backwards) while it is farther than reach; 0 without a target.
   The second parameter is not used. */
static s32 Mummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach) {
    f32 turn;
    f32 limit;
    f32 heading;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    heading = EntityMath_HeadingTo(entity->model->position, entity->target);
    turn = EntityMath_AngleDifference(entity->model->rotation[1], heading);
    limit = entity->params->turn_limit;
    if (0.0f <= turn ? limit < turn : limit < -turn) {
        command = MOVE_COMMAND_TURN_LEFT;
        if (0.0f <= turn) {
            command = MOVE_COMMAND_TURN_RIGHT;
        }
    }
    if (reach < EntityMath_DistanceXZ(entity->model->position, entity->target)) {
        command |= MOVE_COMMAND_BACK;
    }
    return command;
}
