/* Spider: the behaviour of entity type 3 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Spider, Blue Spider. The game names
   no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records
   by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_collision.h"
#include "entity/entity_events.h"
#include <libvu0.h>
#include "math/vu0.h"
#include "player/player.h"
#include <stdlib.h>
#include "entity/entity_math.h"
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"

/* Entity.ai_state.move_choice 1: walks to the point near the spawn, stopping 50 short of it */
#define SPIDER_MOVE_CHOICE_TO_POINT 1

static void Spider_Animate(Entity *entity);
static void Spider_Think(Entity *entity, s32 run_now);
static void Spider_ThinkWander(Entity *entity, s32 run_now);
static void Spider_ThinkCombat(Entity *entity, s32 run_now);
static void Spider_RangedAttack(Entity *entity);
static void Spider_Dormant(Entity *entity);
static void Spider_OnHit(Entity *entity);
static void Spider_Death(Entity *entity);
static void Spider_DropToFloor(Entity *entity);
static void Spider_LevelOut(Entity *entity);

/* Set-up of a type 3 entity: clears its work area and installs the type's handlers (one handler table when it
   starts with its action, another when it starts asleep). */
void Spider_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        ai_state->think = Spider_Think;
        ai_state->animate = Spider_Animate;
        ai_state->on_hit = EntityAi_OnHit;
        ai_state->move = EntityAi_Move;
        ai_state->dormant = Spider_Dormant;
        ai_state->death = EntityAi_Death;
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        ai_state->think = Spider_Think;
        ai_state->animate = Spider_Animate;
        ai_state->on_hit = Spider_OnHit;
        ai_state->move = EntityAi_Move;
        ai_state->dormant = Spider_Dormant;
        ai_state->death = Spider_Death;
        EntityAi_StartAction(entity, Spider_DropToFloor, 1);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler (installed in both branches of the set-up function): as EntityAi_Animate
   without the test of the EntityParams kind bits and without the head turn; while work byte unk7A
   and flag bit 53 are both set it reverses this frame's turn, Entity.turn_delta_yaw. */
static void Spider_Animate(Entity *entity) {
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
    }
    if (ai_state->step_index != 0) {
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            entity->turn_delta_yaw = -entity->turn_delta_yaw;
        }
    }
}

/* The think handler (installed in both branches of the set-up function): as EntityAi_Think, with an
   if in place of the switch: Spider_ThinkCombat in state 2, Spider_ThinkWander in the other states.
   Before that, while work byte unk7A is set (Spider_DropToFloor sets it), it clears it and starts
   Spider_LevelOut once the player is within EntityParams.yaw_limit_deg. */
static void Spider_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    if (ai_state->step_index != 0 && entity->distance_to_player < (float)entity->params->yaw_limit_deg) {
        ai_state->step_index = 0;
        EntityAi_StartAction(entity, Spider_LevelOut, 1);
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
    } else if (entity->state == ENTITY_STATE_NOTICED) {
        Spider_ThinkCombat(entity, run_now);
    } else {
        Spider_ThinkWander(entity, run_now);
    }
}

/* The think function of every state but 2 (Spider_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 59 times in 100) or no
   target, takes the command of EntityAi_MoveCommand towards it (or the turn away from a push,
   EntityAi_TurnFromPush) and starts the move action. */
