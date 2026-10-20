/* The talking characters: entity types 15 and 81. Every character a Recollection page describes has a chr.prm
   record of type 15 (from the game data). Type 15 turns to the player and watches it while scripts put it in
   states 4 to 6; type 81 only stands and walks back to its spawn point. */
#include "entity/npc.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_events.h"
#include "model/motion.h"
#include "entity/entity_math.h"
#include "enemy/enemy.h"
#include <libvu0.h>
#include "math/vu0.h"
#include "entity/entity_ai.h"
#include "entity/entity.h"
#include "player/player.h"
#include "save/save_state.h"


static void Npc_Animate(Entity *entity);
static void Npc_OnHit(Entity *entity);
static void Npc_ThinkStand(Entity *entity, int unknown);
static void Npc_ThinkReactToPlayer(Entity *entity, s32 run_now);
static void Npc_ThinkTurnToPlayer(Entity *entity, s32 run_now);
static void Npc_ThinkWatchPlayer(Entity *entity, int unknown);
static void Npc_ThinkStopWatching(Entity *entity, int unknown);
static void Npc_ThinkTurnBack(Entity *entity, s32 run_now);
static void Npc_TurnToPlayer(Entity *entity);
static void Npc_TurnToSpawnHeading(Entity *entity);
static void Npc_ReactionHeavy(Entity *entity);
static void Npc_Death(Entity *entity);
static void Npc_Move(Entity *entity);
static void Npc_OnHitType81(Entity *entity);
static void Npc_ThinkType81(Entity *entity, s32 run_now);
static int Npc_ThinkType81Stand(void *entity_ptr, int unknown);

/* The set-up function of entity type 15, the talking characters (chr.prm): as EntityAi_InitDefaults, with the
   Npc_ handlers, its look part from EntityParams.type_param_1, and flag bit 16 set. */
void Npc_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    EntityModelPart80 *part;
    s32 i;

    part = (EntityModelPart80 *)entity->model->parts + entity->params->type_param_1;
    entity->model->look_at_part = part;
    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Npc_Think;
    ai_state->animate = Npc_Animate;
    ai_state->on_hit = Npc_OnHit;
    ai_state->move = Npc_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = Npc_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26) {
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    }
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    if (part != NULL) {
        Vector_Copy(ai_state->look_part_angles, part->part_angles);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    // match: called as returning an int, which keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))Motion_StepModel)(entity->model, 0, 0);
    ENTITY_FLAG_BITS(entity)->not_targetable = TRUE;
}

/* The animate handler of types 15 and 81: as EntityAi_Animate, but the blend runs at ai_state->unk40 only in a
   busy-bit-2 reaction, and the head turns at the rates EntityParams.action_frames (towards the target) and unk436. */
static void Npc_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->look_at_target;
        limits[1] = DEG_TO_RAD(entity->params->pitch_limit_deg);
        limits[0] = DEG_TO_RAD(-entity->params->pitch_limit_deg);
        limits[3] = DEG_TO_RAD(entity->params->yaw_limit_deg);
        limits[2] = DEG_TO_RAD(-entity->params->yaw_limit_deg);
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, ENTITY_CONTROLLER(entity)->action_frames);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, entity->params->near_distance);
        }
    }
}

/* The on_hit handler of type 15: lowers the weights of the last choice (EntityAi_LowerWeightsAfterHit),
   forgets the target, stops the head following it, starts Npc_ReactionHeavy and marks its group record
   (entity_group_state[EntityParams.type_param_0].unk02). */
static void Npc_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    EntityAi_SetReaction(entity, Npc_ReactionHeavy, 0);
    entity_group_state[ENTITY_CONTROLLER(entity)->entity_group_index].member_died = 1;
}

/* The think handler of type 15, while not busy: state 4 turns to the player, state 5 watches the player until
   it is farther than EntityParams.attack_range (then state 6), state 6 turns back; any other state stands. */
