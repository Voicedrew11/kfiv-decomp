/* GreaterDemon: the behaviour of entity type 56 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Greater Demon. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "common.h"
#include "entity/entity_world.h"
#include "entity/entity.h"
#include "map/map.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_collision.h"
#include <math.h>
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_events.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai.h"
#include "entity/entity_math.h"
#include "player/player.h"

/* EntitySpawnInfo.spawn_mode: the spawn that weights its ranged attack 2 (GreaterDemon_SetRanged2Weight) */
#define GREATER_DEMON_SPAWN_MODE_RANGED2 1

static KfVector climb_points[3]; /* 0x4FF390 the points GreaterDemon_Think climbs over (it indexes up to 5) */

// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
static void GreaterDemon_Think(Entity *entity, s32 run_now);
static void GreaterDemon_Move(Entity *entity);
static void GreaterDemon_OnHit(Entity *entity);
static void GreaterDemon_ThinkCombat(Entity *, s32);
static s32 GreaterDemon_ShouldGuard(Entity *, f32, f32);
static int GreaterDemon_SetRanged2Weight(Entity *entity);
static void GreaterDemon_Rise(Entity *);
static void GreaterDemon_SlideToStop(Entity *);
static void GreaterDemon_PlayAnim6Or7BySide(Entity *);
static void GreaterDemon_Guard(Entity *);
static void GreaterDemon_RangedAttack2(Entity *);
static void GreaterDemon_RangedAttack(Entity *);
static void GreaterDemon_DescendAtPlayer(Entity *);
static void GreaterDemon_ReactionHeavy(Entity *);

