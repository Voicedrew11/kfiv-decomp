/* Slime: the behaviour of entity type 1 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Green Slime, Poison Slime. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "player/player.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_math.h"
#include <libvu0.h>
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"


/* Entity.ai_state.move_choice; 1 means one thing while wandering, another in combat */
#define SLIME_MOVE_CHOICE_TURN_RANDOM   1   /* wandering: turns to a random heading */
#define SLIME_MOVE_CHOICE_TO_POINT      1   /* combat: walks to a point near its spawn */
#define SLIME_MOVE_CHOICE_STAND         2   /* stands while the wander timer runs (no target) */

static void Slime_Animate(Entity *entity);
static void Slime_OnHit(Entity *entity);
static void Slime_Think(Entity *entity, s32 run_now);
static void Slime_ThinkReturn(Entity *entity, s32 run_now);
static void Slime_ThinkWander(Entity *entity, s32 run_now);
static void Slime_ThinkCombat(Entity *entity, s32 run_now);
static void Slime_Move(Entity *entity);
static void Slime_MeleeAttack(Entity *entity);

/* Set-up of a type 1 entity: clears its work area and installs the type's handlers. */
void Slime_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Slime_Think;
    ai_state->animate = Slime_Animate;
    ai_state->on_hit = Slime_OnHit;
    ai_state->move = Slime_Move;
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
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void Slime_Animate(Entity *entity) {
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
}

/* The on_hit handler: every hit starts EntityAi_ReactionHeavy, whatever its strength; the target
   and flag bits 9 and 10 are left as they are. */
static void Slime_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    EntityAi_SetState(entity, 2);
}

/* The think handler: as EntityAi_Think, with an if in place of the switch: Slime_ThinkReturn
   while flag bit 53 is set, Slime_ThinkCombat in state 2, Slime_ThinkWander in the other states.
   */
static void Slime_Think(Entity *entity, s32 run_now) {
    u32 phase;

    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        Slime_ThinkReturn(entity, run_now);
        return;
    }
    if (entity->state == ENTITY_STATE_NOTICED) {
        Slime_ThinkCombat(entity, run_now);
        return;
    }
    Slime_ThinkWander(entity, run_now);
}

/* The think function while flag bit 53 is set: makes the spawn point the target, gives command 1
   and starts the move action (Slime_Move goes straight for the target). */
static void Slime_ThinkReturn(Entity *entity, s32 run_now) {
    f32 unused[8]; // match: retail reserves 0x20 bytes, which also keeps the call from being a jump

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    entity->target = entity->spawn_info;
    entity->move_command = MOVE_COMMAND_FORWARD;
    entity->action_timer = 0;
    EntityAi_StartAction(entity, entity->ai_state.move, run_now);
}

/* The think function of every state but 2 (Slime_Think calls it): wanders. Each time the
   timer runs out it draws: walk to a point near the spawn point (EntityPoint_PickNear; forward command
   while farther than 50), turn on the spot to a random heading (EntityAi_RandomAngle), or stand; then
   starts the move action. */
