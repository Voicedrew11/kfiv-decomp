/* AncientKing: the behaviour of entity type 61 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: the Ancient King, chr.prm record
   57, whose one spawn is in 006.map with six High Skeletons as spawn ids 10-15, the entities this code
   raises again. The game names no monsters; the name is from the bestiary of Prima's official strategy
   guide (2004), whose description agrees with this code and with the record: it summons High Skeletons,
   and it resists every kind of magic but light (50 each, light 0) while weapons hit it normally. */
#include "entity/entity.h"
#include "entity/entity_world.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_events.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai.h"
#include "entity/entity_math.h"
#include "player/player.h"

/* Entity.action_step values past the shared ones */
#define ANCIENT_KING_DEATH_STEP_SECOND_BLEND_IN 3   /* waits for the second death motion to blend in */
#define ANCIENT_KING_DEATH_STEP_PLAY_SHOT       4   /* plays that motion, launches the shot, then starts the fade */
#define ANCIENT_KING_DEATH_STEP_FADE_OUT        5   /* fades the model out over action_timer, then frees it */
#define ANCIENT_KING_RESPAWN_STEP_END           3   /* waits for the motion to end, then ends the action */


static void AncientKing_Animate(Entity *entity);
static void AncientKing_Think(Entity *entity, s32 run_now);
static void AncientKing_ThinkCombat(Entity *entity, s32 run_now);
static void AncientKing_Move(Entity *entity);
static void AncientKing_Death(Entity *entity);
static void AncientKing_RangedAttack0(Entity *entity);
static void AncientKing_RespawnOthers(Entity *entity);
static void AncientKing_SwitchHandlers(Entity *entity);
static void AncientKing_ThinkAfterSwitch(Entity *entity, s32 run_now);
static void AncientKing_ThinkCombatAfterSwitch(Entity *entity, s32 run_now);
static void AncientKing_DeathLaunchShot(Entity *entity);
static void AncientKing_LaunchShotsNearPlayer(Entity *entity);
static void AncientKing_RangedAttack1(Entity *entity);
static void AncientKing_RespawnOthersAfterSwitch(Entity *entity);
static s32 AncientKing_ShouldRespawnOthers(Entity *entity);
static void AncientKing_RespawnTwoOthers(Entity *entity);

/* Set-up of a type 61 entity: clears its work area and installs the type's handlers. */
void AncientKing_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *choice_weights = entity->choice_weights;
    u8 *attack_weights = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;
    Entity *other;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = ENTITY_WORK_BYTES(entity);

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = AncientKing_Think;
    ai_state->animate = AncientKing_Animate;
    ai_state->move = AncientKing_Move;
    ai_state->death = AncientKing_Death;
    ai_state->on_hit = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    ENTITY_FLAG_BITS(entity)->no_map_object_collision = TRUE;
    EntityAi_StartAction(entity, ai_state->move, 0);
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_SetPartSlots(entity->model, 5, 0x16, 1);
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.bit5 = 1;
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.bit6 = 1;
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_1.bit6 = 1;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
    for (i = 0; i < 6; i++) {
        other = Entity_GetBySpawnId(i + 10);
        if (other != NULL) {
            other->flags &= ~ENTITY_FLAG_LIVE;
            other->flags &= ~ENTITY_FLAG_RESPAWNABLE;
            other->flags &= ~ENTITY_FLAG_RESPAWN_ROLL_DONE;
        }
    }
}

/* The animate handler until AncientKing_SwitchHandlers puts EntityAi_Animate in its place: plays the
   two motion slots of the work area (motion_slot_0 and motion_slot_1), flag bit 34 coming from the second, then
   turns the head. */