static void Spider_ThinkWander(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    if (entity->action_timer == 0) {
        if (rand() % 100 > 40) {
            EntityPoint_PickNear(entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
            entity->target = ai_state->target_point;
            entity->action_timer = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->action_timer = rand() % 30 + 60;
        }
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

/* The think function of state 2 (called under `entity->state == ENTITY_STATE_NOTICED`, not from a switch): as
   EntityAi_ThinkCombat with Spider_RangedAttack as the ranged attack; the head does not
   follow. It closes in on the player only while the player is within EntitySpawnInfo.wander_radius of the
   spawn point; otherwise it goes for a point near the spawn point. */
static void Spider_ThinkCombat(Entity *entity, s32 run_now) {
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
                EntityAi_StartAction(entity, Spider_RangedAttack, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            if (EntityMath_DistanceXZ(&player_state.position.x, &entity->spawn_info->spawn_x) < (f32)entity->spawn_info->wander_radius) {
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 120;
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            } else {
                EntityPoint_PickNear(entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
                entity->target = ai_state->target_point;
                entity->action_timer = rand() % 60 + 120;
                ai_state->move_choice = SPIDER_MOVE_CHOICE_TO_POINT;
            }
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
    if (entity->combat_choice == COMBAT_CHOICE_CLOSE_IN) {
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
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
            case SPIDER_MOVE_CHOICE_TO_POINT:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
                }
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, the shots turned towards a point 30
   below the player (EntityShot_LaunchAimed). */
static void Spider_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + ENTITY_MOTION_RANGED_ATTACK, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(direction, &player_state);
        direction[1] -= 30.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction, DEG_TO_RAD(45), AIMED_SHOT_TURN_RATE);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
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

/* The dormant action (busy bit 0x20), installed in both branches of the set-up function and started
   when the spawn has flag bit 58: plays animation 3 until the entity is EntityParams.pitch_limit_deg or more
   from its spawn point (EntityMath_DistanceXZ), then thinks. It does not wait for the player. */
static void Spider_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 3, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) >= (f32)entity->params->pitch_limit_deg) {
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
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

/* The on_hit handler that the set-up function installs when the spawn has flag bit 58 clear, and
   Spider_DropToFloor again when the drop ends on result 2 or 3; otherwise, and once Spider_LevelOut has
   ended, the slot holds EntityAi_OnHit. No reaction: it starts Spider_LevelOut and goes to state 2.
   */
static void Spider_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    EntityAi_StartAction(entity, Spider_LevelOut, 0);
    EntityAi_SetState(entity, 2);
}

/* The death action (busy bit 0x40), installed and replaced together with Spider_OnHit
   (otherwise the slot holds EntityAi_Death): as EntityAi_Death, while it brings the model's pitch
   and roll back to 0; it clears flag bits 22 to 24 and 27 at the start and sets 26 and 27 for the
   fade. */
static void Spider_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    EntityMath_EaseAngleToward(&entity->model->rotation[0], 0.0f, 6.0f);
    EntityMath_EaseAngleToward(&entity->model->rotation[2], 0.0f, 6.0f);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}

/* An action (busy bit 0x1000): drops 20 a frame along its own up axis until a prop is hit within 200
   (EntityCollision_RayProps). On result 1 it stands there with the shared on_hit and death; on result 2 or 3 it
   clings there (work byte unk7A, Spider_OnHit and Spider_Death installed). Then it thinks. */
static void Spider_DropToFloor(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 landing[4];
    f32 direction[4];
    f32 matrix[16];
    s32 hit;

    ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    direction[0] = 0.0f;
    direction[1] = 1.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    entity->busy_flags = ENTITY_BUSY_1000;
    sceVu0RotMatrixZ(matrix, identity_matrix, entity->model->rotation[2]);
    sceVu0RotMatrixX(matrix, matrix, entity->model->rotation[0]);
    sceVu0RotMatrixY(matrix, matrix, entity->model->rotation[1]);
    sceVu0ApplyMatrix(direction, (int *)matrix, direction);
    hit = EntityCollision_RayProps(entity->model->position, direction, landing, 200.0f);
    if (hit == 1) {
        Vector_Copy(entity->model->position, landing);
        ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ai_state->step_index = 0;
        ai_state->on_hit = EntityAi_OnHit;
        ai_state->death = EntityAi_Death;
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        if (ENTITY_FLAG_BITS(entity)->own_think) {
            ai_state->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
    } else if (hit == 2 || hit == 3) {
        Vector_Copy(entity->model->position, landing);
        ai_state->part_angle_x = landing[0];
        ai_state->part_angle_y = landing[1];
        ai_state->part_angle_z = landing[2];
        ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ai_state->step_index = 1;
        ai_state->on_hit = Spider_OnHit;
        ai_state->death = Spider_Death;
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        if (ENTITY_FLAG_BITS(entity)->own_think) {
            ai_state->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
    } else {
        Vector_Scale(direction, direction, 20.0f);
        Vector_Add(entity->model->position, entity->model->position, direction);
    }
}

/* An action (busy bit 0x2000): eases the model's pitch and roll back to 0; once both are there and flag bit 44
   or 50 is set, it puts back the shared on_hit and death and starts the move action. */
static void Spider_LevelOut(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 pitch_left;
    f32 roll_left;

    ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->animates = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    entity->busy_flags = ENTITY_BUSY_2000;
    pitch_left = EntityMath_EaseAngleToward(&entity->model->rotation[0], 0.0f, 6.0f);
    roll_left = EntityMath_EaseAngleToward(&entity->model->rotation[2], 0.0f, 6.0f);
    if (entity->flags & (ENTITY_FLAG_ON_FLOOR | ENTITY_FLAG_ON_TOP)) {
        pitch_left = ABS(pitch_left);
        roll_left = ABS(roll_left);
        if (ABS(pitch_left) < 0.01f && ABS(roll_left) < 0.01f) {
            entity->model->rotation[0] = 0.0f;
            entity->model->rotation[2] = 0.0f;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ai_state->on_hit = EntityAi_OnHit;
            ai_state->death = EntityAi_Death;
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
    }
}
