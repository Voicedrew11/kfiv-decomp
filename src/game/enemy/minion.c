/* Minion: the behaviour of entity type 60 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Minion. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "effect/shot.h"
#include "common.h"
#include "core/file.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include <libvu0.h>
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai_choice.h"
#include "entity/entity_math.h"
#include "entity/entity_ai.h"
#include "player/player.h"

/* Minion_Guard: Entity.action_step */
#define MINION_GUARD_STEP_HOLD              1       /* holds the guard pose while action_timer runs */
#define MINION_GUARD_STEP_RELEASE           2       /* ends the guard once action_timer runs out */

static void Minion_Think(Entity *entity, s32 run_now);
static void Minion_ThinkCombat(Entity *entity, s32 run_now);
static void Minion_Animate(Entity *entity);
static void Minion_Guard(Entity *entity);
static void Minion_MeleeAttack(Entity *entity);
static void Minion_RangedAttack(Entity *entity);
static void Minion_Move(Entity *entity);
static void Minion_OnHit(Entity *entity);
static void Minion_ReactionLight(Entity *entity);
static void Minion_ReactionHeavy(Entity *entity);
static void Minion_Death(Entity *entity);
static void Minion_Dodge(Entity *entity);
static s32 Minion_LaunchShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static void Minion_WorldToLocal(Entity *entity, f32 *result, f32 *point);

/* Set-up of a type 60 entity: clears its work area and installs the type's handlers. */
void Minion_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Minion_Think;
    ai_state->animate = Minion_Animate;
    ai_state->on_hit = Minion_OnHit;
    ai_state->move = Minion_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = Minion_Death;
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
    if (!(rand() & 1)) {
        Motion_SetPartSlots(entity->model, 4, 5, 1);
        ai_state->action_flags = 0;
    } else {
        Motion_SetPartSlots(entity->model, 6, 7, 1);
        ai_state->action_flags = 1;
    }
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.bit5 = 1;
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.bit6 = 1;
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_1.bit6 = 1;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with Minion_ThinkCombat for state 2. Once
   Entity.hit_points is below half of EntityParams.hit_points it sets work byte unk79 to 1 and keeps animation
   18 on the second motion slot. */
static void Minion_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (ai_state->turn_in_place == 0 && entity->hit_points < entity->params->hit_points >> 1) {
        ai_state->turn_in_place = 1;
    }
    if (ai_state->turn_in_place == 1 || ai_state->turn_in_place == 2) {
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)ai_state)->motion_slot_1, 1, 0x12, 0, entity->params->motion_blend_frames >> 1, 0);
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
                Minion_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat with the type's own guard, dodge and
   attacks; it closes in at the run. */