void Npc_Think(Entity *entity, s32 unknown) {
    if (entity->busy_flags == 0) {
        switch (entity->state) {
        case 4:
            if (entity->params->flags & ENTITY_PARAMS_FLAG_TURNS_IN_PLACE) {
                Npc_ThinkTurnToPlayer(entity, unknown);
            } else {
                Npc_ThinkReactToPlayer(entity, unknown);
            }
            break;
        case 5:
            Npc_ThinkWatchPlayer(entity, unknown);
            if (entity->params->attack_range < entity->distance_to_player) {
                EntityAi_SetState(entity, 6);
            }
            break;
        case 6:
            if (entity->params->flags & ENTITY_PARAMS_FLAG_TURNS_IN_PLACE) {
                Npc_ThinkTurnBack(entity, unknown);
            } else {
                Npc_ThinkStopWatching(entity, unknown);
            }
            break;
        case 0:
        case 1:
        case 2:
        case 3:
        default:
            Npc_ThinkStand(entity, unknown);
            break;
        }
    }
}

/* The think function of states 0-3: the head stops following the target and the move action runs with no
   command (standing). */
static void Npc_ThinkStand(Entity *entity, int unknown) {
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    EntityAi_StartAction(entity, entity->ai_state.move, unknown);
}

/* The think function of state 4 when EntityParams.flags has no 0x80: picks the animation by where the player
   is (26 within EntityParams.far_distance degrees of the heading, else 28 or 27 for the side), then state 5. */
static void Npc_ThinkReactToPlayer(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;
    f32 limit;
    f32 direction;

    direction = EntityMath_HeadingTo(entity->model->position, (float *)&player_state.position);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
    limit = DEG_TO_RAD((f32)ENTITY_CONTROLLER(entity)->far_distance);
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        if (0.0f <= angle) {
            ai_state->pause_timer = MOVE_COMMAND_ANIMATION_28;
        } else {
            ai_state->pause_timer = MOVE_COMMAND_ANIMATION_27;
        }
    } else {
        ai_state->pause_timer = MOVE_COMMAND_ANIMATION_26;
    }
    entity->move_command = ai_state->pause_timer;
    EntityAi_SetState(entity, 5);
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think function of state 4 when EntityParams.flags has 0x80: turns to the player (Npc_TurnToPlayer) when
   it is more than EntityParams.far_distance degrees off the heading, else animation 26 and state 5. */
static void Npc_ThinkTurnToPlayer(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;
    f32 limit;
    f32 direction;

    direction = EntityMath_HeadingTo(entity->model->position, (float *)&player_state.position);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
    limit = DEG_TO_RAD((f32)ENTITY_CONTROLLER(entity)->far_distance);
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        EntityAi_StartAction(entity, Npc_TurnToPlayer, run_now);
    } else {
        ai_state->pause_timer = MOVE_COMMAND_ANIMATION_26;
        entity->move_command = MOVE_COMMAND_ANIMATION_26;
        EntityAi_SetState(entity, 5);
        EntityAi_StartAction(entity, ai_state->move, run_now);
    }
}

/* The think function of state 5: the head follows the target (flag bit 9) and the animation picked in state 4
   goes on. */
