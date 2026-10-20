/* DarkSoldier: the behaviour of entity type 44 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Soldier. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_math.h"
#include "player/player.h"
#include "entity/entity_collision.h"

/* Entity.combat_choice 2: picks a point near the spawn and closes in on it */
#define DARK_SOLDIER_COMBAT_CHOICE_NEAR_SPAWN 2

#define ENTITY_WORK_AREA_SIZE 0xA0

/* Entity.action_step in DarkSoldier_MoveAlongArc */
#define DARK_SOLDIER_ARC_START      0
#define DARK_SOLDIER_ARC_TURN       1   /* turns to the heading, then starts the arc */
#define DARK_SOLDIER_ARC_RUN        2   /* moves along the arc until animation 17 ends */
#define DARK_SOLDIER_ARC_BLEND_OUT  3   /* moves along the arc until the blend ends */
#define DARK_SOLDIER_ARC_LEAP       4   /* moves along the arc until it is close to the target */
#define DARK_SOLDIER_ARC_FALL       5   /* falls until it lands (animation 9) */
#define DARK_SOLDIER_ARC_LAND_NOW   6   /* lands at once if it is already on the floor */
#define DARK_SOLDIER_ARC_LANDING    7   /* waits to land, then ends */
#define DARK_SOLDIER_ARC_END        8   /* the action ends: thinks, or starts the move or melee */

/* Entity.action_step in DarkSoldier_Dodge */
#define DARK_SOLDIER_DODGE_START    0   /* plays animation 6 or 7 by the side of the player */
#define DARK_SOLDIER_DODGE_TURN     1   /* blends in while turning to the player */
#define DARK_SOLDIER_DODGE_FALL     2   /* moves down with animation 9 */
#define DARK_SOLDIER_DODGE_FALLING  3   /* moves down until the blend ends */
#define DARK_SOLDIER_DODGE_LANDING  4   /* moves down until it lands */
#define DARK_SOLDIER_DODGE_END      5   /* the action ends: thinks, or starts the move */

/* EntitySpawnInfo.spawn_mode: the default spawn, the only one that may move along the arc */
#define DARK_SOLDIER_SPAWN_MODE_DEFAULT 0
/* one frame of the arc of DarkSoldier_MoveAlongArc: forwards at unk44 along unk30, in y at minus
   unk48 (up); both wear off */
static inline void move_along_arc(Entity *entity, EntityAiState *ai_state) {
    entity->move_delta_x = ai_state->move_speed * ai_state->target_point[0];
    entity->move_delta_y = -ai_state->part_angle_x;
    entity->move_delta_z = ai_state->move_speed * ai_state->target_point[2];
    ai_state->part_angle_x -= entity->params->type_param_2 * 0.1f;
    ai_state->move_speed -= 0.5f;
    if (ai_state->move_speed < 0.0f) {
        ai_state->move_speed = 0.0f;
    }
}

static void DarkSoldier_Think(Entity *entity, s32 run_now);
static void DarkSoldier_ThinkCombat(Entity *entity, s32 run_now);
static void DarkSoldier_Move(Entity *entity);
static void DarkSoldier_MeleeAttack(Entity *entity);
static void DarkSoldier_PlayAnim6Or7BySide(Entity *entity);
static void DarkSoldier_MoveAlongArc(Entity *entity);
static void DarkSoldier_Dodge(Entity *entity);
static void DarkSoldier_OnHit(Entity *entity);
static s32 DarkSoldier_CanMoveAlongArc(Entity *entity);

