/* DarkPuppet: the behaviour of entity type 52 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Puppet. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_collision.h"
#include "entity/entity_events.h"
#include <libvu0.h>
#include "math/vu0.h"
#include "model/model.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_math.h"
#include "player/player.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

/* Entity.ai_state.move_choice (1 to 4) */
#define DARK_PUPPET_MOVE_CHOICE_BACK        1   /* backs away from the player */
#define DARK_PUPPET_MOVE_CHOICE_STEP_LEFT   2   /* steps left round the player */
#define DARK_PUPPET_MOVE_CHOICE_STEP_RIGHT  3   /* steps right round the player */
#define DARK_PUPPET_MOVE_CHOICE_TURN        4   /* turns to the player in place */

/* DarkPuppet_Death: Entity.action_step (steps 0, 1 and 3 are the shared ones) */
#define DARK_PUPPET_DEATH_STEP_DIE 2                /* waits for frame action_frames, then counts as dead */

static void DarkPuppet_OnHit(Entity *entity);
static void DarkPuppet_Think(Entity *entity, s32 run_now);
static void DarkPuppet_ThinkCombat(Entity *entity, s32 run_now);
static void DarkPuppet_Move(Entity *entity);
static void DarkPuppet_ApplyMoveCommand(Entity *entity);
static void DarkPuppet_MeleeAttack(Entity *entity);
static void DarkPuppet_RangedAttack0(Entity *entity);
static void DarkPuppet_RangedAttack(Entity *entity);
static void DarkPuppet_Death(Entity *entity);

/* Set-up of a type 52 entity: clears its work area and installs the type's handlers. */
void DarkPuppet_Init(Entity *entity) {
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
    ai_state->think = DarkPuppet_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = DarkPuppet_OnHit;
    ai_state->move = DarkPuppet_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = DarkPuppet_Death;
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

/* The on_hit handler: as EntityAi_OnHit, but it leaves the target and flag bit 10 as they are. */
static void DarkPuppet_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 1) {
        if ((s32)(entity->flags >> 19) & ENTITY_FLAG_ACTIVE_LIST) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
}

/* The think handler: as EntityAi_Think, with DarkPuppet_ThinkCombat for state 2. After that it
   keeps flag bit 28 set except during a hit reaction or the death (busy bits 2, 4, 0x40); while the
   bit is set it adds command bit 0x20 or 0x10 by the height EntityCollision_DistanceToPropFloor reads (work byte unk7A:
   set above EntityParams.type_param_1, cleared below type_param_2); and it calls DarkPuppet_ApplyMoveCommand while the entity
   is not busy or is in a hit reaction. */