static void Minion_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, Minion_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, Minion_Dodge, 1);
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
                EntityAi_StartAction(entity, Minion_RangedAttack, run_now);
            } else {
                EntityAi_StartAction(entity, Minion_MeleeAttack, run_now);
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
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The animate handler: plays the two motion slots of the work area (unk80 and unk88) where
   EntityAi_Animate plays the model's one motion, then turns the head. */
static void Minion_Animate(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    EntityModelUnkF8 *part;
    f32 limits[4];
    s32 previous;
    f32 rate;

    if (!ENTITY_FLAG_BITS(entity)->animates) {
        return;
    }
    if (slots->motion_slot_0.frames_left != 0) {
        if (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) {
            rate = ai_state->pose_blend_weight;
        } else {
            rate = 1.0f / slots->motion_slot_0.frames_left;
        }
        // match: the caller passes a float the callee does not take
        ENTITY_FLAG_BITS(entity)->motion_ended = EntityAi_StepSlotMotion(entity, &slots->motion_slot_0, 0, rate);
    } else {
        previous = slots->motion_slot_0.frame_fixed;
        ENTITY_FLAG_BITS(entity)->motion_ended = Motion_StepSlot(entity, &((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        if (slots->motion_slot_0.index != 3) {
            EntityEvent_StartFrameEffects(entity, slots->motion_slot_0.index, previous, slots->motion_slot_0.frame_fixed);
            EntityEvent_StartFrameCues(entity, slots->motion_slot_0.index, previous, slots->motion_slot_0.frame_fixed);
        }
    }
    if (slots->motion_slot_1.frames_left != 0) {
        if (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) {
            rate = ai_state->pose_blend_weight;
        } else {
            rate = 1.0f / slots->motion_slot_1.frames_left;
        }
        // match: the caller passes a float the callee does not take
        ai_state->step_index = EntityAi_StepSlotMotion(entity, &slots->motion_slot_1, 1, rate);
    } else {
        if (ai_state->turn_in_place == 1 && slots->motion_slot_1.index == 0x12 && ai_state->step_index != 0) {
            part = &entity->model->parts[2];
            if (ai_state->action_flags) {
                part = &entity->model->parts[3];
            }
            ai_state->pitch_speed = part->part_angles[0];
            ai_state->heading_speed = part->part_angles[1];
            ai_state->keep_min_distance = part->part_angles[2];
            ai_state->turn_in_place = 2;
        }
        if (ai_state->turn_in_place != 2) {
            previous = slots->motion_slot_1.frame_fixed;
            ai_state->step_index = Motion_StepSlot(entity, &slots->motion_slot_1, 1, ENTITY_FLAG_BITS(entity)->motion_step_mode);
            if (slots->motion_slot_1.index == 3) {
                EntityEvent_StartFrameEffects(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
                EntityEvent_StartFrameCues(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
            }
        }
    }
    limits[1] = DEG_TO_RAD(entity->params->pitch_limit_deg);
    limits[0] = DEG_TO_RAD(-entity->params->pitch_limit_deg);
    limits[3] = DEG_TO_RAD(entity->params->yaw_limit_deg);
    limits[2] = DEG_TO_RAD(-entity->params->yaw_limit_deg);
    if (ENTITY_FLAG_BITS(entity)->look_at_target && entity->target != NULL) {
        EntityAi_LookAtPoint(entity, (int)ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, (int)entity->target, (int)limits, 12.0f);
    } else {
        EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, NULL, 8.0f);
    }
}

/* The guard action (busy bit 0x10): as EntityAi_Guard, on the motion slots. */
static void Minion_Guard(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_GUARD;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, 0xD, 0, 0xA, 1);
        if (ai_state->turn_in_place == 1) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, 0xD, 0, 0xA, 1);
        }
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case MINION_GUARD_STEP_HOLD:
        if (((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.frames_left == 0) {
            entity->action_timer = entity->params->guard_frames;
            ENTITY_FLAG_BITS(entity)->guarding = 1;
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            entity->action_step++;
        }
        break;
    case MINION_GUARD_STEP_RELEASE:
        if (entity->action_timer == 0) {
            ENTITY_FLAG_BITS(entity)->guarding = 0;
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, on the motion slots. */
static void Minion_MeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[ENTITY_FLAG_BITS(entity)->attack_index];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, ENTITY_FLAG_BITS(entity)->attack_index | ENTITY_MOTION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        if (ai_state->turn_in_place == 0) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, ENTITY_FLAG_BITS(entity)->attack_index | ENTITY_MOTION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        }
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
    case ACTION_STEP_BLEND_IN:
        if (slots->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (slots->motion_slot_0.frame <= unk240->hit_start_frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = (slots->motion_slot_0.frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, slots->motion_slot_0.frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, slots->motion_slot_0.frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, slots->motion_slot_0.frame);
        }
        if (slots->motion_slot_0.frame > 0 && unk240->hit_end_frame >= slots->motion_slot_0.frame) {
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, on the motion slots;
   Minion_LaunchShots launches the shots. */
static void Minion_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->attack_index);

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, ENTITY_FLAG_BITS(entity)->attack_index + ENTITY_MOTION_RANGED_ATTACK, 0, entity->params->motion_blend_frames >> 1, 1);
        if (ai_state->turn_in_place == 0) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, ENTITY_FLAG_BITS(entity)->attack_index + ENTITY_MOTION_RANGED_ATTACK, 0, entity->params->motion_blend_frames >> 1, 1);
        }
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (slots->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (slots->motion_slot_0.frame <= unk3D8->turn_frames) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        Minion_LaunchShots(entity, unk3D8, (slots->motion_slot_0.frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8, slots->motion_slot_0.frame, NULL, 0.0f, 0.0f);
        if (slots->motion_slot_0.frame > 0 && unk3D8->attack_end_frame >= slots->motion_slot_0.frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
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

/* The move handler: turns as the command says and picks animation 0, 3 or 4 on the motion slots. */
static void Minion_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    f32 turn;

    if (entity == player_state.stepped_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
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
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_0, 0, 4, 0, entity->params->motion_blend_frames >> 1, 0);
            if (ai_state->turn_in_place == 0) {
                EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_1, 1, 4, 0, entity->params->motion_blend_frames >> 1, 0);
            }
        } else {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_0, 0, 3, 0, entity->params->motion_blend_frames >> 1, 0);
            if (ai_state->turn_in_place == 0) {
                EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_1, 1, 3, 0, entity->params->motion_blend_frames >> 1, 0);
            }
        }
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_0, 0, 3, 0, entity->params->motion_blend_frames >> 1, 0);
        if (ai_state->turn_in_place == 0) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_1, 1, 3, 0, entity->params->motion_blend_frames >> 1, 0);
        }
    } else {
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_0, 0, 0, 0, entity->params->motion_blend_frames >> 1, 0);
        if (ai_state->turn_in_place == 0) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_1, 1, 0, 0, entity->params->motion_blend_frames >> 1, 0);
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit with the type's own two reactions. */
static void Minion_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->attacking = 0;
    ENTITY_FLAG_BITS(entity)->look_at_target = 0;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 1) {
        EntityAi_SetReaction(entity, Minion_ReactionLight, 0);
    } else if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, Minion_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the motion slots. */