static void AncientKing_Animate(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    f32 limits[4];
    s32 previous;

    if (!ENTITY_FLAG_BITS(entity)->animates) {
        return;
    }
    if (slots->motion_slot_0.frames_left != 0) {
        // match: the caller passes a float the callee does not take
        ENTITY_FLAG_BITS(entity)->motion_ended = EntityAi_StepSlotMotion(entity, &((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, 1.0f / slots->motion_slot_0.frames_left);
    } else {
        previous = slots->motion_slot_0.frame_fixed;
        Motion_StepSlot(entity, &((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        if (slots->motion_slot_0.index == 1) {
            EntityEvent_StartFrameEffects(entity, slots->motion_slot_0.index, previous, slots->motion_slot_0.frame_fixed);
            EntityEvent_StartFrameCues(entity, slots->motion_slot_0.index, previous, slots->motion_slot_0.frame_fixed);
        }
    }
    if (slots->motion_slot_1.frames_left != 0) {
        // match: the caller passes a float the callee does not take
        EntityAi_StepSlotMotion(entity, &slots->motion_slot_1, 1, 1.0f / slots->motion_slot_1.frames_left);
    } else {
        previous = slots->motion_slot_1.frame_fixed;
        ENTITY_FLAG_BITS(entity)->motion_ended = Motion_StepSlot(entity, &slots->motion_slot_1, 1, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        if (slots->motion_slot_1.index != 1) {
            EntityEvent_StartFrameEffects(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
            EntityEvent_StartFrameCues(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
        }
    }
    limits[1] = DEG_TO_RAD(entity->params->pitch_limit_deg);
    limits[0] = DEG_TO_RAD(-entity->params->pitch_limit_deg);
    limits[3] = DEG_TO_RAD(entity->params->yaw_limit_deg);
    limits[2] = DEG_TO_RAD(-entity->params->yaw_limit_deg);
    if (ENTITY_FLAG_BITS(entity)->look_at_target) {
        EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, limits, 12.0f);
    } else {
        EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, -1, NULL, 8.0f);
    }
}

/* The think handler until AncientKing_SwitchHandlers installs AncientKing_ThinkAfterSwitch. While the entity
   is not busy: once Entity.hit_points is below three quarters of EntityParams.hit_points it starts
   AncientKing_SwitchHandlers; else in state 2 it calls AncientKing_ThinkCombat, and in the other
   states starts the move action. Counts down ai_state.pause_timer. */
static void AncientKing_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->busy_flags == 0) {
        EntityAi_StepState(entity);
        if (entity->hit_points < (s32)(entity->params->hit_points * 0.75f)) {
            EntityAi_StartAction(entity, AncientKing_SwitchHandlers, run_now);
            return;
        }
        switch (entity->state) {
        case ENTITY_STATE_NOTICED:
            AncientKing_ThinkCombat(entity, run_now);
            break;
        case ENTITY_STATE_UNAWARE:
        case ENTITY_STATE_SEARCHING:
        default:
            EntityAi_StartAction(entity, ai_state->move, run_now);
            break;
        }
    }
    if (ai_state->pause_timer > 0) {
        ai_state->pause_timer--;
    }
}

/* The think function of state 2: starts AncientKing_RespawnOthers when AncientKing_ShouldRespawnOthers says so; else
   AncientKing_RangedAttack0 when the pause Entity.attack_delay_frames is over and EntityAi_CanUseRanged finds the player
   in reach of ranged record 0; else the move action. */
static void AncientKing_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (AncientKing_ShouldRespawnOthers(entity)) {
        EntityAi_StartAction(entity, AncientKing_RespawnOthers, run_now);
    } else if (entity->attack_delay_frames == 0 && EntityAi_CanUseRanged(entity, 0)) {
        EntityAi_StartAttack(entity, AncientKing_RangedAttack0, run_now, 1, 0);
    } else {
        EntityAi_StartAction(entity, ai_state->move, run_now);
    }
}

/* The move handler until AncientKing_SwitchHandlers installs EntityAi_Move: keeps animation 1 on both
   motion slots and does not turn. */
static void AncientKing_Move(Entity *entity) {
    entity->ai_state.turn_rate = 0.0f;
    EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)entity->ai_state.motion_slot_0, 0, 1, 0, entity->params->motion_blend_frames, 0);
    EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)entity->ai_state.motion_slot_1, 1, 1, 0, entity->params->motion_blend_frames, 0);
}

/* The death action (busy bit 0x40) until AncientKing_SwitchHandlers installs AncientKing_DeathLaunchShot:
   on the motion slots, plays animation 2 to its end and then animation 16; as that passes frame 43
   it launches shot launches[0].magic_effect of ranged record 0 with the player's position as its starting
   point (EntityAi_LaunchShot); then the fade and the removal as in EntityAi_Death. */