static void DarkPuppet_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 height;

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
                DarkPuppet_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
    if (entity->busy_flags & (ENTITY_BUSY_DEATH | ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY)) {
        entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    } else {
        entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
    }
    if (ENTITY_FLAG_BITS(entity)->no_floor_lift) {
        height = EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position);
        if (height != dark_puppet_no_floor_height[0]) {
            if ((f32)entity->params->type_param_1 < height) {
                ai_state->step_index = 1;
            } else if (height < (f32)entity->params->type_param_2) {
                ai_state->step_index = 0;
            }
            if (ai_state->step_index != 0) {
                entity->move_command |= MOVE_COMMAND_UP;
            } else {
                entity->move_command |= MOVE_COMMAND_DOWN;
            }
        }
    }
    if (entity->busy_flags == 0 || (entity->busy_flags & (ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY))) {
        DarkPuppet_ApplyMoveCommand(entity);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat with the type's own attacks
   (DarkPuppet_MeleeAttack; DarkPuppet_RangedAttack0 for ranged record 0,
   DarkPuppet_RangedAttack for the others). Closing in lasts EntityParams.type_param_0 frames and has
   four ways, kept in work byte unk78: 0 forward (the player farther than unk438), 1 backwards
   (nearer than unk436), 2 and 3 sideways (command bits 4 and 8) while keeping between the two
   distances; EntityAi_IsPushedFromFront makes it change the way. Waiting (4) it turns to the player. */
static void DarkPuppet_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
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
                if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                    EntityAi_StartAction(entity, DarkPuppet_RangedAttack0, run_now);
                } else {
                    EntityAi_StartAction(entity, DarkPuppet_RangedAttack, run_now);
                }
            } else {
                EntityAi_StartAction(entity, DarkPuppet_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            if ((f32)entity->params->far_distance < entity->distance_to_player) {
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            } else if (entity->distance_to_player < (f32)entity->params->near_distance) {
                ai_state->move_choice = DARK_PUPPET_MOVE_CHOICE_BACK;
            } else if (EntityMath_AngleDifference(player_state.rotation.y,
                                     EntityMath_AngleXZ(&player_state.position.x, entity->model->position)) > 0.0f) {
                ai_state->move_choice = DARK_PUPPET_MOVE_CHOICE_STEP_LEFT;
            } else {
                ai_state->move_choice = DARK_PUPPET_MOVE_CHOICE_STEP_RIGHT;
            }
            entity->action_timer = entity->params->type_param_0;
            break;
        case COMBAT_CHOICE_WAIT:
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = DARK_PUPPET_MOVE_CHOICE_TURN;
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
            switch (ai_state->move_choice) {
            case MOVE_CHOICE_CLOSE_IN:
                turn = EntityAi_TurnFromPush(entity, direction);
                if (turn > 0) {
                    entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command |= EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case DARK_PUPPET_MOVE_CHOICE_BACK:
                if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] + PI))) {
                    ai_state->move_choice = rand() % 3 + 2;
                } else {
                    entity->move_command |= EntityAi_TurnCommand(entity);
                    entity->move_command |= MOVE_COMMAND_BACK;
                }
                break;
            case DARK_PUPPET_MOVE_CHOICE_STEP_LEFT:
                if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] - HALF_PI))) {
                    ai_state->move_choice = DARK_PUPPET_MOVE_CHOICE_STEP_RIGHT;
                } else {
                    entity->move_command |= EntityAi_TurnCommand(entity);
                    entity->move_command |= MOVE_COMMAND_STEP_LEFT;
                    if ((f32)entity->params->far_distance < entity->distance_to_player) {
                        entity->move_command |= MOVE_COMMAND_FORWARD;
                    } else if (entity->distance_to_player < (f32)entity->params->near_distance) {
                        entity->move_command |= MOVE_COMMAND_BACK;
                    }
                }
                break;
            case DARK_PUPPET_MOVE_CHOICE_STEP_RIGHT:
                if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] + HALF_PI))) {
                    ai_state->move_choice = DARK_PUPPET_MOVE_CHOICE_STEP_LEFT;
                } else {
                    entity->move_command |= EntityAi_TurnCommand(entity);
                    entity->move_command |= MOVE_COMMAND_STEP_RIGHT;
                    if ((f32)entity->params->far_distance < entity->distance_to_player) {
                        entity->move_command |= MOVE_COMMAND_FORWARD;
                    } else if (entity->distance_to_player < (f32)entity->params->near_distance) {
                        entity->move_command |= MOVE_COMMAND_BACK;
                    }
                }
                break;
            case DARK_PUPPET_MOVE_CHOICE_TURN:
                entity->move_command |= EntityAi_TurnCommand(entity);
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The move handler: only picks the animation, 6 or 7 for command bits 4 and 8, 3 forward, 5
   backwards, else 0. DarkPuppet_ApplyMoveCommand does the turning and the moving. */
static void DarkPuppet_Move(Entity *entity) {
    if (entity == (Entity *)entity_under_player[0]) {
        if (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON) {
            entity->move_command = 0;
        }
    }
    if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* Carries out the movement command in Entity.move_command; DarkPuppet_Think, DarkPuppet_MeleeAttack
   and the ranged attacks call it. It turns the entity and speeds up or slows down along its three
   axes by EntityParams.speed_step_per_frame a frame up to unk54 (work floats unk44, unk48 and unk4C), twice as
   fast for command bit 0x40. No command when the entity is player_state.unk204 and EntityParams.flags
   has 0x800. */
static void DarkPuppet_ApplyMoveCommand(Entity *entity) {
    EntityAiStateKind16BF98 *ai_state = (EntityAiStateKind16BF98 *)&entity->ai_state;
    f32 turn;
    f32 step;
    f32 limit;

    if (entity == player_state.stepped_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
    turn = 0.0f;
    if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
        if (ai_state->turn_rate < -entity->params->turn_limit) {
            ai_state->turn_rate = -entity->params->turn_limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
        if (entity->params->turn_limit < ai_state->turn_rate) {
            ai_state->turn_rate = entity->params->turn_limit;
        }
    } else if (ai_state->turn_rate > entity->params->turn_step_per_frame) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
    } else if (ai_state->turn_rate < -entity->params->turn_step_per_frame) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
    } else {
        ai_state->turn_rate = 0.0f;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->speed_step_per_frame * 0.5f;
    limit = entity->params->max_speed * 0.2f;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        ai_state->forward_speed -= entity->params->speed_step_per_frame;
        if (ai_state->forward_speed < -entity->params->max_speed) {
            ai_state->forward_speed = -entity->params->max_speed;
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        ai_state->forward_speed += entity->params->speed_step_per_frame;
        if (entity->params->max_speed < ai_state->forward_speed) {
            ai_state->forward_speed = entity->params->max_speed;
        }
    } else if (entity->params->speed_step_per_frame < ai_state->forward_speed) {
        ai_state->forward_speed -= entity->params->speed_step_per_frame;
    } else if (ai_state->forward_speed < -entity->params->speed_step_per_frame) {
        ai_state->forward_speed += entity->params->speed_step_per_frame;
    } else {
        ai_state->forward_speed = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        ai_state->move_speed += entity->params->speed_step_per_frame;
        if (entity->params->max_speed < ai_state->move_speed) {
            ai_state->move_speed = entity->params->max_speed;
        }
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        ai_state->move_speed -= entity->params->speed_step_per_frame;
        if (ai_state->move_speed < -entity->params->max_speed) {
            ai_state->move_speed = -entity->params->max_speed;
        }
    } else if (entity->params->speed_step_per_frame < ai_state->move_speed) {
        ai_state->move_speed -= entity->params->speed_step_per_frame;
    } else if (ai_state->move_speed < -entity->params->speed_step_per_frame) {
        ai_state->move_speed += entity->params->speed_step_per_frame;
    } else {
        ai_state->move_speed = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_DOWN) {
        ai_state->vertical_speed -= step;
        if (ai_state->vertical_speed < -limit) {
            ai_state->vertical_speed = -limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        ai_state->vertical_speed += step;
        if (limit < ai_state->vertical_speed) {
            ai_state->vertical_speed = limit;
        }
    } else if (step < ai_state->vertical_speed) {
        ai_state->vertical_speed -= step;
    } else if (ai_state->vertical_speed < -step) {
        ai_state->vertical_speed += step;
    } else {
        ai_state->vertical_speed = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = ai_state->move_speed;
    ENTITY_SCRATCH_VECTOR[1] = ai_state->vertical_speed;
    ENTITY_SCRATCH_VECTOR[2] = ai_state->forward_speed;
    sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, identity_matrix, entity->model->rotation[2]);
    sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[0]);
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[1]);
    Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    if (entity->move_command & MOVE_COMMAND_RUN) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
    entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
    entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, but it turns by adding the command of
   EntityAi_TurnCommand, and calls DarkPuppet_ApplyMoveCommand every frame. */
static void DarkPuppet_MeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[kind];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, kind | ENTITY_MOTION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_1_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_2_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, entity->model->level_tables->frame);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, ai_state->move, 1);
            return;
        }
        break;
    }
    DarkPuppet_ApplyMoveCommand(entity);
}