/* Set-up of a type 56 entity: clears its work area and installs the type's handlers. */
void GreaterDemon_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->ai_state;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = GreaterDemon_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = GreaterDemon_OnHit;
    ai_state->move = GreaterDemon_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    if (ENTITY_FLAG_WORD(entity->spawn_info->spawn_flags) & (0x8000L << 43)) {
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
    ai_state->mode_timer = entity->params->ranged_attacks[2].record.ranged_weight;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

static void GreaterDemon_Think(Entity *entity, s32 run_now) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    MapSlot *slot;
    KfVector *point;
    f32 height;
    f32 no_floor;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (player_state.cast_launched_shot != 0) {
        ai_state->magic_alert_timer = 30;
    }
    if (ai_state->magic_alert_timer > 0) {
        ai_state->magic_alert_timer--;
    }
    if (entity->distance_to_player < entity->radius + 500.0f + 100.0f && ai_state->player_far == 1 && ai_state->sliding == 0) {
        ai_state->player_far = 0;
    } else if (entity->radius + 1000.0f + 100.0f < entity->distance_to_player && ai_state->player_far == 0) {
        ai_state->player_far = 1;
    }
    if (ai_state->climb_point_index < 5) {
        EntityPoint_PickNear(500.0f, &climb_points[ai_state->climb_point_index], &player_state, &player_state);
        slot = &map_slots[current_map_slot];
        height = EntityCollision_DistanceToPropFloor((MapRecord *)slot->records + player_state.map_index, &climb_points[ai_state->climb_point_index]);
        no_floor = greater_demon_no_floor_height_think[0];
        if (height == no_floor) {
            height = EntityCollision_DistanceToPropFloor(entity->map_record, &climb_points[ai_state->climb_point_index]);
            if (height != no_floor) {
                point = &climb_points[ai_state->climb_point_index];
                point->y += height;
                ai_state->climb_point_index++;
            }
        } else {
            point = &climb_points[ai_state->climb_point_index];
            point->y += height;
            ai_state->climb_point_index++;
        }
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
                GreaterDemon_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

static void GreaterDemon_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    f32 direction[4];
    f32 angle;
    f32 turn_left;
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    if (GreaterDemon_ShouldGuard(entity, 200.0f, DEG_TO_RAD(60))) {
        EntityAi_StartAction(entity, GreaterDemon_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (entity->spawn_info->spawn_mode == GREATER_DEMON_SPAWN_MODE_RANGED2) {
        GreaterDemon_SetRanged2Weight(entity);
    } else {
        entity->params->ranged_attacks[2].record.ranged_weight = 0;
    }
    // match: angle is read here before anything has set it
    if (ai_state->sliding == 0 && entity->distance_to_player < entity->radius + 50.0f + 100.0f && entity->action_timer == 0 &&
        rand() % 100 < (s16)entity->params->action_frames && ABS(angle) < DEG_TO_RAD(60)) {
        turn_left = entity->heading_to_player - entity->model->rotation[1];
        if (ABS(turn_left) < DEG_TO_RAD(60)) {
            EntityAi_StartAction(entity, GreaterDemon_PlayAnim6Or7BySide, run_now);
            return;
        }
    }
    side = EntityAi_PickCombatChoice(entity, 2);
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
            if (ai_state->sliding == 1) {
                EntityAi_StartAction(entity, GreaterDemon_SlideToStop, run_now);
            } else if (EntityAi_PickAttack(entity)) {
                if (ENTITY_FLAG_BITS(entity)->attack_index == 2) {
                    EntityAi_StartAction(entity, GreaterDemon_RangedAttack2, run_now);
                } else {
                    EntityAi_StartAction(entity, GreaterDemon_RangedAttack, run_now);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            if (ai_state->player_far == 1 && ai_state->sliding == 0 && EntitySight_EntityInWay(entity) == 0) {
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 0xF0;
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
                EntityAi_StartAction(entity, GreaterDemon_Rise, run_now);
                return;
            }
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 0x78;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 0x1E;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (entity->combat_choice == COMBAT_CHOICE_CLOSE_IN) {
        turn = EntityAi_TurnFromPush(entity, direction);
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
        } else if (ai_state->player_far == 1) {
            if (EntitySight_EntityInWay(entity) == 0 && ai_state->sliding == 0) {
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 0xF0;
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
                EntityAi_StartAction(entity, GreaterDemon_Rise, run_now);
                return;
            }
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
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

static void GreaterDemon_Move(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    EntityParams *params;
    f32 turn;

    if (entity == player_state.stepped_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
    params = entity->params;
    if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= params->turn_step_per_frame;
        if (ai_state->turn_rate < -params->turn_limit) {
            ai_state->turn_rate = -params->turn_limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += params->turn_step_per_frame;
        if (params->turn_limit < ai_state->turn_rate) {
            ai_state->turn_rate = params->turn_limit;
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
            Motion_Start(entity, 4, 0, params->motion_blend_frames, 0);
        } else {
            Motion_Start(entity, 3, 0, params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_DOWN) {
        Motion_Start(entity, 8, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        Motion_Start(entity, 9, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, params->motion_blend_frames, 0);
    }
}

/* An action (busy bits 0x3000) that GreaterDemon_ThinkCombat starts: plays animation 8 turning to
   Entity.heading_to_player while it blends in; from frame 41 it moves the entity up, by work float unk48 a
   frame, which grows by 0.8 to 12 (unk88 is given minus unk48; y grows downward, EntityCollision_DistanceToPropFloor).
   When the motion has ended it sets work byte unk7B and starts GreaterDemon_DescendAtPlayer. */
static void GreaterDemon_Rise(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= (ENTITY_BUSY_1000 | ENTITY_BUSY_2000);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->draw_afterimage = 0;
        ai_state->part_angle_x = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        return;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->action_flags = 1;
            EntityAi_StartAction(entity, GreaterDemon_DescendAtPlayer, 1);
        }
        if (entity->model->level_tables->frame >= 0x29) {
            float next_unk48;
            entity->move_delta_y = -ai_state->part_angle_x;
            next_unk48 = ai_state->part_angle_x + 0.8f;
            ai_state->part_angle_x = next_unk48;
            if (next_unk48 > 12.0f) ai_state->part_angle_x = 12.0f;
        }
        break;
    }
}

/* An action (busy bit 0x4000) that GreaterDemon_Rise starts. In animation 17 it goes on moving up
   while EntityCollision_DistanceToPropFloor, the distance to the floor below, reads under 220, and turns to Entity.heading_to_player;
   once the motion has ended and the heading is within 20 degrees of unk68 it plays animations 18
   and 19 and, from frame 16 of 18, moves the entity along its heading at work float unk44 (12,
   growing by 9 percent a frame up to 80) and down at unk48 (20, falling by 0.8 to 3). When flag bit
   44, 45, 46, 48 or 49 is set it stops, checks part 0 of the third melee record
   (EntityParams.melee_attack_third) for a hit if it was bit 49, and starts GreaterDemon_SlideToStop. */
static void GreaterDemon_DescendAtPlayer(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 flag;

    entity->busy_flags |= ENTITY_BUSY_4000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, 10, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        ai_state->reload_timer = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        entity->move_delta_y = -ai_state->part_angle_x;
        ai_state->part_angle_x += 0.8f;
        if (ai_state->part_angle_x > 12.0f) {
            ai_state->part_angle_x = 12.0f;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < DEG_TO_RAD(20)) {
                Motion_Start(entity, 0x12, 0, 1, 1);
                entity->action_step++;
                ai_state->part_angle_x = 20.0f;
                ai_state->move_speed = 12.0f;
            }
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        if (EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position) < 220.0f) {
            entity->move_delta_y = -ai_state->part_angle_x;
            ai_state->part_angle_x += 0.8f;
            if (ai_state->part_angle_x > 12.0f) {
                ai_state->part_angle_x = 12.0f;
            }
        }
        break;
    case 3:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            Motion_Start(entity, 0x13, 0, 10, 1);
            entity->action_step++;
        }
        if (entity->model->level_tables->frame >= 0x10) {
            entity->move_delta_x = -sinf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->move_speed;
            entity->move_delta_y = ai_state->part_angle_x;
            entity->move_delta_z = -cosf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->move_speed;
            ai_state->move_speed *= 1.09f;
            ai_state->part_angle_x -= 0.8f;
            if (ai_state->move_speed > 80.0f) {
                ai_state->move_speed = 80.0f;
            }
            if (ai_state->part_angle_x < 3.0f) {
                ai_state->part_angle_x = 3.0f;
            }
        }
        break;
    case 5:
        if (ENTITY_FLAG_BITS(entity)->on_floor == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->wall_push == 1 || ENTITY_FLAG_BITS(entity)->player_push == 1
            || ENTITY_FLAG_BITS(entity)->entity_push == 1) {
            EntityParamsUnk240 *unk350;

            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->busy_flags = 0;
            unk350 = (EntityParamsUnk240 *)entity->params->melee_attack_third;
            entity->move_command = 0;
            entity->action_timer = 0;
            flag = ENTITY_FLAG_BITS(entity)->player_push;
            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                ai_state->reload_timer = flag;
            }
            EntityAi_StartAction(entity, GreaterDemon_SlideToStop, 1);
            return;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->action_step++;
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
        }
        entity->move_delta_x = -sinf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->move_speed;
        entity->move_delta_y = ai_state->part_angle_x;
        entity->move_delta_z = -cosf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->move_speed;
        ai_state->move_speed *= 1.09f;
        ai_state->part_angle_x -= 0.8f;
        if (ai_state->move_speed > 80.0f) {
            ai_state->move_speed = 80.0f;
        }
        if (ai_state->part_angle_x < 3.0f) {
            ai_state->part_angle_x = 3.0f;
        }
        break;
    case 6:
        if (ENTITY_FLAG_BITS(entity)->on_floor == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->wall_push == 1 || ENTITY_FLAG_BITS(entity)->player_push == 1
            || ENTITY_FLAG_BITS(entity)->entity_push == 1) {
            EntityParamsUnk240 *unk350;

            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->busy_flags = 0;
            unk350 = (EntityParamsUnk240 *)entity->params->melee_attack_third;
            entity->move_command = 0;
            entity->action_timer = 0;
            flag = ENTITY_FLAG_BITS(entity)->player_push;
            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                ai_state->reload_timer = flag;
            }
            EntityAi_StartAction(entity, GreaterDemon_SlideToStop, 1);
            return;
        }
        entity->move_delta_x = -sinf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->move_speed;
        entity->move_delta_y = ai_state->part_angle_x;
        entity->move_delta_z = -cosf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->move_speed;
        ai_state->move_speed *= 1.09f;
        ai_state->part_angle_x -= 0.8f;
        if (ai_state->move_speed > 80.0f) {
            ai_state->move_speed = 80.0f;
        }
        if (ai_state->part_angle_x < 3.0f) {
            ai_state->part_angle_x = 3.0f;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        return;
    }
    return; // match: with this the call above becomes a tail call
}

/* An action (busy bits 0x5000) that GreaterDemon_DescendAtPlayer starts (and
   GreaterDemon_ThinkCombat, while work byte unk7B is 1): plays animation 29 to its end while the
   entity goes on along its heading at work float unk44, which falls by 2 a frame to 0. Once, when
   flag bit 49 is set, it checks part 0 of the third melee record (EntityParams.melee_attack_third) for a hit,
   unless GreaterDemon_DescendAtPlayer did. At the end it clears unk7B and thinks. */
static void GreaterDemon_SlideToStop(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    EntityParamsUnk240 *unk350 = (EntityParamsUnk240 *)entity->params->melee_attack_third;

    entity->busy_flags |= (ENTITY_BUSY_1000 | ENTITY_BUSY_4000);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x1D, 0, 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->flags |= ENTITY_FLAG_DRAW_AFTERIMAGE;
            entity->action_step++;
        }
        if (ai_state->reload_timer == 0) {
            s32 flag = ENTITY_FLAG_BITS(entity)->player_push;

            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                ai_state->reload_timer = flag;
            }
        }
        break;
    case 2:
        if (ai_state->reload_timer == 0) {
            s32 flag = ENTITY_FLAG_BITS(entity)->player_push;

            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                ai_state->reload_timer = flag;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->sliding = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        entity->move_delta_x = -sinf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->unk44;
        entity->move_delta_z = -cosf(EntityMath_WrapAngle(entity->model->rotation[1])) * ai_state->unk44;
        ai_state->unk44 -= 2.0f;
        if (ai_state->unk44 < 0.0f) {
            ai_state->unk44 = 0.0f;
        }
        break;
    }
}

/* An action (busy bit 0x1000) that GreaterDemon_ThinkCombat starts: plays animation 6 or 7, by the
   side of the player's facing the entity is on, to its end, turning to Entity.heading_to_player throughout;
   then thinks. Animations 6 and 7 are the ones EntityAi_Move plays for the sideways commands. */
static void GreaterDemon_PlayAnim6Or7BySide(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        if (0.0f < EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position)) * RAD_TO_DEG) {
            Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 1);
        } else {
            Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 1);
        }
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        break;
    case 2:
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
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

