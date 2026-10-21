/* Critter: the behaviour of entity type 25 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Critter. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_collision.h"
#include "entity/entity_math.h"
#include "player/player.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void Critter_Dormant(Entity *entity);
static void Critter_Think(Entity *entity, s32 run_now);
static void Critter_ThinkIdle(Entity *entity, s32 run_now);
static void Critter_ThinkCombat(Entity *entity, s32 run_now);

/* Set-up of a type 25 entity: clears its work area and installs the type's handlers. */
void Critter_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Critter_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = Critter_Dormant;
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
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The dormant action (busy bit 0x20): holds animation 2 at its start, the motion stopped, until the
   player is within EntitySpawnInfo.wake_distance or flag bit 43 is set; then plays it to its end, goes to
   state 2 and thinks. */
static void Critter_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 2, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        ENTITY_FLAG_BITS(entity)->animates = FALSE;
        entity->action_step++;
        break;
    case DORMANT_STEP_SLEEP:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            EntityAi_SetState(entity, 2);
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

/* The think handler: as EntityAi_Think, with Critter_ThinkIdle for state 0 and
   Critter_ThinkCombat for states 1 and 2. Sets the flag field bits60 to 5 whenever
   EntityCollision_DistanceToGround gives a reading for the entity's position. */
static void Critter_Think(Entity *entity, s32 run_now) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (EntityCollision_DistanceToGround(entity->model->position) != critter_no_ground_height[0]) {
        ENTITY_FLAG_BITS(entity)->floor_material = 5;
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
        break;
    case ENTITY_STATE_SEARCHING:
    case ENTITY_STATE_NOTICED:
        Critter_ThinkCombat(entity, run_now);
        return;
    }
    Critter_ThinkIdle(entity, run_now);
}

/* The think function of state 0 (called after the switch, which `case 0:` leaves): gives command
   0x41 and, once in each such run, checks part 0 of melee record 1 for a hit (EntityAi_CheckMeleeHit, work
   byte unk7C). When the entity is pushed and the pause in work byte unk7B is over, it picks a
   heading 90 degrees to the side of the one EntityAi_TurnFromPush gives, give or take 70 degrees (work
   float unk48), turns to it with command bit 0x400 and no movement (work byte unk7A set), then sets
   the pause to EntityParams.action_frames. */
static void Critter_ThinkIdle(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 turn;

    if (ai_state->step_index == 0) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN);
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn != 0 && ai_state->action_flags <= 0) {
            if (turn > 0) {
                direction[0] += HALF_PI;
            } else if (turn < 0) {
                direction[0] -= HALF_PI;
            }
            direction[0] += DEG_TO_RAD((f32)(rand() % 140 - 70));
            ai_state->part_angle_x = EntityMath_WrapAngle(direction[0]);
            ai_state->step_index = 1;
        }
        if (ai_state->action_flags > 0) {
            ai_state->action_flags--;
        }
        if (ai_state->action_counter == 0) {
            ai_state->action_counter = EntityAi_CheckMeleeHit(entity, &entity->params->melee_attacks[1], 0, 1, 1);
        }
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        angle = EntityMath_AngleDifference(entity->model->rotation[1], ai_state->part_angle_x);
        if (ABS(angle) > entity->params->turn_limit) {
            if (angle >= 0.0f) {
                entity->move_command |= (MOVE_COMMAND_TURN_RIGHT | MOVE_COMMAND_TURN_FAST);
            } else {
                entity->move_command |= (MOVE_COMMAND_TURN_LEFT | MOVE_COMMAND_TURN_FAST);
            }
        } else {
            ai_state->step_index = 0;
            ai_state->action_counter = 0;
            ai_state->action_flags = entity->params->action_frames;
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think function of states 1 and 2: as EntityAi_ThinkCombat; it leaves flag bits 9 and 10 as
   they are and never adds command bit 0x40. */
static void Critter_ThinkCombat(Entity *entity, s32 run_now) {
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