static void Minion_ReactionLight(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_LIGHT;
    switch (entity->reaction_step) {
    case ACTION_STEP_START:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, 0xE, 0, 5, 1);
        if (ai_state->turn_in_place == 0) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, 0xE, 0, 5, 1);
        }
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = 0;
        ENTITY_FLAG_BITS(entity)->guarding = 0;
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->reaction_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.frames_left == 0) {
            entity->reaction_step++;
        }
        break;
    case REACTION_LIGHT_STEP_END:
        EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
        EntityAi_SetReaction(entity, NULL, 0);
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        if (ENTITY_FLAG_BITS(entity)->own_think) {
            entity->ai_state.think(entity, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the motion slots. */
static void Minion_ReactionHeavy(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, 0xF, 0, 5, 1);
        if (ai_state->turn_in_place == 0) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, 0xF, 0, 5, 1);
        }
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
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
        break;
    case ACTION_STEP_BLEND_IN:
        if (((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.frames_left == 0) {
            entity->reaction_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
            EntityAi_SetReaction(entity, NULL, 0);
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots. */
static void Minion_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, 0x10, 0, 5, 1);
        if (ai_state->turn_in_place == 0) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, 0x10, 0, 5, 1);
        }
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FLY) {
            entity->collision_radius = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->action_timer = 20;
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = 0;
        }
        break;
    }
}

/* The dodge action (busy bit 8): plays animation 6 or 7, by the side of the player's facing the
   entity is on, turning to the player. */
static void Minion_Dodge(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DODGE;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        if (EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position)) * RAD_TO_DEG > 0.0f) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, 6, 0, entity->params->motion_blend_frames >> 1, 1);
            if (ai_state->turn_in_place == 0) {
                EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, 6, 0, entity->params->motion_blend_frames >> 1, 1);
            }
        } else {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, 7, 0, entity->params->motion_blend_frames >> 1, 1);
            if (ai_state->turn_in_place == 0) {
                EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_1, 1, 7, 0, entity->params->motion_blend_frames >> 1, 1);
            }
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Helper of Minion_RangedAttack: launches the record's shots whose frame has come, as
   EntityShot_LaunchAimed does, from both launch points while work byte unk79 is 0 and afterwards only from
   the one work byte unk7B picks. */
static s32 Minion_LaunchShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;
    s32 i;
    s32 end;

    if (entity->ailment_4_frames != 0 && record->ignores_ailment_4 == 0) {
        return 0;
    }
    if (ai_state->turn_in_place == 0) {
        i = 0;
        end = 2;
    } else {
        i = ai_state->action_flags;
        end = i + 1;
    }
    for (; i < end; i++) {
        if (record->launches[i].frame != 0 && frame_from < record->launches[i].frame && record->launches[i].frame <= frame_to) {
            shot = Shot_Alloc();
            shot->player_magic = shot->player_power = 0;
            shot->is_magic_shot = 1;
            shot->shot_params = entity->params->shot_params;
            from = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), record->launches[i].bone);
            tip = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), record->launches[i].bone + 1);
            if (from != NULL && tip != NULL) {
                if (record->launches[i].bone != 0xFF) {
                    delta_x = tip[0] - from[0];
                    delta_y = tip[1] - from[1];
                    delta_z = tip[2] - from[2];
                    angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                    angles[2] = angles[3] = 0.0f;
                } else {
                    Vector_Copy(angles, entity->model->rotation);
                    angles[1] = EntityMath_WrapAngle(angles[1] + PI);
                }
                if (target != NULL) {
                    pitch = EntityMath_WrapAngle(-atan2(target[1] - from[1], EntityMath_DistanceXZ(from, target)));
                    yaw = EntityMath_AngleXZ(from, target);
                    angles[0] = EntityMath_Approach(angles[0], pitch, pitch_rate);
                    angles[1] = EntityMath_Approach(angles[1], yaw, yaw_rate);
                }
                Attack_LaunchMagicEffect(record->launches[i].magic_effect, shot, from, angles, entity);
            }
        }
    }
    return 0;
}

/* Turns a point from world space into the entity's own space: undoes the model's position, then its rotation.
   Nothing calls it. */
static void Minion_WorldToLocal(Entity *entity, f32 *result, f32 *point) {
    f32 matrix[16];
    f32 offset[4];

    memset(matrix, 0, 0x40);
    matrix[15] = matrix[10] = matrix[5] = matrix[0] = 1.0f;
    offset[0] = -entity->model->position[0];
    offset[1] = -entity->model->position[1];
    offset[2] = -entity->model->position[2];
    offset[3] = -entity->model->position[3];
    sceVu0TransMatrix(matrix, matrix, offset);
    sceVu0RotMatrixY(matrix, matrix, -entity->model->rotation[1]);
    sceVu0RotMatrixZ(matrix, matrix, -entity->model->rotation[2]);
    sceVu0RotMatrixX(matrix, matrix, -entity->model->rotation[0]);
    sceVu0ApplyMatrix(result, matrix, point);
}