/* When the state field in flags is 2, resets the entity and registers GreaterDemon_ReactionHeavy; then sets state 2. */
static void GreaterDemon_OnHit(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_LowerWeightsAfterHit(entity);
        entity->target = NULL;
        entity->flags &= ~ENTITY_FLAG_ATTACKING;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        EntityAi_SetReaction(entity, GreaterDemon_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
}

static void GreaterDemon_ReactionHeavy(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case 0:
        Motion_Start(entity, 0xF, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = 0;
        ENTITY_FLAG_BITS(entity)->guarding = 0;
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
            ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
        }
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->reaction_step++;
        ai_state->sliding = 0;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->reaction_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
            EntityAi_SetReaction(entity, NULL, 0);
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

static void GreaterDemon_Guard(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_GUARD;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xD, 0, 0xA, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_timer = entity->params->guard_frames;
            entity->flags |= ENTITY_FLAG_GUARDING;
            entity->flags &= ~ENTITY_FLAG_ANIMATES;
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->hit_by_player && ENTITY_FLAG_BITS(entity)->hit_source_magic && !ENTITY_FLAG_BITS(entity)->hit_source_weapon) {
            entity->flags &= ~ENTITY_FLAG_HIT_BY_PLAYER;
            entity->action_timer = entity->params->guard_frames;
        }
        EntityEvent_StartEffectAtPoint((char **)entity, 0x32F, 0, (int)entity->model->rotation);
        if (entity->action_timer == 0) {
            ENTITY_FLAG_BITS(entity)->guarding = 0;
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
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

/* Helper of GreaterDemon_ThinkCombat: whether to start GreaterDemon_Guard. Only when the entity is
   not busy, the player is at least min_gap away (less the radius and 100), Entity.heading_to_player is within
   max_angle of the heading, the entity is within 45 degrees of the player's facing and work byte
   unk7C is above 0; then EntityParams.guard_chance times in 100. */
static s32 GreaterDemon_ShouldGuard(Entity *entity, f32 min_gap, f32 max_angle) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    f32 gap = entity->distance_to_player - (entity->radius + 100.0f);
    f32 angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    f32 facing = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));

    if (entity->busy_flags != 0 || gap < min_gap || max_angle < ABS(angle) || DEG_TO_RAD(45) < ABS(facing) || ai_state->magic_alert_timer <= 0) {
        return FALSE;
    }
    return rand() % 100 < entity->params->guard_chance;
}