/* The ranged attack with record 0 (busy bit 1; DarkPuppet_ThinkCombat starts it for index 0
   only): animation 23; on frame 30 it launches the record's shot unk10 from the point Model_GetSkinnedBonePoint
   gives for the record's unk11, along the model's angles (EntityAi_LaunchShot). Turns and moves as
   DarkPuppet_MeleeAttack does. */
static void DarkPuppet_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8Array *unk3D8 = entity->params->ranged_attacks;
    f32 target[4]; // match: never used, but retail keeps 16 bytes of stack for it
    f32 *point;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->record.turn_frames != 0) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->record.turn_frames >= entity->model->level_tables->frame) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        if (ENTITY_FLAG_BITS(entity)->attack_index == 0 && entity->model->level_tables->frame == 0x1E &&
            (entity->ailment_4_frames == 0 || unk3D8->ignores_ailment_4 != 0)) {
            point = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), unk3D8->bone_index);
            if (point != NULL) {
                EntityAi_LaunchShot(entity, unk3D8->magic_effect, point, entity->model->rotation);
            }
        }
        if (entity->model->level_tables->frame > 0 && unk3D8->record.attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->record.attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
            return;
        }
        break;
    }
    DarkPuppet_ApplyMoveCommand(entity);
}

/* The ranged attack of the other records (busy bit 1; started for index 1 or 2): as
   EntityAi_RangedAttack; turns and moves as DarkPuppet_MeleeAttack does. */
static void DarkPuppet_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, kind + ENTITY_MOTION_RANGED_ATTACK, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8,
                      entity->model->level_tables->frame);
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
                return;
            }
            EntityAi_StartAction(entity, ai_state->move, 1);
            return;
        }
        break;
    }
    DarkPuppet_ApplyMoveCommand(entity);
}

/* The death action (busy bit 0x40): animation 16; once it is past frame EntityParams.action_frames it
   calls EntityEvent_OnDeath and fades the model out over the rest of the motion (the frames left, kept in
   work word unk68), then removes the entity when the motion has ended. */
static void DarkPuppet_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags = (entity->flags & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_DYING_1;
        entity->flags |= ENTITY_FLAG_NO_PLAYER_COLLISION;
        entity->flags |= ENTITY_FLAG_NO_ENTITY_COLLISION;
        entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case DARK_PUPPET_DEATH_STEP_DIE:
        if ((s16)entity->params->action_frames < entity->model->level_tables->frame) {
            entity->action_timer = ai_state->pause_timer = EntityAi_AnimationFrameCount(entity, entity->animation_index) - entity->model->level_tables->frame;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / (f32)ai_state->pause_timer;
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = 0;
        }
        break;
    }
}