/* Set-up of a type 44 entity: clears its work area and installs the type's handlers. */
void DarkSoldier_Init(Entity *entity) {
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
    ai_state->think = DarkSoldier_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = DarkSoldier_OnHit;
    ai_state->move = DarkSoldier_Move;
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
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with DarkSoldier_ThinkCombat for state 2; walking back to
   the spawn point (flag bit 53) gets command bits 0x440 added. Work byte unk7A goes to 0 with the
   player nearer than the radius plus 400 and to 1 with the player farther than the radius plus
   1100. */
static void DarkSoldier_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->distance_to_player < entity->radius + 300.0f + 100.0f && ai_state->step_index == 1) {
        ai_state->step_index = 0;
    } else if (entity->radius + 1000.0f + 100.0f < entity->distance_to_player && ai_state->step_index == 0) {
        ai_state->step_index = 1;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
            entity->move_command |= (MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_FAST);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            case ENTITY_STATE_SEARCHING:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                DarkSoldier_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but the dodge is DarkSoldier_Dodge, and
   each time the timer is at 0 it may first start DarkSoldier_PlayAnim6Or7BySide (a chance of
   EntityParams.near_distance in 100, the entity within 60 degrees of the player's facing and the player
   within 60 of its heading) or DarkSoldier_MoveAlongArc (a chance of unk438 in 100, within 90
   degrees of the player's facing, EntitySpawnInfo.spawn_mode at 0 and DarkSoldier_CanMoveAlongArc
   true). A melee attack draws for the same two actions first, with work byte unk7B at 1 so that the
   attack follows them. In place of waiting it goes to a point EntityPoint_PickNear picks round the spawn
   point; closing in is at the run with command bit 0x400. */
static void DarkSoldier_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 point[4];
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
        EntityAi_StartAction(entity, DarkSoldier_Dodge, 1);
        return;
    }
    if (entity->action_timer == 0) {
        angle = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));
        if (rand() % 100 < entity->params->near_distance && ABS(angle) < DEG_TO_RAD(60)
            && ABS(entity->heading_to_player - entity->model->rotation[1]) < DEG_TO_RAD(60)) {
            ai_state->action_flags = 0;
            EntityAi_StartAction(entity, DarkSoldier_PlayAnim6Or7BySide, run_now);
            return;
        }
        if (rand() % 100 < entity->params->far_distance && ABS(angle) < HALF_PI && entity->spawn_info->spawn_mode == DARK_SOLDIER_SPAWN_MODE_DEFAULT
            && DarkSoldier_CanMoveAlongArc(entity)) {
            ai_state->action_flags = 0;
            EntityAi_StartAction(entity, DarkSoldier_MoveAlongArc, run_now);
            return;
        }
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
                return;
            }
            angle = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));
            if (rand() % 100 < entity->params->near_distance && ABS(angle) < DEG_TO_RAD(40)) {
                ai_state->action_flags = 1;
                EntityAi_StartAction(entity, DarkSoldier_PlayAnim6Or7BySide, run_now);
                return;
            }
            if (rand() % 100 < entity->params->far_distance && ABS(angle) < HALF_PI && entity->spawn_info->spawn_mode == DARK_SOLDIER_SPAWN_MODE_DEFAULT
                && DarkSoldier_CanMoveAlongArc(entity)) {
                ai_state->action_flags = 1;
                EntityAi_StartAction(entity, DarkSoldier_MoveAlongArc, run_now);
                return;
            }
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 60;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            break;
        case DARK_SOLDIER_COMBAT_CHOICE_NEAR_SPAWN:
            EntityPoint_PickNear(entity->spawn_info->wander_radius, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            entity->combat_choice = COMBAT_CHOICE_CLOSE_IN;
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
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
            }
            entity->move_command |= MOVE_COMMAND_TURN_FAST;
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The move handler: as EntityAi_Move, but it only turns the entity (twice as fast for command bits
   0x40 and 0x400) and picks the animation; it does not move it. */