static void GreaterDemon_RangedAttack2(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    EntityParams *params = entity->params;
    s32 kind = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(params, kind);
    MapSlot *slot;
    KfVector *floor_point;
    KfVector *point;
    Entity *minion;
    EntityModel *minion_model;
    f32 height;
    f32 no_floor;
    f32 spin;
    s32 minion_index;
    s32 point_index;
    s32 count;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->climb_point_index = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (ai_state->climb_point_index < 5) {
            EntityPoint_PickNear(500.0f, &climb_points[ai_state->climb_point_index], &player_state, &player_state);
            slot = &map_slots[current_map_slot];
            height = EntityCollision_DistanceToPropFloor((MapRecord *)slot->records + player_state.map_index, &climb_points[ai_state->climb_point_index]);
            no_floor = greater_demon_no_floor_height_attack[0];
            if (height == no_floor) {
                height = EntityCollision_DistanceToPropFloor(entity->map_record, &climb_points[ai_state->climb_point_index]);
                if (height == no_floor) {
                    break;
                }
            }
            floor_point = &climb_points[ai_state->climb_point_index];
            floor_point->y += height;
            ai_state->climb_point_index++;
        }
        break;
    case 2:
        ai_state->turn_rate = 0.0f;
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->level_tables->frame_fixed - params->motion_speeds[entity->animation_index]) >> 8,
                      entity->model->level_tables->frame);
        if (entity->model->level_tables->frame == 0x11) {
            minion_index = 0;
            count = rand() % 5 + 1;
            if (ai_state->climb_point_index < count) {
                count = ai_state->climb_point_index;
            }
            for (point_index = 0; point_index < count; point_index++) {
                for (; minion_index < 8; minion_index++) {
                    minion = Entity_GetBySpawnId(minion_index + 10);
                    if (minion != NULL && (minion->flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE)) == 1 &&
                        (ENTITY_FLAG_BITS(minion)->dying == 0 || ENTITY_FLAG_BITS(minion)->dying == 3)) {
                        point = &climb_points[point_index];
                        if (EntityMath_DistanceXZ(&point->x, &player_state.position.x) > 180.0f) {
                            EntityEvent_Respawn(minion);
                            minion->model->position[0] = point->x;
                            minion->model->position[1] = point->y;
                            minion->model->position[2] = point->z;
                            minion->model->rotation[1] = EntityMath_WrapAngle(DEG_TO_RAD((f32)(rand() % 360)));
                            minion_model = minion->model;
                            spin = minion_model->scale[0];
                            if (rand() % 3 != 0) {
                                spin = -spin;
                            }
                            minion_model->scale[0] = spin;
                            break;
                        }
                    }
                }
            }
        }
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

