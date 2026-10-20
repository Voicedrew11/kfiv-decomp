/* AncientMummy: the behaviour of entity type 42 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Ancient Mummy. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_collision.h"
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_math.h"
#include "model/motion.h"
#include "player/player.h"

/* Entity.ai_state.move_choice 1: backs away from the player */
#define ANCIENT_MUMMY_MOVE_CHOICE_BACK 1

// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void AncientMummy_Think(Entity *entity, s32 run_now);
static void AncientMummy_ThinkCombat(Entity *entity, s32 run_now);
static s32 AncientMummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach);
static void AncientMummy_ShootAndRelocate(Entity *entity);
static void AncientMummy_FadeBackIn(Entity *entity);

/* Set-up of a type 42 entity: clears its work area and installs the type's handlers. */
void AncientMummy_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = AncientMummy_Think;
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

/* The think handler: as EntityAi_Think, with AncientMummy_ThinkCombat for state 2. Counts down work
   word unk68, the pause before the next AncientMummy_ShootAndRelocate. */
static void AncientMummy_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (ai_state->pause_timer > 0) {
        ai_state->pause_timer--;
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
                AncientMummy_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but with the entity within 40 degrees of
   the player's facing it first starts AncientMummy_ShootAndRelocate when the player is nearer than 300
   and the pause is over, or, with the player nearer than 800 and less than 90 degrees off its
   heading, goes backwards for 90 to 149 frames (work byte unk78 is 1) with the command
   AncientMummy_BackwardsCommand gives. Closing in never runs. */
static void AncientMummy_ThinkCombat(Entity *entity, s32 run_now) {
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
    angle = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));
    if (entity->distance_to_player_3d - entity->radius - 100.0f < 300.0f && ABS(angle) < DEG_TO_RAD(40) && ai_state->pause_timer == 0) {
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        EntityAi_StartAction(entity, AncientMummy_ShootAndRelocate, 1);
        return;
    }
    if (entity->distance_to_player_3d - entity->radius - 100.0f < 800.0f && ABS(angle) < DEG_TO_RAD(40)) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (ABS(angle) < HALF_PI) {
            ai_state->move_choice = ANCIENT_MUMMY_MOVE_CHOICE_BACK;
            entity->target = &player_state;
            entity->combat_choice = COMBAT_CHOICE_CLOSE_IN;
            entity->action_timer = rand() % 60 + 90;
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
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case ANCIENT_MUMMY_MOVE_CHOICE_BACK:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_LEFT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_RIGHT);
                } else {
                    // match: the caller passes an integer argument the callee does not take
                    entity->move_command = AncientMummy_BackwardsCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The movement command for going backwards: a turn bit (0x100 / 0x200) while the target is off the heading by
   more than the turn limit, and command bit 2 (backwards) while it is farther than reach; 0 without a target.
   The second parameter is not used. */
static s32 AncientMummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach) {
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
        command = 0x100;
        if (0.0f <= turn) {
            command = 0x200;
        }
    }
    if (reach < EntityMath_DistanceXZ(entity->model->position, entity->target)) {
        command |= MOVE_COMMAND_BACK;
    }
    return command;
}

/* An action (busy bit 0x1000): plays animation 23 and launches the shots of ranged record 0 on
   their frames. Meanwhile it looks for a point (EntityPoint_PickNearFromDistance, EntityCollision_DistanceToPropFloor) that is more than 90
   degrees off the player's facing; from frame 35 it shrinks the model in x and z, stretches it in y
   and fades it out over EntityParams.far_distance frames, then puts the entity at the point if it found
   one and starts AncientMummy_FadeBackIn. */
static void AncientMummy_ShootAndRelocate(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;
    f32 *position;
    f32 *destination;
    f32 height;
    f32 yaw;
    f32 step;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->hit_strength = 0;
        ai_state->turn_rate = 0.0f;
        ai_state->mode_timer = entity->params->far_distance;
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            ai_state->step_index = 0;
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (ai_state->step_index == 0) {
            position = entity->model->position;
            destination = ai_state->target_point;
            EntityPoint_PickNearFromDistance(destination, position, EntityMath_DistanceXZ(position, (float *)entity->spawn_info), entity->spawn_info->wander_radius);
            height = EntityCollision_DistanceToPropFloor(entity->map_record, destination);
            if (height != ancient_mummy_no_floor_height[0]) {
                yaw = EntityMath_AngleXZ(&player_state.position.x, destination);
                if (EntityMath_WrapAngle(yaw - player_state.rotation.y) < -HALF_PI || EntityMath_WrapAngle(yaw - player_state.rotation.y) > HALF_PI) {
                    ai_state->step_index = 1;
                    ai_state->target_point[1] += height;
                }
            }
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8, entity->model->level_tables->frame);
        if (entity->model->level_tables->frame >= 0x23) {
            ai_state->mode_timer--;
            ENTITY_FLAG_BITS(entity)->bit20 = 1;
            ENTITY_FLAG_BITS(entity)->bit18 = 1;
            ENTITY_FLAG_BITS(entity)->bit21 = 1;
            ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            step = 1.3f / entity->params->far_distance;
            entity->model->scale[0] -= step;
            entity->model->scale[1] += step;
            entity->model->scale[2] -= step;
            entity->model_alpha -= 0x80 / entity->params->far_distance;
        }
        if (ai_state->mode_timer < 0) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->mode_timer = (u16)entity->params->type_param_0 + (u16)entity->params->type_param_1;
            entity->model_alpha = 0;
            if (ai_state->step_index == 1) {
                Vector_Copy(entity->model->position, ai_state->target_point);
            }
            EntityAi_StartAction(entity, AncientMummy_FadeBackIn, 1);
        }
        break;
    }
}

/* An action (busy bit 0x2000) that AncientMummy_ShootAndRelocate starts: the second half of it.
   Waits EntityParams.near_distance frames, turns the entity straight to the player, then over
   EntityParams.type_param_0 frames brings the model's scale and Entity.model_alpha back (to 1.3 and 0x80),
   waits type_param_1 frames more, clears the flag bits the first half set, lets the motion end, goes to
   state 2 and thinks, with the pause in work word unk68 set to EntityParams.action_frames. */
static void AncientMummy_FadeBackIn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 step;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->hit_strength = 0;
        ai_state->sub_timer = 0;
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->action_step++;
        break;
    case 1:
        if (entity->params->near_distance < ai_state->sub_timer++) {
            entity->model->rotation[1] = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            entity->action_step++;
        }
        break;
    case 2:
        ai_state->mode_timer--;
        if (ai_state->mode_timer < 0) {
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->no_player_collision = 0;
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            entity->action_step++;
        } else if (entity->params->type_param_1 < ai_state->mode_timer) {
            step = 1.3f / entity->params->type_param_0;
            entity->model->scale[0] += step;
            entity->model->scale[1] -= step;
            entity->model->scale[2] += step;
            entity->model_alpha += 0x80 / entity->params->type_param_0;
        } else if (ai_state->mode_timer == entity->params->type_param_1) {
            entity->model->scale[0] = 1.3f;
            entity->model->scale[1] = 1.3f;
            entity->model->scale[2] = 1.3f;
            entity->model_alpha = 0x80;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->pause_timer = entity->params->action_frames;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}
