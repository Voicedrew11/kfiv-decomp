/* Salamander: the behaviour of entity type 22 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Salamander. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "player/player.h"
#include "entity/entity_aim.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai.h"

/* ai_state->move_choice: 1 waits in view and attacks, 2 backs off from the player */
#define SALAMANDER_MOVE_CHOICE_ATTACK_IN_VIEW 1
#define SALAMANDER_MOVE_CHOICE_BACK_OFF        2

/* Salamander_ReactionLight: Entity.reaction_step */
/* Salamander_Death: Entity.action_step */
#define SALAMANDER_DEATH_STEP_FALL              2       /* waits for frame 30, then the body falls free */
#define SALAMANDER_DEATH_STEP_LEVEL             3       /* eases the body level while the fall plays */
#define SALAMANDER_DEATH_STEP_FADE_OUT          4       /* fades the model out over action_timer, then frees it */

static void Salamander_Animate(Entity *entity);
static void Salamander_OnHit(Entity *entity);
static void Salamander_ReactionRewind(Entity *entity);
static void Salamander_ReactionLight(Entity *entity);
static void Salamander_ReactionHeavy(Entity *entity);
static void Salamander_Death(Entity *entity);
static void Salamander_MeleeAttack(Entity *entity);
static void Salamander_RangedAttack(Entity *entity);
static void Salamander_Think(Entity *entity, s32 run_now);
static void Salamander_ThinkReturn(Entity *entity, s32 run_now);
static void Salamander_ThinkWanderAndShoot(Entity *entity, s32 run_now);
static void Salamander_ThinkCombat(Entity *entity, s32 run_now);
static void Salamander_Move(Entity *entity);

/* Set-up of a type 22 entity: clears its work area and installs the type's handlers. */
void Salamander_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *choice_weights = entity->choice_weights;
    u8 *attack_weights = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = ENTITY_WORK_BYTES(entity);

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Salamander_Think;
    ai_state->animate = Salamander_Animate;
    ai_state->on_hit = Salamander_OnHit;
    ai_state->move = Salamander_Move;
    ai_state->death = Salamander_Death;
    ai_state->dormant = NULL;
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
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_SetPartSlots(entity->model, 10, 0x13, 1);
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.bit5 = 1;
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_0.bit6 = 1;
    ((EntityAiStateKind151D30 *)ai_state)->motion_slot_1.bit6 = 1;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: plays the two motion slots of the work area (unk80 and unk88) where
   EntityAi_Animate plays the model's one motion, then turns the head (EntityAi_LookAtPoint with the target
   while flag bit 9 is set and there is one). */