static void AncientKing_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    f32 target[4];

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_0, 0, 2, 0, entity->params->motion_blend_frames, 0);
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_1, 1, 2, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_0, 0, 0x10, 5, entity->params->motion_blend_frames, 1);
            EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&slots->motion_slot_1, 1, 0x10, 5, entity->params->motion_blend_frames, 1);
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
            ENTITY_FLAG_BITS(entity)->dying = 1;
            ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->action_step++;
        }
        break;
    case ACTION_STEP_BLEND_IN:
    case ANCIENT_KING_DEATH_STEP_SECOND_BLEND_IN:
        if (slots->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        break;
    case ANCIENT_KING_DEATH_STEP_PLAY_SHOT:
        if ((slots->motion_slot_1.frame_fixed - entity->params->motion_speeds[slots->motion_slot_1.index]) >> 8 < 0x2B && slots->motion_slot_1.frame >= 0x2B) {
            Vector_Copy(target, &player_state.position);
            EntityAi_LaunchShot(entity, ((EntityParamsUnk3D8Full *)&entity->params->ranged_attacks[0])->launches[0].magic_effect, target,
                          entity->model->rotation);
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case ANCIENT_KING_DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1) of the first set of handlers: animation 23 on the
   second motion slot; on each of the five frames in ancient_king_near_shot_frames it launches the record's shot
   launches[0].magic_effect, its starting point up to 600 from the player's position in x and in z
   (EntityAi_LaunchShot). */
static void AncientKing_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->ranged_attacks[0];
    f32 target[4];
    EntityFrameTable5 launch_frames = ancient_king_near_shot_frames;
    s32 elapsed;
    s32 i;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)entity->ai_state.motion_slot_1, 1, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (((EntityAiMotionSlot *)ai_state->motion_slot_1)->frames_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        elapsed = (((EntityAiMotionSlot *)ai_state->motion_slot_1)->frame_fixed -
                   entity->params->motion_speeds[((EntityAiMotionSlotBits *)ai_state->motion_slot_1)->index]) >> 8;
        for (i = 0; i < 5; i++) {
            if (entity->ailment_4_frames == 0 || unk3D8->ignores_ailment_4 != 0) {
                if (elapsed < launch_frames.frames[i] && ((EntityAiMotionSlot *)ai_state->motion_slot_1)->frame >= launch_frames.frames[i]) {
                    Vector_Copy(target, &player_state.position);
                    target[0] += rand() % 1201 - 600;
                    target[2] += rand() % 1201 - 600;
                    EntityAi_LaunchShot(entity, unk3D8->launches[0].magic_effect, target, entity->model->rotation);
                }
            }
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

/* An action (busy bit 0x1000) that AncientKing_ThinkCombat starts when
   AncientKing_ShouldRespawnOthers says so: animation 17 on the second motion slot; on frame
   EntityParams.far_distance it starts effect near_distance at model point type_param_2 (EntityEvent_StartEffectAtPoint), past frame
   type_param_1 effect type_param_0 there, and calls AncientKing_RespawnTwoOthers; then it plays to the end. */
static void AncientKing_RespawnOthers(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)entity->ai_state.motion_slot_1, 1, 0x11, 0, entity->params->motion_blend_frames, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (((EntityAiMotionSlot *)ai_state->motion_slot_1)->frames_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (((EntityAiMotionSlot *)ai_state->motion_slot_1)->frame == entity->params->far_distance) {
            EntityEvent_StartEffectAtPoint(entity, entity->params->near_distance, entity->params->type_param_2, entity->model->rotation);
        }
        if (entity->params->type_param_1 < ((EntityAiMotionSlot *)ai_state->motion_slot_1)->frame) {
            EntityEvent_StartEffectAtPoint(entity, entity->params->type_param_0, entity->params->type_param_2, entity->model->rotation);
            AncientKing_RespawnTwoOthers(entity);
            entity->action_step++;
        }
        break;
    case ANCIENT_KING_RESPAWN_STEP_END:
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

/* An action (busy bit 0x2000) that AncientKing_Think starts: plays animation 2 on both motion
   slots to its end, then installs the second set of handlers (AncientKing_ThinkAfterSwitch,
   AncientKing_DeathLaunchShot, EntityAi_Animate, EntityAi_OnHit, EntityAi_Move) and thinks. */
static void AncientKing_SwitchHandlers(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)entity->ai_state.motion_slot_0, 0, 2, 0, entity->params->motion_blend_frames, 0);
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)entity->ai_state.motion_slot_1, 1, 2, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (((EntityAiMotionSlot *)ai_state->motion_slot_0)->frames_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ai_state->animate = EntityAi_Animate;
            ai_state->on_hit = EntityAi_OnHit;
            ai_state->move = EntityAi_Move;
            ai_state->death = AncientKing_DeathLaunchShot;
            entity->ai_state.think = AncientKing_ThinkAfterSwitch;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
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

/* The think handler that AncientKing_SwitchHandlers installs: as EntityAi_Think, with
   AncientKing_ThinkCombatAfterSwitch for state 2 and EntityAi_ThinkIdle for states 0 and 1. Clears flag
   bit 25 once the entity is more than 160 from its spawn point (EntityMath_DistanceXZ). Counts down
   ai_state.pause_timer. */
static void AncientKing_ThinkAfterSwitch(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) > 160.0f) {
        ENTITY_FLAG_BITS(entity)->no_map_object_collision = 0;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_NOTICED:
                AncientKing_ThinkCombatAfterSwitch(entity, run_now);
                break;
            case ENTITY_STATE_UNAWARE:
            case ENTITY_STATE_SEARCHING:
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
    if (ai_state->pause_timer > 0) {
        ai_state->pause_timer--;
    }
}

/* The think function of state 2 for AncientKing_ThinkAfterSwitch: as EntityAi_ThinkCombat, but it starts
   AncientKing_RespawnOthersAfterSwitch when AncientKing_ShouldRespawnOthers says so, and ranged records 0 and 1 have the
   type's own actions (AncientKing_LaunchShotsNearPlayer, AncientKing_RangedAttack1). */
static void AncientKing_ThinkCombatAfterSwitch(Entity *entity, s32 run_now) {
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
    if (AncientKing_ShouldRespawnOthers(entity)) {
        EntityAi_StartAction(entity, AncientKing_RespawnOthersAfterSwitch, run_now);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
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
                    EntityAi_StartAction(entity, AncientKing_LaunchShotsNearPlayer, run_now);
                } else if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                    EntityAi_StartAction(entity, AncientKing_RangedAttack1, run_now);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
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

/* The death action (busy bit 0x40) that AncientKing_SwitchHandlers installs: as EntityAi_Death; as
   animation 16 passes frame 43 it launches shot launches[0].magic_effect of ranged record 0 with the player's
   position as its starting point (EntityAi_LaunchShot). */
static void AncientKing_DeathLaunchShot(Entity *entity) {
    f32 target[4];

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FLY) {
            entity->collision_radius = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if ((entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8 < 0x2B &&
            entity->model->level_tables->frame >= 0x2B) {
            Vector_Copy(target, &player_state.position);
            EntityAi_LaunchShot(entity, ((EntityParamsUnk3D8Full *)&entity->params->ranged_attacks[0])->launches[0].magic_effect, target,
                          entity->model->rotation);
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1) of the handlers AncientKing_SwitchHandlers installs: as
   AncientKing_RangedAttack0, but animation 23 plays on the main motion and the starting points lie up to 200
   from the player's position in x and in z. */
static void AncientKing_LaunchShotsNearPlayer(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->ranged_attacks[0];
    f32 target[4];
    EntityFrameTable5 launch_frames = ancient_king_near_shot_frames;
    s32 elapsed;
    s32 i;

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
        break;
    case ACTION_STEP_PLAY:
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        for (i = 0; i < 5; i++) {
            if (entity->ailment_4_frames == 0 || unk3D8->ignores_ailment_4 != 0) {
                if (elapsed < launch_frames.frames[i] && entity->model->level_tables->frame >= launch_frames.frames[i]) {
                    Vector_Copy(target, &player_state.position);
                    target[0] += rand() % 401 - 200;
                    target[2] += rand() % 401 - 200;
                    EntityAi_LaunchShot(entity, unk3D8->launches[0].magic_effect, target, entity->model->rotation);
                }
            }
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

/* The ranged attack with record 1 (busy bit 1): animation 24; on each of the four frames in
   ancient_king_bone_shot_frames it launches the record's shot launches[0].magic_effect from the point Model_GetSkinnedBonePoint gives for
   EntityParams.type_param_2, heading away from the entity's own position, with a pitch of -10 degrees
   (EntityAi_LaunchShot). */
static void AncientKing_RangedAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)ENTITY_PARAMS_UNK3D8(entity->params, 1);
    f32 direction[4];
    EntityFrameTable launch_frames = ancient_king_bone_shot_frames;
    f32 *source;
    f32 delta_x;
    f32 delta_z;
    s32 elapsed;
    s32 i;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x18, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        for (i = 0; i < 4; i++) {
            if (entity->ailment_4_frames == 0 || unk3D8->ignores_ailment_4 != 0) {
                if (elapsed < launch_frames.frames[i] && entity->model->level_tables->frame >= launch_frames.frames[i]) {
                    source = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), entity->params->type_param_2);
                    if (source != NULL) {
                        delta_x = source[0] - entity->model->position[0];
                        delta_z = source[2] - entity->model->position[2];
                        direction[0] = -DEG_TO_RAD(10);
                        direction[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                        direction[2] = direction[3] = 0.0f;
                        EntityAi_LaunchShot(entity, unk3D8->launches[0].magic_effect, source, direction);
                    }
                }
            }
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

/* A second action with animation 17 (busy bit 0x1000), which AncientKing_ThinkCombatAfterSwitch
   starts: as AncientKing_RespawnOthers, but on the model's one motion (Motion_Start) where that
   one uses the second motion slot. It belongs to the handlers AncientKing_SwitchHandlers installs.
   */
static void AncientKing_RespawnOthersAfterSwitch(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 1);
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (((EntityAiMotionSlot *)ai_state->motion_slot_1)->frame == entity->params->far_distance) {
            EntityEvent_StartEffectAtPoint(entity, entity->params->near_distance, entity->params->type_param_2, entity->model->rotation);
        }
        if (entity->params->type_param_1 < entity->model->level_tables->frame) {
            EntityEvent_StartEffectAtPoint(entity, entity->params->type_param_0, entity->params->type_param_2, entity->model->rotation);
            AncientKing_RespawnTwoOthers(entity);
            entity->action_step++;
        }
        break;
    case ANCIENT_KING_RESPAWN_STEP_END:
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

/* Whether to start AncientKing_RespawnOthers or AncientKing_RespawnOthersAfterSwitch (the two
   think functions of state 2 ask): counts the entities with spawn ids 10 to 15 that have flag bits
   0 and 1 set and flag bits 2-3 below 3 (the count goes to ai_state.step_index, a bit each to
   ai_state.sub_timer); TRUE when there are fewer than 5 and the pause in ai_state.pause_timer is over. */
static s32 AncientKing_ShouldRespawnOthers(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 count = 0;
    s32 i;

    ai_state->sub_timer = 0;
    for (i = 0; i < 6; i++) {
        Entity *other = Entity_GetBySpawnId(i + 10);
        if (other != NULL && (other->flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE)) == (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE) && ENTITY_FLAG_BITS(other)->dying < 3) {
            ai_state->sub_timer |= 1 << i;
            count++;
        }
    }
    ai_state->step_index = count;
    if (ai_state->step_index < 5) {
        if (ai_state->pause_timer < 1) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Called by AncientKing_RespawnOthers and AncientKing_RespawnOthersAfterSwitch: draws up to two
   of the entities with spawn ids 10 to 15 that AncientKing_ShouldRespawnOthers did not count,
   respawns each that has flag bit 0 set (EntityEvent_Respawn, which calls Entity_ResetToSpawn), and sets
   the pause in ai_state.pause_timer to EntityParams.action_frames. */
static void AncientKing_RespawnTwoOthers(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 remaining = 2;
    s32 chance = 200;
    s32 i;
    s32 slots_left = 6 - ai_state->step_index;

    for (i = 0; i < 6; i++) {
        if (!((ai_state->sub_timer >> i) & 1)) {
            if (rand() % 100 < chance / slots_left) {
                Entity *other = Entity_GetBySpawnId(i + 10);
                if (other != NULL && ((s32)other->flags & ENTITY_FLAG_ACTIVE_LIST)) {
                    chance -= 100;
                    EntityEvent_Respawn(other);
                    remaining--;
                }
            }
            if (remaining <= 0) {
                break;
            }
            slots_left--;
            if (slots_left <= 0) {
                break;
            }
        }
    }
    ai_state->pause_timer = entity->params->action_frames;
}
