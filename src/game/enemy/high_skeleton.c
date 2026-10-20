/* HighSkeleton: the behaviour of entity type 62 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: High Skeleton Small, High Skeleton
   Large. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to
   the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
#include "entity/entity.h"
#include "entity/entity_world.h"
#include "effect/effect.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "math/vu0.h"
#include "entity/entity_ai.h"
#include "model/motion.h"

/* EntitySpawnInfo.spawn_mode: the default spawn, which keeps the think handler with states */
#define HIGH_SKELETON_SPAWN_MODE_DEFAULT 0

// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void HighSkeleton_Think(Entity *entity, s32 run_now);
static void HighSkeleton_Dormant(Entity *entity);
static void HighSkeleton_DormantScaleUp(Entity *entity);
static void HighSkeleton_Death(Entity *entity);

/* Set-up of a type 62 entity: clears its work area and installs the type's handlers. */
void HighSkeleton_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = HighSkeleton_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    if (entity->spawn_info->spawn_mode != HIGH_SKELETON_SPAWN_MODE_DEFAULT) {
        ai_state->dormant = HighSkeleton_DormantScaleUp;
    } else {
        ai_state->dormant = HighSkeleton_Dormant;
    }
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

/* The think handler: as EntityAi_Think. Before that, with EntitySpawnInfo.spawn_mode and variant set, it
   looks every Entity.spawn_index frames at the entity with spawn id `variant` and starts
   HighSkeleton_Death once that one has flag bits 2-3 at 2 or more or flag bit 1 clear. */
static void HighSkeleton_Think(Entity *entity, s32 run_now) {
    Entity *other;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->spawn_info->spawn_mode != HIGH_SKELETON_SPAWN_MODE_DEFAULT && entity->spawn_info->variant != 0) {
        other = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (other != NULL && frame_counter % entity->spawn_index == 0) {
            if (ENTITY_FLAG_BITS(other)->dying >= 2 || !ENTITY_FLAG_BITS(other)->live) {
                EntityAi_StartAction(entity, HighSkeleton_Death, 1);
                return;
            }
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
                EntityAi_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, and flag bits 20 and 25 are set with the
   others while it lasts. */
static void HighSkeleton_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_map_object_collision = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
            if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            } else {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
            }
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_map_object_collision = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
                ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
            }
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

/* The action the set-up function puts in the dormant slot for a spawn whose EntitySpawnInfo.spawn_mode
   is set; its busy bit is 0x1000, not 0x20. It holds animation 0 for EntityParams.near_distance frames
   with flag bits 18 and 21 set, calls EntityEvent_StartEffectAtPoint with 0x22F once, and meanwhile scales the model
   in x and z and the radius from 0 up to their full size; then goes to state 2 and thinks. */
static void HighSkeleton_DormantScaleUp(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 duration;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        entity->action_timer = (u16)entity->params->near_distance;
        ai_state->part_angle_x = entity->model->scale[0];
        ai_state->part_angle_y = entity->model->scale[1];
        ai_state->part_angle_z = entity->model->scale[2];
        ai_state->pitch_speed = entity->radius;
        entity->model->scale[0] = 0.0f;
        entity->model->scale[2] = 0.0f;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case 1:
        EntityEvent_StartEffectAtPoint((char **)entity, 0x22F, 0xFF, (int)entity->model->rotation);
        entity->action_step++;
        break;
    case 2:
        duration = entity->params->near_distance;
        entity->model->scale[0] = (f32)(duration - entity->action_timer) / (f32)duration;
        entity->model->scale[2] = (f32)(duration - entity->action_timer) / (f32)duration;
        entity->radius = ai_state->pitch_speed * ((f32)(duration - entity->action_timer) / (f32)duration);
        if (entity->action_timer == 0) {
            entity->model->scale[0] = ai_state->part_angle_x;
            entity->model->scale[1] = ai_state->part_angle_y;
            entity->model->scale[2] = ai_state->part_angle_z;
            entity->radius = ai_state->pitch_speed;
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
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

/* A death action (busy bit 0x40) that HighSkeleton_Think starts; the death slot has EntityAi_Death.
   As EntityAi_Death, but it frees the entity's effects (Effect_FreeForOwner) when
   EntityParams.respawn_effect_id is neither 0 nor 0xFFFF and does not call EntityEvent_OnDeath. */
static void HighSkeleton_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        if (entity->params->respawn_effect_id != 0 && entity->params->respawn_effect_id != 0xFFFF) {
            Effect_FreeForOwner(entity);
        }
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->action_timer = 20;
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
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