/* Helper of GreaterDemon_ThinkCombat (spawn mode 1 only): counts those of the entities with spawn
   ids 10 to 17 that have flag bit 0 set, bit 1 clear and flag bits 30-31 at 0 or 3, and gives
   ranged record 2 the weight kept in work area unk68[2] times that count over 8. Returns 0. */
static int GreaterDemon_SetRanged2Weight(Entity *entity) {
    EntityAiStateKind17FC60 *work;
    Entity *ally;
    s32 ally_index;
    s32 ally_count;
    u64 ally_flags;
    s32 flag_bits;

    work = (EntityAiStateKind17FC60 *)&entity->ai_state;
    ally_count = 0;
    for (ally_index = 0; ally_index < 8; ally_index++) {
        ally = Entity_GetBySpawnId(ally_index + 0xA);
        if (ally != NULL) {
            ally_flags = ally->flags;
            if ((ally_flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE)) == 1) {
                flag_bits = (s32)((ally_flags << 0x1E) >> 0x20); // match: bits 30-31 of the flag word
                if (((flag_bits & 3) == 0) || ((flag_bits & 3) == 3)) {
                    ally_count++;
                }
            }
        }
    }
    entity->params->ranged_attacks[2].record.ranged_weight = (f32)work->action_weights[2] * (f32)ally_count * 0.125f;
    return 0;
}

static void GreaterDemon_RangedAttack(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    s32 kind = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[3];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
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
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        direction[0] = 0.0f;
        direction[1] = EntityMath_WrapAngle(entity->model->rotation[1] + PI);
        direction[2] = 0.0f;
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction);
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