static void Npc_ThinkWatchPlayer(Entity *entity, int unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    u16 timer = ai_state->pause_timer;

    ENTITY_FLAGS(entity) |= ENTITY_FLAG_LOOK_AT_TARGET;
    entity->move_command = timer;
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 6 when EntityParams.flags has no 0x80: the head stops following the target,
   state 0. */
static void Npc_ThinkStopWatching(Entity *entity, int unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    ai_state->pause_timer = 0;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    EntityAi_SetState(entity, 0);
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 6 when EntityParams.flags has 0x80: turns back to the spawn heading
   (Npc_TurnToSpawnHeading) when more than EntityParams.far_distance degrees off it, else as Npc_ThinkStopWatching. */
static void Npc_ThinkTurnBack(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;
    f32 limit;

    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
    limit = DEG_TO_RAD((f32)ENTITY_CONTROLLER(entity)->far_distance);
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        EntityAi_StartAction(entity, Npc_TurnToSpawnHeading, run_now);
    } else {
        ai_state->pause_timer = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        EntityAi_SetState(entity, 0);
        EntityAi_StartAction(entity, ai_state->move, run_now);
    }
}

/* An action (busy bit 0x1000): plays the turning animation for the side the player is on (27 or 28; 30 or 31
   in spawn mode 1 when its map_script_flags entry is set) and turns the body to the heading towards the player
   (Entity.heading_to_player). */
static void Npc_TurnToPlayer(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 yaw;
    f32 angle;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->map_object_id + entity->spawn_info->variant * 0x40] == entity->spawn_info->second_map_object_id) {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1F, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1E, 0, entity->params->motion_blend_frames, 0);
            }
        } else {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1C, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1B, 0, entity->params->motion_blend_frames, 0);
            }
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_timer = EntityAi_AnimationFrameCount(entity, entity->animation_index);
            entity->action_step++;
        }
        break;
    case 2:
        yaw = entity->model->rotation[1];
        EntityMath_EaseAngleToward(&yaw, entity->heading_to_player, entity->action_timer);
        entity->turn_delta_yaw += yaw - entity->model->rotation[1];
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

/* An action (busy bit 0x1000): as Npc_TurnToPlayer, towards the heading in the spawn record
   (EntitySpawnInfo.spawn_rotation_y). */
static void Npc_TurnToSpawnHeading(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 yaw;
    f32 angle;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->map_object_id + entity->spawn_info->variant * 0x40] == entity->spawn_info->second_map_object_id) {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1F, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1E, 0, entity->params->motion_blend_frames, 0);
            }
        } else {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1C, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1B, 0, entity->params->motion_blend_frames, 0);
            }
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_timer = EntityAi_AnimationFrameCount(entity, entity->animation_index);
            entity->action_step++;
        }
        break;
    case 2:
        yaw = entity->model->rotation[1];
        EntityMath_EaseAngleToward(&yaw, entity->spawn_info->spawn_rotation_y, entity->action_timer);
        entity->turn_delta_yaw += yaw - entity->model->rotation[1];
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

/* The reaction to every hit (busy bit 4): as EntityAi_ReactionHeavy, with animation 18 instead of 15 in spawn
   mode 1 when its map_script_flags entry is set. */
