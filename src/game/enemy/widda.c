/* Widda: the behaviour of entity type 19 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Widda Large, Widda. The game names
   no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records
   by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include <math.h>
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"
#include "entity/entity_math.h"
#include "player/player.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

static void Widda_Animate(Entity *entity);
static void Widda_AimPartsAtTarget(Entity *entity);
static void Widda_Think(Entity *entity, s32 run_now);
static void Widda_ThinkCombat(Entity *entity, s32 run_now);
static void Widda_Move(Entity *entity);

/* Set-up of a type 19 entity: clears its work area and installs the type's handlers. */
void Widda_Init(Entity *entity) {
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
    ai_state->think = Widda_Think;
    ai_state->animate = Widda_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = Widda_Move;
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

/* The animate handler: as EntityAi_Animate, and Widda_AimPartsAtTarget turns two model parts.
   */
static void Widda_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        Widda_AimPartsAtTarget(entity);
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

/* Helper of Widda_Animate: turns the two model parts EntityParams.near_distance and unk438 name
   towards the target, pitch from the player's height (at most 30 degrees) and yaw from the bearing
   (at most 40), each part by half of it; the angles are work floats unk48 to unk54. Without a
   target or while the entity is busy it only reads the parts' angles into them. */
static void Widda_AimPartsAtTarget(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->parts;
    s32 first = entity->params->near_distance;
    s32 second = entity->params->far_distance;
    f32 angle;
    f32 bearing;
    f32 limit;

    if (entity->target == NULL || entity->busy_flags != 0) {
        ai_state->part_angle_x = parts[first].part_angles[0];
        ai_state->part_angle_y = parts[first].part_angles[1];
        ai_state->part_angle_z = parts[second].part_angles[0];
        ai_state->pitch_speed = parts[second].part_angles[1];
    } else {
        angle = atan2f(player_state.position.y - entity->model->position[1], entity->distance_to_player);
        limit = DEG_TO_RAD(30);
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        EntityMath_EaseAngleToward(&ai_state->part_angle_x, angle * 0.5f, 10.0f);
        EntityMath_EaseAngleToward(&ai_state->part_angle_z, angle * 0.5f, 10.0f);
        parts[first].part_angles[0] = ai_state->part_angle_x;
        parts[second].part_angles[0] = ai_state->part_angle_z;
        bearing = EntityMath_HeadingTo(entity->model->position, entity->target);
        angle = EntityMath_AngleDifference(entity->model->rotation[1], bearing);
        limit = DEG_TO_RAD(40);
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        EntityMath_EaseAngleToward(&ai_state->part_angle_y, angle * 0.5f, 10.0f);
        EntityMath_EaseAngleToward(&ai_state->pitch_speed, angle * 0.5f, 10.0f);
        parts[first].part_angles[1] = ai_state->part_angle_y;
        parts[second].part_angles[1] = ai_state->pitch_speed;
    }
}

/* The think handler: as EntityAi_Think, with Widda_ThinkCombat for state 2. Sets work byte
   unk7A when flag bit 45, 47 or 48 is set during a dodge (busy bit 8). */
static void Widda_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if ((entity->busy_flags & ENTITY_BUSY_DODGE) && (entity->flags & (ENTITY_FLAG_WALL_PUSH | ENTITY_FLAG_OBJECT_PUSH | ENTITY_FLAG_ENTITY_PUSH))) {
        ai_state->step_index = 1;
    }
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
    case 0:
        EntityAi_ThinkIdle(entity, run_now);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, run_now);
        return;
    case 2:
        Widda_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but closing in runs (work byte unk78 is
   1) once the player is farther than 900 and within 40 degrees ahead. Every tenth frame, with the
   player near and the entity within 40 degrees of the player's facing, a chance of
   EntityParams.action_frames in 100 makes it start EntityAi_Dodge when the player is 40 to 90 degrees off
   its heading, or close in at the run when the player is farther off or work byte unk7A is set. */
static void Widda_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(40))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    side = EntityAi_PickCombatChoice(entity, 0);
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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case 1:
            entity->target = &player_state;
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (entity->distance_to_player > 900.0f && ABS(angle) < DEG_TO_RAD(40)) {
                entity->action_timer = 300;
                ai_state->move_choice = 1;
            } else {
                entity->action_timer = rand() % 60 + 120;
                ai_state->move_choice = 0;
            }
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
    angle = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ((float *)&player_state.position, entity->model->position));
    if (entity->distance_to_player_3d - entity->radius - 100.0f < 200.0f && ABS(angle) < DEG_TO_RAD(40)
        && frame_counter % 10U == entity->spawn_index % 10U && rand() % 100 < ENTITY_CONTROLLER(entity)->action_frames) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (ai_state->step_index == 0) {
            if (ABS(angle) > DEG_TO_RAD(40) && ABS(angle) < HALF_PI) {
                EntityAi_StartAction(entity, EntityAi_Dodge, 1);
                ai_state->step_index = 0;
                return;
            }
            if (ABS(angle) >= HALF_PI) {
                entity->prev_combat_choice = entity->combat_choice;
                entity->target = &player_state;
                entity->action_timer = 300;
                ai_state->move_choice = entity->combat_choice = 1;
                ai_state->step_index = 0;
            }
        } else {
            entity->prev_combat_choice = entity->combat_choice;
            entity->target = &player_state;
            entity->action_timer = 300;
            ai_state->move_choice = entity->combat_choice = 1;
            ai_state->step_index = 0;
        }
    }
    if (entity->combat_choice == 1) {
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
            case 0:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
                if (entity->distance_to_player > 900.0f && ABS(angle) < DEG_TO_RAD(40)) {
                    entity->action_timer = 300;
                    ai_state->move_choice = 1;
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The move handler: as EntityAi_Move, but it only turns the entity (twice as fast for command bits
   0x40 and 0x400) and picks the animation; it does not move it. */
static void Widda_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 turn;

    if (entity == (Entity *)entity_under_player[0] && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
    if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
        if (ai_state->turn_rate < -entity->params->turn_limit) {
            ai_state->turn_rate = -entity->params->turn_limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
        if (ai_state->turn_rate > entity->params->turn_limit) {
            ai_state->turn_rate = entity->params->turn_limit;
        }
    } else {
        ai_state->turn_rate = 0.0f;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & (MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_FAST)) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_DOWN) {
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}