static void DarkSoldier_Move(Entity *entity) {
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

/* The melee attack (busy bit 1): the same as EntityAi_MeleeAttack. DarkSoldier_PlayAnim6Or7BySide and
   DarkSoldier_MoveAlongArc start it after calling EntityAi_PickAttack. */
static void DarkSoldier_MeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[ENTITY_FLAG_BITS(entity)->attack_index];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->attack_index | ENTITY_MOTION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
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

/* An action (busy bit 0x1000): plays animation 6 or 7 to its end, turning to the player while it
   blends in. With work byte unk7B at 1 the side is the one of the player's facing the entity is on,
   and DarkSoldier_MeleeAttack follows; with unk7B at 0 the side is drawn, weighted by that angle,
   and it thinks afterwards. */
static void DarkSoldier_PlayAnim6Or7BySide(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 side;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        side = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));
        if (ai_state->action_flags == 1) {
            if (side > 0.0f) {
                Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 1);
            } else {
                Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 1);
            }
        } else if (side < 0.0f) {
            if ((f32)(rand() % 120) > side * RAD_TO_DEG + 60.0f) {
                Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 1);
            } else {
                Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 1);
            }
        } else {
            if ((f32)(rand() % 120) > side * RAD_TO_DEG + 60.0f) {
                Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 1);
            } else {
                Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 1);
            }
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
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ai_state->action_flags == 0) {
                if (ENTITY_FLAG_BITS(entity)->own_think) {
                    entity->ai_state.think(entity, 1);
                } else {
                    EntityAi_StartAction(entity, ai_state->move, 1);
                }
            } else {
                EntityAi_PickAttack(entity);
                EntityAi_StartAction(entity, DarkSoldier_MeleeAttack, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x2000): the move DarkSoldier_CanMoveAlongArc tried out. Plays animation 8
   turning to the player, then from frame 10 moves the entity along the arc (move_along_arc:
   forwards at work float unk44 along unk30, in y at unk48, both from EntityParams.type_param_0 to type_param_2)
   through animations 17 and 9, until flag bit 44, 48, 49 or 50 is set. Afterwards, with work byte
   unk7B set, it calls EntityAi_PickAttack and starts DarkSoldier_MeleeAttack; else it thinks. */
static void DarkSoldier_MoveAlongArc(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModel *model;
    f32 unused[4]; // match: the frame has room for it, and no call here is a tail call

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case DARK_SOLDIER_ARC_START:
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        ENTITY_FLAG_BITS(entity)->draw_afterimage = FALSE;
        entity->action_step++;
    case DARK_SOLDIER_ARC_TURN:
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        model = entity->model;
        if (model->level_tables->blend_left == 0) {
            ai_state->part_angle_x = entity->params->type_param_0;
            ai_state->move_speed = entity->params->type_param_1;
            entity->action_step++;
            EntityMath_FacingVector((Vector4 *)ai_state->target_point, model->rotation[1]);
        }
        break;
    case DARK_SOLDIER_ARC_RUN:
        if (entity->model->level_tables->frame >= 10) {
            move_along_arc(entity, ai_state);
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            Motion_Start(entity, 0x11, 0, 1, 1);
            entity->action_step += 2;
        }
        break;
    case DARK_SOLDIER_ARC_LEAP:
        move_along_arc(entity, ai_state);
        if (ai_state->part_angle_x < 10.0f) {
            if (ABS(entity->heading_to_player - entity->model->rotation[1]) > HALF_PI) {
                ENTITY_FLAG_BITS(entity)->animates = TRUE;
                entity->action_step++;
            } else {
                Motion_Start(entity, 9, 0, 5, 1);
                entity->action_step += 2;
            }
        }
        break;
    case DARK_SOLDIER_ARC_FALL:
        if (ENTITY_FLAG_BITS(entity)->on_floor == 1 || ENTITY_FLAG_BITS(entity)->player_push == 1
            || ENTITY_FLAG_BITS(entity)->entity_push == 1 || ENTITY_FLAG_BITS(entity)->on_top == 1) {
            Motion_Start(entity, 9, 0, 1, 1);
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->action_step = DARK_SOLDIER_ARC_END;
            entity->move_delta_y = 0.0f;
            entity->move_delta_z = 0.0f;
            entity->move_delta_x = 0.0f;
            break;
        }
        move_along_arc(entity, ai_state);
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            Motion_Start(entity, 9, 0, 1, 1);
            entity->action_step = DARK_SOLDIER_ARC_LANDING;
        }
        break;
    case DARK_SOLDIER_ARC_LAND_NOW:
        if (ENTITY_FLAG_BITS(entity)->on_floor == 1 || ENTITY_FLAG_BITS(entity)->player_push == 1
            || ENTITY_FLAG_BITS(entity)->entity_push == 1 || ENTITY_FLAG_BITS(entity)->on_top == 1) {
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->action_step = DARK_SOLDIER_ARC_END;
            entity->move_delta_y = 0.0f;
            entity->move_delta_z = 0.0f;
            entity->move_delta_x = 0.0f;
            break;
        }
    case DARK_SOLDIER_ARC_BLEND_OUT:
        move_along_arc(entity, ai_state);
        if (entity->model->level_tables->blend_left == 0) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            entity->action_step++;
        }
        break;
    case DARK_SOLDIER_ARC_LANDING:
        if (ENTITY_FLAG_BITS(entity)->on_floor == 1 || ENTITY_FLAG_BITS(entity)->player_push == 1
            || ENTITY_FLAG_BITS(entity)->entity_push == 1 || ENTITY_FLAG_BITS(entity)->on_top == 1) {
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->action_step++;
            entity->move_delta_y = 0.0f;
            entity->move_delta_z = 0.0f;
            entity->move_delta_x = 0.0f;
            break;
        }
        move_along_arc(entity, ai_state);
        break;
    case DARK_SOLDIER_ARC_END:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ENTITY_FLAG_BITS(entity)->draw_afterimage = TRUE;
            if (ai_state->action_flags == 0) {
                if (ENTITY_FLAG_BITS(entity)->own_think) {
                    ai_state->think(entity, 1);
                } else {
                    EntityAi_StartAction(entity, ai_state->move, 1);
                }
            } else {
                EntityAi_PickAttack(entity);
                EntityAi_StartAction(entity, DarkSoldier_MeleeAttack, 1);
            }
        }
        break;
    }
}