static void Slime_ThinkWander(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 roll;
    f32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    if (entity->action_timer == 0) {
        roll = rand() % 100;
        if (roll > 60) {
            EntityPoint_PickNear(entity->spawn_info->wander_radius, entity->ai_state.target_point, entity->spawn_info, entity->model->position);
            entity->target = entity->ai_state.target_point;
            entity->action_timer = rand() % 120 + 240;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
        } else if (roll > 40) {
            ai_state->part_angle_x = EntityAi_RandomAngle();
            entity->action_timer = rand() % 30 + 60;
            ai_state->move_choice = SLIME_MOVE_CHOICE_TURN_RANDOM;
        } else {
            entity->target = NULL;
            entity->action_timer = rand() % 30 + 60;
            ai_state->move_choice = SLIME_MOVE_CHOICE_STAND;
        }
    }
    if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
        if (entity->target != NULL) {
            if (EntityMath_DistanceXZ(entity->model->position, entity->target) > 50.0f) {
                entity->move_command = MOVE_COMMAND_FORWARD;
            }
        }
    } else if (ai_state->move_choice == SLIME_MOVE_CHOICE_TURN_RANDOM) {
        turn = EntityMath_AngleDifference(entity->model->rotation[1], ai_state->part_angle_x);
        if (ABS(turn) > entity->params->turn_limit) {
            entity->move_command = (turn >= 0.0f) ? MOVE_COMMAND_TURN_RIGHT : MOVE_COMMAND_TURN_LEFT;
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think function of state 2 (called under `entity->state == 2`, not from a switch): guards the
   player's swing and picks among attacking, closing in and waiting as EntityAi_ThinkCombat does; it
   has no dodge, and its melee attack is Slime_MeleeAttack. Closing in is for 480 to 599
   frames, on the player (EntityParams.pitch_limit_deg times in 100) or on a point near the spawn point, with
   command 1 while the target is farther than the range. */
static void Slime_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
                return;
            }
            EntityAi_StartAction(entity, Slime_MeleeAttack, run_now);
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            if (rand() % 100 < entity->params->pitch_limit_deg) {
                entity->target = &player_state;
                entity->action_timer = rand() % 120 + 480;
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            } else {
                EntityPoint_PickNear(entity->spawn_info->wander_radius, entity->ai_state.target_point, entity->spawn_info, entity->model->position);
                entity->target = entity->ai_state.target_point;
                entity->action_timer = rand() % 120 + 480;
                ai_state->move_choice = SLIME_MOVE_CHOICE_TO_POINT;
            }
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    switch (ai_state->move_choice) {
    case MOVE_CHOICE_CLOSE_IN:
        if (entity->target != NULL) {
            if (EntityMath_DistanceXZ(entity->model->position, entity->target) > entity->radius + 100.0f + 30.0f) {
                entity->move_command = MOVE_COMMAND_FORWARD;
            }
        }
        break;
    case SLIME_MOVE_CHOICE_TO_POINT:
        if (entity->target != NULL) {
            if (EntityMath_DistanceXZ(entity->model->position, entity->target) > 50.0f) {
                entity->move_command = MOVE_COMMAND_FORWARD;
            }
        }
        break;
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The move handler: turns as the command bits in Entity.move_command say and, with command bit 1, moves
   straight towards the target, whatever the heading; picks animation 4 or 3 for that, else 0. */
static void Slime_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *vector;
    f32 turn = 0.0f;
    f32 amount;
    f32 heading;

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
    } else {
        ai_state->turn_rate = 0.0f;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if ((entity->move_command & MOVE_COMMAND_FORWARD) && entity->target != NULL) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[3] = 0.0f;
        amount = -1.0f;
        vector[2] = amount;
        ai_state->move_speed = EntityAi_Accelerate(entity, ai_state->move_speed);
        amount = ai_state->move_speed;
        if (entity->move_command & MOVE_COMMAND_RUN) {
            amount += amount;
        }
        heading = EntityMath_HeadingTo(entity->model->position, entity->target);
        sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, identity_matrix, heading);
        Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
        Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
        entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
        entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
        entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
    } else {
        ai_state->move_speed = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack; a hit of any of the three parts also
   clears player_state.timed_flags[0], EntityParams.yaw_limit_deg times in 100. */
static void Slime_MeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[ENTITY_FLAG_BITS(entity)->attack_index];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
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
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->level_tables->frame);
            if (ENTITY_FLAG_BITS(entity)->melee_0_hit && rand() % 100 < entity->params->yaw_limit_deg) {
                player_state.timed_flags[0] = 0;
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->level_tables->frame);
            if (ENTITY_FLAG_BITS(entity)->melee_1_hit && rand() % 100 < entity->params->yaw_limit_deg) {
                player_state.timed_flags[0] = 0;
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->level_tables->frame);
            if (ENTITY_FLAG_BITS(entity)->melee_2_hit && rand() % 100 < entity->params->yaw_limit_deg) {
                player_state.timed_flags[0] = 0;
            }
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