static void Npc_ReactionHeavy(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParams *params;
    s32 kind;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case 0:
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->map_object_id + entity->spawn_info->variant * 0x40] == entity->spawn_info->second_map_object_id) {
            Motion_Start(entity, 0x12, 0, 5, 1);
        } else {
            Motion_Start(entity, 0xF, 0, 5, 1);
        }
        params = entity->params;
        if (params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            kind = 1;
            if (ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_1) {
                kind = 2;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->animates = 1;
        ENTITY_FLAG_BITS(entity)->attack_window = 0;
        ENTITY_FLAG_BITS(entity)->guarding = 0;
        if (ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_WALK) {
            ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
        }
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->reaction_step++;
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

/* The death action of types 15 and 81 (busy bit 0x40): as EntityAi_Death (animation 19 instead of 16 in spawn
   mode 1 when its map_script_flags entry is set); also writes into its group record whether the player killed it
   (1) or not (2), and sets story_flags[13] when the player did. */
static void Npc_Death(Entity *entity) {
    EntityParams *params;
    s32 kind;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->map_object_id + entity->spawn_info->variant * 0x40] == entity->spawn_info->second_map_object_id) {
            Motion_Start(entity, 0x13, 0, 5, 1);
        } else {
            Motion_Start(entity, 0x10, 0, 5, 1);
        }
        params = entity->params;
        if (params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            kind = 1;
            if (ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_1) {
                kind = 2;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        ENTITY_FLAG_BITS(entity)->animates = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->action_timer = 20;
            ENTITY_FLAG_BITS(entity)->animates = 0;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            EntityEvent_OnDeath(entity);
            entity_group_state[ENTITY_CONTROLLER(entity)->entity_group_index].death_kind = ENTITY_FLAG_HIT_BITS(entity)->hit_source ? 1 : 2;
            if (ENTITY_FLAG_HIT_BITS(entity)->hit_source) {
                story_flags[0xD] = 1;
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = 0;
        }
        break;
    }
}

/* The move action of types 15 and 81: as EntityAi_Move for forward and back only; plays animation 17 in spawn
   mode 1 when its map_script_flags entry is set, and animations 26-28 for the commands 0x1000, 0x2000 and 0x4000. */
static void Npc_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *vector;
    f32 turn;
    f32 amount;

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
    } else {
        ai_state->turn_rate = turn;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (ENTITY_FLAG_BITS(entity)->motion_step_mode == 0) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
        if (entity->move_command & MOVE_COMMAND_FORWARD_BACK) {
            amount = 0.0f;
            if (entity->move_command & MOVE_COMMAND_FORWARD) {
                amount = -1.0f;
            } else if (entity->move_command & MOVE_COMMAND_BACK) {
                amount = 1.0f;
            }
            vector[2] += amount;
        }
        if (entity->move_command & MOVE_COMMAND_FORWARD_BACK) {
            ai_state->move_speed = EntityAi_Accelerate(entity, ai_state->move_speed);
            amount = ai_state->move_speed;
            if (entity->move_command & MOVE_COMMAND_RUN) {
                amount += amount;
            }
            sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, identity_matrix, entity->model->rotation[2]);
            sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[0]);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[1]);
            Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
            entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
            entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
        } else {
            ai_state->move_speed = 0.0f;
        }
    } else {
        ai_state->move_speed = 0.0f;
    }
    if (entity->spawn_info->spawn_mode == 1
        && map_script_flags[entity->spawn_info->map_object_id + entity->spawn_info->variant * 0x40] == entity->spawn_info->second_map_object_id) {
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_ANIMATION_26) {
        Motion_Start(entity, 0x1A, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_ANIMATION_27) {
        Motion_Start(entity, 0x1B, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_ANIMATION_28) {
        Motion_Start(entity, 0x1C, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The set-up function of entity type 81: as Npc_Init, with Npc_ThinkType81 and Npc_OnHitType81. */
void Npc_InitType81(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    EntityModelPart80 *part;
    s32 i;

    part = (EntityModelPart80 *)entity->model->parts + entity->params->type_param_1;
    entity->model->look_at_part = part;
    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Npc_ThinkType81;
    ai_state->animate = Npc_Animate;
    ai_state->on_hit = Npc_OnHitType81;
    ai_state->move = Npc_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = Npc_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26) {
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    }
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    if (part != NULL) {
        Vector_Copy(ai_state->look_part_angles, part->part_angles);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    // match: called as returning an int, which keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))Motion_StepModel)(entity->model, 0, 0);
    ENTITY_FLAG_BITS(entity)->not_targetable = TRUE;
}

/* The on_hit handler of type 81: as Npc_OnHit, and sets flag bit 29 when the player dealt the hit (flag bits
   38-39). */
static void Npc_OnHitType81(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    EntityAi_SetReaction(entity, Npc_ReactionHeavy, 0);
    entity_group_state[ENTITY_CONTROLLER(entity)->entity_group_index].member_died = 1;
    if (ENTITY_FLAG_HIT_BITS(entity)->hit_source) {
        entity->flags |= ENTITY_FLAG_20000000;
    }
}

/* The think handler of type 81: clears flag bit 29, updates what it knows of the player every tenth frame,
   walks back while flag bit 53 is set, else stands. */
static void Npc_ThinkType81(Entity *entity, s32 run_now) {
    s32 mask = ~ENTITY_FLAG_20000000;

    entity->flags &= mask;
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    Npc_ThinkType81Stand(entity, run_now);
}

/* The think function of type 81 that stands: as Npc_ThinkStand. */
static int Npc_ThinkType81Stand(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;

    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    EntityAi_StartAction(entity, entity->ai_state.move, unknown);
}
