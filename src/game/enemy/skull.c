/* Skull: the behaviour of entity type 65 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Skull. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/shot.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include <libvu0.h>
#include "math/vu0.h"
#include "model/model.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"
#include "player/player.h"

/* EntityAiState.move_choice 1 (0 is MOVE_CHOICE_CLOSE_IN): runs at the player */
#define SKULL_MOVE_CHOICE_RUN_IN 1


static void Skull_Think(Entity *entity, s32 run_now);
static void Skull_ThinkCombat(Entity *entity, s32 run_now);
static void Skull_Move(Entity *entity);
static void Skull_Death(Entity *entity);
static void Skull_ApplyMoveCommand(Entity *entity);
static void Skull_SwingPart3(Entity *entity);
static void Skull_Animate(Entity *entity);
static void Skull_RangedAttack0(Entity *entity);

/* Set-up of a type 65 entity: clears its work area and installs the type's handlers. */
void Skull_Init(Entity *entity) {
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
    ai_state->think = Skull_Think;
    ai_state->animate = Skull_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = Skull_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = Skull_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
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
    ai_state->sub_timer = rand() % 30;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with Skull_ThinkCombat for state 2. It keeps flag
   bits 22 and 28 set except in the death action. While bit 28 is set and no attack runs it adds
   command bit 0x20 or 0x10 (work byte unk7A), chosen anew every 20 to 34 frames (work word unk6A)
   so as to keep its y between 300 and 100 below the player's; then Skull_ApplyMoveCommand moves
   the entity. When work word unk68 has run below 0 it draws new values for
   Skull_SwingPart3 (work bytes unk7B and unk7C). */
static void Skull_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags & ENTITY_BUSY_DEATH) {
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
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
                Skull_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
    if (ai_state->sub_timer > 0) {
        ai_state->sub_timer--;
    }
    if (ENTITY_FLAG_BITS(entity)->no_floor_lift && !(entity->busy_flags & ENTITY_BUSY_ATTACK)) {
        if (ai_state->sub_timer <= 0) {
            if (entity->model->position[1] < player_state.position.y - 300.0f) {
                ai_state->step_index = 1;
            } else if (player_state.position.y - 100.0f < entity->model->position[1]) {
                ai_state->step_index = 0;
            } else {
                ai_state->step_index = ai_state->step_index == 0;
            }
            ai_state->sub_timer = rand() % 15 + 20;
        }
        if (ai_state->step_index != 0) {
            entity->move_command |= MOVE_COMMAND_UP;
        } else {
            entity->move_command |= MOVE_COMMAND_DOWN;
        }
    }
    Skull_ApplyMoveCommand(entity);
    if (ai_state->pause_timer < 0) {
        ai_state->action_counter = (rand() % 10 + 10) * 2;
        if (rand() % 10 == 1) {
            ai_state->action_flags = 0;
        } else {
            ai_state->action_flags = rand() % entity->params->far_distance;
        }
        ai_state->pause_timer = ai_state->action_counter * 2;
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat with Skull_RangedAttack0 as the
   ranged attack. It attacks only while its y is between 300 and 50 below the player's; otherwise it
   sets the choice of command bit 0x20 or 0x10 (work byte unk7A) for 15 to 29 frames. One time in
   three it closes in with command bit 0x40. */
static void Skull_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    } else {
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    if (entity->action_timer == 0 || mode == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = mode;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (entity->model->position[1] < player_state.position.y - 50.0f && player_state.position.y - 300.0f < entity->model->position[1]) {
                if (EntityAi_PickAttack(entity)) {
                    EntityAi_StartAction(entity, Skull_RangedAttack0, run_now);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
                }
                return;
            }
            if (entity->model->position[1] < player_state.position.y) {
                ai_state->step_index = 1;
            } else {
                ai_state->step_index = 0;
            }
            ai_state->sub_timer = rand() % 15 + 15;
            break;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            if (rand() % 3 == 0) {
                ai_state->move_choice = SKULL_MOVE_CHOICE_RUN_IN;
            } else {
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
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
            side = EntityAi_TurnFromPush(entity, direction);
            switch (ai_state->move_choice) {
            case MOVE_CHOICE_CLOSE_IN:
                if (side > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (side < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case SKULL_MOVE_CHOICE_RUN_IN:
                if (side > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
                } else if (side < 0) {
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

/* The move handler: only picks the animation, 3 with command bit 1, else 0.
   Skull_ApplyMoveCommand does the turning and the moving. */
static void Skull_Move(Entity *entity) {
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; it sets Entity.collision_radius to 80. */
static void Skull_Death(Entity *entity) {
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
        entity->collision_radius = 80.0f;
        entity->flags &= ~ENTITY_FLAG_400000;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= ENTITY_FLAG_DYING_MASK;
            entity->flags &= ~ENTITY_FLAG_LIVE;
        }
        break;
    }
}

/* Helper of Skull_Think: turns and moves the entity as the command says, the speeds eased in
   work floats move_speed (sideways), vertical_speed and forward_speed. While the entity is busy with
   anything but a hit reaction only command bits 0x10 and 0x20 count, and the turn and the other
   speeds ease out. */
static void Skull_ApplyMoveCommand(Entity *entity) {
    EntityAiStateKind16BF98 *ai_state = (EntityAiStateKind16BF98 *)&entity->ai_state;
    f32 turn = 0.0f;
    f32 step;
    f32 limit;

    if (entity->busy_flags == 0 || (entity->busy_flags & (ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY))) {
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
        } else if (entity->params->turn_step_per_frame < ai_state->turn_rate) {
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
        } else if (ai_state->forward_speed > entity->params->speed_step_per_frame) {
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
        } else if (ai_state->move_speed > entity->params->speed_step_per_frame) {
            ai_state->move_speed -= entity->params->speed_step_per_frame;
        } else if (ai_state->move_speed < -entity->params->speed_step_per_frame) {
            ai_state->move_speed += entity->params->speed_step_per_frame;
        } else {
            ai_state->move_speed = 0.0f;
        }
    } else {
        if (entity->params->turn_step_per_frame < ai_state->turn_rate) {
            ai_state->turn_rate -= entity->params->turn_step_per_frame;
        } else if (ai_state->turn_rate < -entity->params->turn_step_per_frame) {
            ai_state->turn_rate += entity->params->turn_step_per_frame;
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (ai_state->forward_speed > entity->params->speed_step_per_frame) {
            ai_state->forward_speed -= entity->params->speed_step_per_frame;
        } else if (ai_state->forward_speed < -entity->params->speed_step_per_frame) {
            ai_state->forward_speed += entity->params->speed_step_per_frame;
        } else {
            ai_state->forward_speed = 0.0f;
        }
        if (entity->params->speed_step_per_frame < ai_state->move_speed) {
            ai_state->move_speed -= entity->params->speed_step_per_frame;
        } else if (ai_state->move_speed < -entity->params->speed_step_per_frame) {
            ai_state->move_speed += entity->params->speed_step_per_frame;
        } else {
            ai_state->move_speed = 0.0f;
        }
    }
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->speed_step_per_frame * 0.8f;
    limit = entity->params->max_speed * 0.5f;
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

/* Helper of Skull_Animate: swings angle unk50[0] of model part 3 up to work byte unk7B
   degrees and back to 0 over twice work byte unk7C frames, counted down in work word unk68; the
   angle is kept in work float unk50. */
static void Skull_SwingPart3(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *part = &((EntityModelPart80 *)entity->model->parts)[3];
    f32 angle;

    if (part != NULL) {
        if (ai_state->pause_timer >= ai_state->action_counter) {
            ai_state->part_angle_z += (f32)ai_state->action_flags / (f32)ai_state->action_counter * DEG_TO_RAD(1);
            if (DEG_TO_RAD(ai_state->action_flags) < ai_state->part_angle_z) {
                ai_state->part_angle_z = DEG_TO_RAD(ai_state->action_flags);
            }
        } else {
            ai_state->part_angle_z -= (f32)ai_state->action_flags / (f32)ai_state->action_counter * DEG_TO_RAD(1);
            if (ai_state->part_angle_z < 0.0f) {
                ai_state->part_angle_z = 0.0f;
            }
        }
        angle = ai_state->part_angle_z;
        part->part_angles[0] = angle;
        part->part_angles[0] = EntityMath_WrapAngle(angle);
        ai_state->pause_timer--;
    }
}

/* The animate handler: as EntityAi_Animate without the head turn. Skull_SwingPart3 swings
   model part 3 while the entity is not busy. Every tenth frame it launches shot 0xD0 from the point
   Model_GetSkinnedBonePoint gives for 0xFF (Attack_LaunchMagicEffect); on the other frames it starts effect 0x39D at model
   point 0xFF. */
static void Skull_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;
    EntityUnk1B1390 *shot;
    f32 unused[4]; /* retail reserves the 0x10 bytes */

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FIXED) {
            entity->flags = flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        }
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        if (entity->busy_flags == 0 && entity->model->level_tables->blend_left == 0) {
            Skull_SwingPart3(entity);
        } else {
            ai_state->part_angle_z = 0.0f;
        }
        if (frame_counter % 10U == 0) {
            shot = Shot_Alloc();
            shot->is_magic_shot = 1;
            shot->player_power = 0;
            shot->player_magic = 0;
            shot->shot_params = entity->params->shot_params;
            Attack_LaunchMagicEffect(0xD0, shot, Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), 0xFF), entity->model->rotation, entity);
        } else {
            EntityEvent_StartEffectAtPoint(entity, 0x39D, 0xFF, entity->model->rotation);
        }
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame EntityParams.type_param_0 to
   type_param_1 launches the record's shots every other frame. */
static void Skull_RangedAttack0(Entity *entity) {
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
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (entity->model->level_tables->frame >= entity->params->type_param_0 && entity->model->level_tables->frame <= entity->params->type_param_1 &&
            (frame_counter & 1)) {
            EntityAi_LaunchShots(entity, unk3D8, entity->params->type_param_0, entity->params->type_param_1);
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