/* The dodge action (busy bit 8): plays animation 6 or 7, by the side of the player's facing the
   entity is on, to its end, turning to the player while it blends in. It goes from step 1 straight
   to step 5: steps 2 to 4 (a movement in y by work float unk48, with animation 9) are never
   reached. */
static void DarkSoldier_Dodge(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 landed;

    entity->busy_flags |= ENTITY_BUSY_DODGE;
    switch (entity->action_step) {
    case DARK_SOLDIER_DODGE_START:
        if (EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position)) > 0.0f) {
            Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 1);
        } else {
            Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 1);
        }
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case DARK_SOLDIER_DODGE_TURN:
        if (entity->model->level_tables->blend_left == 0) {
            ai_state->part_angle_x = entity->params->type_param_0 * 0.8f;
            entity->action_step = DARK_SOLDIER_DODGE_END;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        break;
    case DARK_SOLDIER_DODGE_FALL:
        if (entity->model->level_tables->frame >= 4) {
            entity->move_delta_y = -ai_state->part_angle_x;
            ai_state->part_angle_x -= entity->params->type_param_2 * 0.1f;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            Motion_Start(entity, 9, 0, entity->params->motion_blend_frames >> 1, 1);
            entity->action_step++;
        }
        break;
    case DARK_SOLDIER_DODGE_FALLING:
        entity->move_delta_y = -ai_state->part_angle_x;
        ai_state->part_angle_x -= entity->params->type_param_2 * 0.1f;
        if (entity->model->level_tables->blend_left == 0) {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ANIMATES;
            entity->action_step++;
        }
        break;
    case DARK_SOLDIER_DODGE_LANDING:
        entity->move_delta_y = -ai_state->part_angle_x;
        ai_state->part_angle_x -= entity->params->type_param_2 * 0.1f;
        landed = ENTITY_FLAG_BITS(entity)->on_floor;
        if (landed == 1) {
            entity->move_delta_x = 0.0f;
            entity->move_delta_y = 0.0f;
            entity->move_delta_z = 0.0f;
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ANIMATES;
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
            entity->action_step++;
        }
        break;
    case DARK_SOLDIER_DODGE_END:
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

/* The on_hit handler: as EntityAi_OnHit, but it keeps the target and clears Entity.unk5C and flag
   bit 28. */
static void DarkSoldier_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    ENTITY_UNK5C(entity)->as_s32 = 0;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 1) {
        if ((s32)((s64)(entity->flags << 13) >> 32) & ENTITY_FLAG_ACTIVE_LIST) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
}

/* Helper of DarkSoldier_ThinkCombat: whether the way is free for DarkSoldier_MoveAlongArc. It
   follows the arc from the entity's feet (the arithmetic of move_along_arc) and asks EntityCollision_RayProps
   about the straight line to where it ends; TRUE when that returns 0. */
static s32 DarkSoldier_CanMoveAlongArc(Entity *entity) {
    f32 direction[4];
    f32 from[4];
    f32 target[4];
    f32 rise;
    f32 advance;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 distance;

    EntityMath_FacingVector((Vector4 *)direction, entity->model->rotation[1]);
    rise = entity->params->type_param_0;
    advance = entity->params->type_param_1;
    delta_x = delta_y = delta_z = 0.0f;
    while (rise > 0.0f) {
        delta_x += advance * direction[0];
        delta_z += advance * direction[2];
        delta_y -= rise;
        rise -= entity->params->type_param_2 * 0.1f;
        advance -= 0.5f;
        if (advance < 0.0f) {
            advance = 0.0f;
        }
    }
    direction[0] = delta_x;
    direction[1] = delta_y - 100.0f;
    direction[2] = delta_z;
    Vector_Copy(from, entity->model->position);
    from[1] -= entity->collision_height;
    Vector_Add(target, from, direction);
    distance = EntityMath_Distance(from, target);
    Vector_Normalize(direction, direction);
    return EntityCollision_RayProps(from, direction, NULL, distance) == 0;
}
