/* Snake: the behaviour of entity type 21 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Snake. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "map_object/map_object.h"
#include "map/map.h"
#include "effect/effect.h"
#include "core/game.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_collision.h"
#include "math/vu0.h"
#include "save/save_state.h"
#include "script/script.h"
#include "entity/entity_ai.h"
#include "model/motion.h"
#include "entity/entity_math.h"


static void Snake_DormantHoldAnim2(Entity *entity);
static void Snake_DormantHoldAnim8ThenMove(Entity *entity);
static void Snake_Dormant(Entity *entity);
static void Snake_Think(Entity *entity, s32 run_now);
static void Snake_Death(Entity *entity);

/* Set-up of a type 21 entity: clears its work area and installs the type's handlers. */
void Snake_Init(Entity *entity) {
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
    ai_state->think = Snake_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
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
        if (entity->spawn_info->spawn_mode == 0) {
            ai_state->dormant = Snake_DormantHoldAnim2;
        } else if (entity->spawn_info->spawn_mode == 1) {
            ai_state->dormant = Snake_DormantHoldAnim8ThenMove;
        } else {
            ai_state->dormant = Snake_Dormant;
        }
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

/* The dormant action of a spawn in mode 0 (EntitySpawnInfo.spawn_mode is 0; busy bit 0x20): holds
   animation 2 at its start until the player is within EntitySpawnInfo.wake_distance or the entity is hit,
   then plays it to its end (animation 17 instead when the map object EntitySpawnInfo.variant names
   has bit 16 clear or is at step 15), goes to state 2 and thinks. */
static void Snake_DormantHoldAnim2(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 2, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_map_object_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case 1:
        ENTITY_FLAG_BITS(entity)->animates = FALSE;
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_strength != 0) {
            map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
            if (map_object != NULL) {
                if (!(map_object->state_flags & MAP_OBJECT_IN_USE) || MAP_OBJECT_BITS(map_object)->step == 0xF) {
                    Motion_Start(entity, 0x11, 0, 0, 1);
                }
            }
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_map_object_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
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

/* The dormant action of a spawn in mode 1 (busy bit 0x20): holds animation 8 until the player is
   within EntitySpawnInfo.wake_distance, plays it past frame 30, then moves the entity along its heading by
   EntityParams.near_distance a frame (unk438 in y) until flag bit 44 is set or EntityCollision_DistanceToPropFloor reads below
   100, plays the rest, goes to state 2 and thinks. While it waits it removes the entity once
   EntitySpawnInfo.variant is set and map_script_flags[unk43 + unk42 * 64] is not 0. */
static void Snake_DormantHoldAnim8ThenMove(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 distance;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_map_object_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->spawn_info->variant != 0 &&
            map_script_flags[entity->spawn_info->second_map_object_id + entity->spawn_info->map_object_id * 64] != 0) {
            ENTITY_FLAG_BITS(entity)->live = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 3;
            break;
        }
        ENTITY_FLAG_BITS(entity)->animates = FALSE;
        if (entity->distance_to_player < entity->spawn_info->wake_distance) {
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame > 30) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            entity->action_step++;
        }
        break;
    case 3:
        distance = EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position);
        if (ENTITY_FLAG_BITS(entity)->on_floor || distance < 100.0f) {
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            entity->action_step++;
            break;
        }
        EntityMath_FacingVector((Vector4 *)direction, entity->model->rotation[1]);
        Vector_Scale(direction, direction, entity->params->near_distance);
        direction[1] += entity->params->far_distance;
        entity->move_delta_x += direction[0];
        entity->move_delta_y += direction[1];
        entity->move_delta_z += direction[2];
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_map_object_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
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

/* The dormant action of the other modes (busy bit 0x20): holds animation 3 until the player is
   within EntitySpawnInfo.wake_distance, then plays it until EntityMath_DistanceXZ between the entity and its spawn
   point reaches EntityParams.type_param_0, goes to state 2 and thinks. */
static void Snake_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 3, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case 1:
        ENTITY_FLAG_BITS(entity)->animates = FALSE;
        if (entity->distance_to_player < (f32)entity->spawn_info->wake_distance) {
            ENTITY_FLAG_BITS(entity)->animates = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        if (EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) >= (f32)entity->params->type_param_0) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
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

/* The think handler: as EntityAi_Think, but first, for a spawn in mode 1 whose
   EntitySpawnInfo.variant is 1, it starts Snake_Death once map_script_flags[unk43 + unk42 * 64] is
   not 0. */
static void Snake_Think(Entity *entity, s32 run_now) {
    s32 index;

    if (entity->spawn_info->spawn_mode == 1 && entity->spawn_info->variant == 1) {
        index = entity->spawn_info->second_map_object_id;
        index += entity->spawn_info->map_object_id * 64;
        if (map_script_flags[index] != 0) {
            EntityAi_StartAction(entity, Snake_Death, 1);
            return;
        }
    }
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) != ENTITY_PARAMS_MOVEMENT_FIXED) {
            EntityAi_CheckFarFromSpawn(entity);
        }
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case 2:
                EntityAi_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* A death action (busy bit 0x40) that Snake_Think starts; the death slot holds
   EntityAi_Death. As EntityAi_Death, but when the motion has ended it calls Game_CountSpawnDeath, frees
   the entity's effects and starts script EntitySpawnInfo.death_script_id where that one calls EntityEvent_OnDeath.
   */
static void Snake_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            entity->action_timer = 20;
            Game_CountSpawnDeath(map_slots[entity->group_id].tag, entity->spawn_info - map_spawn_infos[entity->group_id]);
            Effect_FreeForOwner(entity);
            if (entity->spawn_info->death_script_id != 0 && entity->group_id == current_map_slot) {
                Script_Start(entity->spawn_info->death_script_id, 0);
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}