static void Salamander_Animate(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    f32 limits[4];
    f32 unused_vector[4]; // match: retail reserves 16 more bytes of stack than it uses
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
        EntityAi_StepSlotMotion(entity, &slots->motion_slot_1, 1, rate);
    } else {
        previous = slots->motion_slot_1.frame_fixed;
        Motion_StepSlot(entity, &slots->motion_slot_1, 1, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        if (slots->motion_slot_1.index == 3) {
            EntityEvent_StartFrameEffects(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
            EntityEvent_StartFrameCues(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
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

/* The on_hit handler: as EntityAi_OnHit with the type's own three reactions; it leaves flag bit 10
   as it is and sets work byte unk7A. */
static void Salamander_OnHit(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 1) {
        if ((s32)(entity->flags >> 19) & ENTITY_FLAG_ACTIVE_LIST) {
            EntityAi_SetReaction(entity, Salamander_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, Salamander_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, Salamander_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
    ai_state->step_index = 1;
}

/* The reaction to a light hit while flag bit 19 is set (busy bit 2 while it blends in): as
   EntityAi_ReactionRewind, on the first motion slot. */
static void Salamander_ReactionRewind(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_REACTION_LIGHT;
    switch (entity->reaction_step) {
    case ACTION_STEP_START:
        frame = ai_state->motion_slot_0.frame - 3;
        if (frame < 0) {
            frame = 0;
        }
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&ai_state->motion_slot_0, 0, ai_state->motion_slot_0.motion_bits & 0x1F, frame, 5, 1);
        entity->reaction_step++;
        entity->flags = ((entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1) & ~ENTITY_FLAG_LOOK_AT_TARGET;
        break;
    case ACTION_STEP_BLEND_IN:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->busy_flags &= ~ENTITY_BUSY_REACTION_LIGHT;
            EntityAi_SetReaction(entity, NULL, 0);
        }
        break;
    }
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the first motion slot,
   without the call of EntityEvent_StartFrameCues. */
static void Salamander_ReactionLight(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_LIGHT;
    switch (entity->reaction_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind1740A8 *)&entity->ai_state)->motion_slot_0, 0, 0xE, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = FALSE;
        ENTITY_FLAG_BITS(entity)->guarding = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->reaction_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (ai_state->motion_slot_0.frames_left == 0) {
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

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the first motion slot. */
static void Salamander_ReactionHeavy(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&((EntityAiStateKind1740A8 *)&entity->ai_state)->motion_slot_0, 0, 0xF, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = FALSE;
        ENTITY_FLAG_BITS(entity)->guarding = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->reaction_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (ai_state->motion_slot_0.frames_left == 0) {
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

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots, with Entity.collision_radius set
   to 20; from frame 30 of animation 16 it clears flag bit 28 and brings the model's pitch and roll
   back to 0. */
static void Salamander_Death(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&ai_state->motion_slot_0, 0, 0x10, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&ai_state->motion_slot_1, 1, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        entity->collision_radius = 20.0f;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        break;
    case SALAMANDER_DEATH_STEP_FALL:
        if (ai_state->motion_slot_0.frame >= 30) {
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            entity->action_step++;
        }
        break;
    case SALAMANDER_DEATH_STEP_LEVEL:
        EntityMath_EaseAngleToward(&entity->model->rotation[0], 0.0f, 10.0f);
        EntityMath_EaseAngleToward(&entity->model->rotation[2], 0.0f, 10.0f);
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case SALAMANDER_DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, on the first motion slot. */
static void Salamander_MeleeAttack(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[kind];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&ai_state->motion_slot_0, 0, kind | ENTITY_MOTION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (ai_state->motion_slot_0.frame <= unk240->hit_start_frame) {
            EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0;
        }
        elapsed = (ai_state->motion_slot_0.frame_fixed - entity->params->motion_speeds[((EntityAiMotionSlotBits *)&ai_state->motion_slot_0)->index]) >> 8;
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, ai_state->motion_slot_0.frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_1_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, ai_state->motion_slot_0.frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_2_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, ai_state->motion_slot_0.frame);
        }
        if (ai_state->motion_slot_0.frame > 0 && unk240->hit_end_frame >= ai_state->motion_slot_0.frame) {
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
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, on the first motion slot; with
   animation 23 it launches the record's shots every other frame from frame 20 to 44 instead, the
   head following the target meanwhile. */
static void Salamander_RangedAttack(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)&ai_state->motion_slot_0, 0, kind + ENTITY_MOTION_RANGED_ATTACK, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (ai_state->motion_slot_0.frame <= unk3D8->turn_frames) {
            EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0;
        }
        if (((EntityAiMotionSlotBits *)&ai_state->motion_slot_0)->index == 0x17) {
            if (ai_state->motion_slot_0.frame >= 20 && ai_state->motion_slot_0.frame <= 44) {
                if (frame_counter & 1) {
                    EntityAi_LaunchShots(entity, unk3D8, 20, 44);
                }
                ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
            }
        } else {
            EntityAi_LaunchShots(entity, unk3D8,
                          (ai_state->motion_slot_0.frame_fixed - entity->params->motion_speeds[((EntityAiMotionSlotBits *)&ai_state->motion_slot_0)->index]) >> 8,
                          ai_state->motion_slot_0.frame);
        }
        if (ai_state->motion_slot_0.frame > 0 && unk3D8->attack_end_frame >= ai_state->motion_slot_0.frame) {
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

/* The think handler: as EntityAi_Think, with EntityAi_CheckFarFromSpawn3D where that one calls EntityAi_CheckFarFromSpawn,
   Salamander_ThinkReturn while flag bit 53 is set, Salamander_ThinkCombat for states 1 and 2,
   and Salamander_ThinkWanderAndShoot for state 0 and, as long as work byte unk7A is 0 (Salamander_OnHit sets it),
   for every state. Keeps animation 3 on the second motion slot except in the death action. */
static void Salamander_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (!(entity->busy_flags & ENTITY_BUSY_DEATH)) {
        EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)ai_state->motion_slot_1, 1, 3, 0, entity->params->motion_blend_frames, 0);
    }
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn3D(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        Salamander_ThinkReturn(entity, run_now);
        return;
    }
    if (ai_state->step_index == 0) {
        Salamander_ThinkWanderAndShoot(entity, run_now);
        return;
    }
    switch (entity->state) {
    case ENTITY_STATE_UNAWARE:
        break;
    case ENTITY_STATE_SEARCHING:
    case ENTITY_STATE_NOTICED:
        Salamander_ThinkCombat(entity, run_now);
        return;
    }
    Salamander_ThinkWanderAndShoot(entity, run_now);
}

/* The think function while flag bit 53 is set: walks back to the spawn point, turning away from
   whatever is pushing the entity, as EntityAi_ThinkReturn; with the command of EntityAi_MoveCommand3D and,
   while it is pushed, command bit 0x20 or 0x10 by the y of the push. */
static void Salamander_ThinkReturn(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    entity->action_timer = 0;
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (ENTITY_FLAG_WALL_PUSH | ENTITY_FLAG_400000000000)) {
            push[0] += entity->wall_push[0];
            push[1] += entity->wall_push[1];
            push[2] += entity->wall_push[2];
        }
        if (ENTITY_FLAG_BITS(entity)->entity_push) {
            push[0] += entity->entity_push[0];
            push[1] += entity->entity_push[1];
            push[2] += entity->entity_push[2];
        }
        Vector_Normalize(push, push);
        Vector_Subtract(to_spawn, entity->spawn_info, entity->model->position);
        Vector_Normalize(to_spawn, to_spawn);
        Vector_Cross(to_spawn, push, to_spawn);
        if (to_spawn[1] <= 0.0f) {
            if (turn == -1) {
                turn = 1;
            }
        } else if (turn == 1) {
            turn = -1;
        }
        entity->target = entity->spawn_info;
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            if (push[1] > 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_UP | MOVE_COMMAND_TURN_RIGHT);
            } else if (push[1] < 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_DOWN | MOVE_COMMAND_TURN_RIGHT);
            }
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            if (push[1] > 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_UP | MOVE_COMMAND_TURN_LEFT);
            } else if (push[1] < 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_DOWN | MOVE_COMMAND_TURN_LEFT);
            }
        } else {
            entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think function of state 0, and of every state until Salamander_OnHit sets work byte unk7A: starts
   Salamander_RangedAttack (EntityAi_PickRangedAttack picks the record) when the player is in view
   (EntityAi_IsPointInView with EntityParams.type_param_0 and type_param_1 degrees and type_param_2) and the pause in work word
   unk6A (EntityParams.far_distance) is over; else moves in three dimensions to a random point near the spawn point
   (EntityPoint_PickNearWithHeight), a new one every 240 to 359 frames. */
static void Salamander_ThinkWanderAndShoot(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 *point;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    if (EntityAi_IsPointInView(entity, &player_state.position.x, DEG_TO_RAD(entity->params->type_param_0),
                      DEG_TO_RAD(entity->params->type_param_1), entity->params->type_param_2) &&
        ai_state->sub_timer <= 0) {
        EntityAi_PickRangedAttack(entity);
        // match: an int result keeps $2 busy after the call
        EntityAi_StartAction(entity, Salamander_RangedAttack, run_now);
        ai_state->sub_timer = entity->params->far_distance;
        return;
    }
    if (ai_state->sub_timer > 0) {
        ai_state->sub_timer--;
    }
    if (entity->action_timer == 0) {
        point = ai_state->target_point;
        EntityPoint_PickNearWithHeight(point, entity->spawn_info->wander_radius, entity->spawn_info->wander_height_range, entity->spawn_info,
                      entity->model->position);
        entity->target = point;
        entity->action_timer = rand() % 120 + 240;
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->move_command = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think function of states 1 and 2: it has none of EntityAi_ThinkCombat's guard, dodge and draw
   among the actions, but a sequence in work byte unk78. 0: goes for a point 220 below the player
   until the player is nearer than EntityParams.action_frames. 1: goes for a point 190 below the player,
   and when EntityAi_IsPointInView finds that point within the angles EntityParams.type_param_0 and type_param_1 and the
   distance type_param_2, starts Salamander_RangedAttack or Salamander_MeleeAttack as EntityAi_PickAttack
   draws. 2: goes for a point 800 from the player along the reverse of Entity.heading_to_player, at the spawn
   point's y, for EntityParams.near_distance frames (work word unk68) or until the player is unk434 away.
   The commands come from EntityAi_MoveCommand3D, or EntityAi_PushTurnCommand3D while it is pushed. */
static void Salamander_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->move_command = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        switch (ai_state->move_choice) {
        case MOVE_CHOICE_CLOSE_IN: {
            f32 *point = ai_state->target_point;

            Vector_Copy(point, &player_state);
            ai_state->target_point[1] -= 220.0f;
            entity->target = point;
            entity->move_command = EntityAi_MoveCommand3D(entity, 0, entity->radius + 100.0f + 30.0f);
            if (entity->distance_to_player < (f32)ENTITY_CONTROLLER(entity)->action_frames) {
                ai_state->move_choice++;
            }
            break;
        }
        case SALAMANDER_MOVE_CHOICE_ATTACK_IN_VIEW: {
            f32 *point = ai_state->target_point;

            Vector_Copy(point, &player_state);
            ai_state->target_point[1] -= 190.0f;
            entity->target = point;
            if (EntityAi_IsPointInView(entity, point, DEG_TO_RAD(entity->params->type_param_0),
                              DEG_TO_RAD(entity->params->type_param_1), entity->params->type_param_2)) {
                if (EntityAi_PickAttack(entity)) {
                    EntityAi_StartAction(entity, Salamander_RangedAttack, run_now);
                } else {
                    EntityAi_StartAction(entity, Salamander_MeleeAttack, run_now);
                }
                ai_state->pause_timer = entity->params->near_distance;
                ai_state->move_choice++;
                return;
            }
            entity->move_command = EntityAi_MoveCommand3D(entity, 0, entity->radius + 100.0f + 30.0f);
            break;
        }
        case SALAMANDER_MOVE_CHOICE_BACK_OFF: {
            f32 *point = ai_state->target_point;

            direction[0] = EntityMath_WrapAngle(entity->heading_to_player + PI);
            EntityMath_FacingVector((Vector4 *)point, direction[0]);
            Vector_Scale(point, point, 800.0f);
            Vector_Add(point, point, &player_state);
            entity->target = point;
            ai_state->target_point[1] = entity->spawn_info->spawn_y;
            entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
            if (entity->distance_to_player >= (f32)ENTITY_CONTROLLER(entity)->action_frames || ai_state->pause_timer <= 0) {
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            }
            break;
        }
        }
    }
    if (ai_state->pause_timer > 0) {
        ai_state->pause_timer--;
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The move handler: turns as the command says, rolls the model 20 degrees into the turn, and
   pitches it up to 30 degrees for command bits 0x10 and 0x20 (through Entity.turn_delta_pitch); keeps
   animation 3 on the first motion slot. It adds no movement of its own. */
static void Salamander_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 value;
    f32 roll;
    f32 pitch;
    f32 pitch_change;
    f32 pitch_rate;

    if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
        if (ai_state->turn_rate < -entity->params->turn_limit) {
            ai_state->turn_rate = -entity->params->turn_limit;
        }
        roll = DEG_TO_RAD(20);
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
        if (ai_state->turn_rate > entity->params->turn_limit) {
            ai_state->turn_rate = entity->params->turn_limit;
        }
        roll = -DEG_TO_RAD(20);
    } else {
        ai_state->turn_rate = 0.0f;
        roll = ai_state->turn_rate;
    }
    value = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        value += value;
    }
    entity->turn_delta_yaw += value;
    EntityMath_EaseAngleToward(&entity->model->rotation[2], roll, 30.0f);
    if (entity->move_command & MOVE_COMMAND_DOWN) {
        pitch = -DEG_TO_RAD(30);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        pitch = DEG_TO_RAD(30);
    } else {
        pitch = 0.0f;
    }
    pitch_change = pitch - entity->model->rotation[0];
    if (ABS(pitch_change) > DEG_TO_RAD(30)) {
        pitch_rate = 60.0f;
    } else {
        pitch_rate = 30.0f;
    }
    value = entity->model->rotation[0];
    EntityMath_EaseAngleToward(&value, pitch, pitch_rate);
    entity->turn_delta_pitch += value - entity->model->rotation[0];
    EntityAi_StartSlotMotion(entity, (EntityMotionSlotWord *)ai_state->motion_slot_0, 0, 3, 0, entity->params->motion_blend_frames, 0);
}
