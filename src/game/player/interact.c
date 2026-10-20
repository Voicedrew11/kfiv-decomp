#include "core/game.h"
#include "model/motion.h"
#include "math/camera.h"
#include "player/player.h"
#include "map/map.h"
#include "entity/entity.h"
#include "map_object/map_object.h"
#include "collision/collision.h"
#include "math/vu0.h"
#include "core/timer.h"
#include "enemy/enemy.h"
#include "item/item.h"
#include <libvu0.h>
#include <math.h>
#include "script/script.h"

/* map tags (the map file number) the interact checks name */
#define MAP_TAG_ANCIENT_CITY_EXTERIOR       0xC
#define MAP_TAG_FORGE_017                   0x11
#define MAP_TAG_FORGE_018                   0x12
#define MAP_TAG_FORGE_019                   0x13
#define MAP_TAG_ANCIENT_CITY_LEVEL2_WEST    0x6D

#define INTERACT_CONE_COS 0.866025f /* cos 30 degrees: objects further off the view direction are skipped */


/* Keep last frame's view ray, then rebuild it: from the player's eyes (or the camera, in a cutscene) along the
   direction looked in. */
void Interact_UpdateViewRay(void) {
    f32 rotation[16];
    f32 pitch;
    f32 heading;

    Vector_Copy(view_ray_status.prev_view_origin, view_ray_status.view_origin);
    Vector_Copy(view_ray_status.prev_view_direction, view_ray_status.view_direction);
    view_ray_status.prev_view_distance = view_ray_status.view_distance;
    if (game_status.camera_mode == CAMERA_MODE_FOLLOW_PLAYER) {
        Vector_Copy(view_ray_status.view_origin, &player_state);
        view_ray_status.view_origin[1] = view_ray_status.view_origin[1] - 220.0f + player_state.landing_dip_offset + camera_state.height_offset;
        pitch = player_state.rotation.x - player_state.view_tilt_pitch;
        if (pitch > DEG_TO_RAD(58)) {
            pitch = DEG_TO_RAD(58);
        } else if (pitch < -DEG_TO_RAD(58)) {
            pitch = -DEG_TO_RAD(58);
        }
        heading = player_state.rotation.y;
    } else {
        Vector_Copy(view_ray_status.view_origin, &camera_state);
        pitch = camera_state.pitch;
        heading = camera_state.yaw;
    }
    view_ray_status.view_direction[2] = 1.0f;
    view_ray_status.view_direction[0] = 0.0f;
    view_ray_status.view_direction[1] = 0.0f;
    view_ray_status.view_direction[3] = 0.0f;
    sceVu0RotMatrixX(rotation, identity_matrix, pitch);
    Vector_ApplyMatrix(view_ray_status.view_direction, rotation, view_ray_status.view_direction);
    sceVu0RotMatrixY(rotation, identity_matrix, heading);
    Vector_ApplyMatrix(view_ray_status.view_direction, rotation, view_ray_status.view_direction);
}

/* Finds what is in front of the player: how far the view reaches before a wall or a map object (unk40), and the
   entity, map object or item near enough to act on. Then, when an event may start (GameStatus.bit1), starts the
   script of whichever was found. */
void Interact_FindTarget(void) {
    f32 delta[4];
    f32 unit[4];
    f32 nearest;
    f32 reach;
    f32 distance;
    Entity *entity;
    MapObject *object;
    MapObject *facing;
    MapObject *used;
    ItemNode *pickup;
    Entity *user;
    s32 item_map;
    ItemNode *node;
    MapItemDef *def;
    s32 map;
    s32 count;
    s32 i;
    s32 is_plain;
    s32 frame;
    s32 can_act;
    f32 limit;
    f32 turn;

    Timer_GetTicks();
    Interact_UpdateViewRay();
    nearest = view_ray_status.target_distance = view_ray_status.view_distance = view_ray_no_hit_distance[0];
    view_ray_status.facing_entity = NULL;
    view_ray_status.current = NULL;
    view_ray_status.facing_item = NULL;
    Collision_CastRayAtMapLists(2000.0f, view_ray_status.view_origin, view_ray_status.view_direction, 7);
    if (COLLISION_RESULT->hit != 0) {
        view_ray_status.view_distance = COLLISION_RESULT->distance;
        nearest = COLLISION_RESULT->distance;
    }
    for (entity = active_entities_head; entity != NULL; entity = entity->next) {
        if ((entity->flags & (ENTITY_FLAG_LIVE | ENTITY_FLAG_DYING_MASK)) != 2) {
            continue;
        }
        reach = entity->radius + 100.0f + 200.0f;
        distance = Interact_GetDistanceWithin(reach, &player_state.position.x, &ENTITY_MODEL(entity)->position.x);
        if (distance < nearest) {
            nearest = distance;
        }
        if (view_ray_status.view_distance <= distance) {
            continue;
        }
        if (view_ray_status.target_distance <= distance) {
            continue;
        }
        Collision_RayCylinder(entity->radius, entity->height, reach, &ENTITY_MODEL(entity)->position.x, view_ray_status.view_origin,
                          view_ray_status.view_direction);
        if (COLLISION_RESULT->hit != 0) {
            view_ray_status.target_distance = distance;
            view_ray_status.facing_entity = entity;
        }
    }
    for (map = 0; map < 2; map++) {
        if (map_slots[map].loaded == 0) {
            continue;
        }
        count = map_object_counts[map];
        object = map_slot_objects[map];
        for (i = 0; i < count; i++, object++) {
            if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
                continue;
            }
            if (!(object->state_flags & MAP_OBJECT_IN_VIEW_PREVIOUS)) {
                continue;
            }
            reach = object->interact_radius + 200.0f < 2000.0f ? 2000.0f : object->interact_radius + 200.0f;
            is_plain = object->source->collision_mesh_index == 0xFFFF && !(object->definition->kind == MAP_OBJECT_KIND_DOOR || object->definition->kind == MAP_OBJECT_KIND_DOUBLE_DOOR ||
                       object->definition->kind == MAP_OBJECT_KIND_MOVING_DOOR || object->definition->kind == MAP_OBJECT_KIND_SIGNAL_DOOR);
            if (is_plain) {
                sceVu0SubVector(delta, &object->model->position, &camera_state);
                sceVu0Normalize(delta, delta);
                if (sceVu0InnerProduct(delta, view_ray_status.view_direction) < INTERACT_CONE_COS) {
                    continue;
                }
                sceVu0SubVector(delta, &object->model->position, &camera_state);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            } else {
                Collision_HitRayObj(object, (f32 *)&camera_state, view_ray_status.view_direction, reach);
                if (COLLISION_RESULT->hit == 0) {
                    continue;
                }
                sceVu0SubVector(delta, COLLISION_RESULT, &camera_state);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            }
            if (distance < object->interact_radius + 100.0f && distance < view_ray_status.target_distance) {
                view_ray_status.facing_entity = NULL;
                view_ray_status.current = object;
                view_ray_status.target_distance = distance;
            }
            if (!is_plain && distance < view_ray_status.view_distance) {
                view_ray_status.view_distance = distance;
            }
        }
    }
    facing = view_ray_status.current;
    if (facing != NULL && facing->definition->facing_angle != 0.0f) {
        turn = MapObject_GetTurnToward(facing, &player_state.position);
        limit = ((MapObject *)view_ray_status.current)->definition->facing_angle;
        if (turn < -limit || limit < turn) {
            view_ray_status.current = NULL;
        }
    }
    for (node = lying_items; node != NULL; node = node->next) {
        if (!MapItem_IsPickable(node)) {
            continue;
        }
        if (node->model->position.y < player_state.position.y - 50.0f - 230.0f) {
            continue;
        }
        if (player_state.position.y + 50.0f < node->model->position.y) {
            continue;
        }
        sceVu0SubVector(delta, &node->model->position, &camera_state);
        distance = delta[0] * delta[0] + delta[2] * delta[2];
        if (90000.0f < distance) {
            continue;
        }
        sceVu0Normalize(unit, delta);
        if (sceVu0InnerProduct(unit, view_ray_status.view_direction) < INTERACT_CONE_COS) {
            continue;
        }
        distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
        if (view_ray_status.view_distance < distance) {
            continue;
        }
        def = node->def;
        if (def != NULL) {
            item_map = node->flags & 1;
            if (map_slots[item_map].loaded != 0) {
                object = NULL;
                switch (map_slots[item_map].tag) {
                case MAP_TAG_ANCIENT_CITY_EXTERIOR:
                    if (def->slot_id == 0xFA) {
                        object = MapObject_GetById(item_map, def->slot_id);
                    }
                    break;
                case MAP_TAG_FORGE_017:
                    if (def->slot_id == 0xC9) {
                        object = MapObject_GetById(item_map, def->slot_id + 0x32);
                    }
                    break;
                case MAP_TAG_FORGE_018:
                case MAP_TAG_FORGE_019:
                    if (def->slot_id == 0xC9 || def->slot_id == 0xCA) {
                        object = MapObject_GetById(item_map, def->slot_id + 0x32);
                    }
                    break;
                case MAP_TAG_ANCIENT_CITY_LEVEL2_WEST:
                    if (def->slot_id >= 0xFA && def->slot_id <= 0xFD) {
                        object = MapObject_GetById(item_map, def->slot_id);
                    }
                    break;
                }
                if (object != NULL && (object->state_flags & MAP_OBJECT_IN_USE) && MODEL_CHANNEL(object->model) != NULL) {
                    if (!(object->state_flags & MAP_OBJECT_ON)) {
                        continue;
                    }
                    frame = MODEL_CHANNEL(object->model)->frame;
                    if (frame != 1 && !(((int (*)(char *))ModelMotion_GetLastFrame)((char *)object->model) / 2 < frame)) { // match: its u16 is halved as an int
                        continue;
                    }
                }
            }
        }
        view_ray_status.target_distance = distance;
        view_ray_status.facing_entity = NULL;
        view_ray_status.current = NULL;
        view_ray_status.facing_item = node;
    }
    if (view_ray_status.view_distance == view_ray_no_hit_distance[0]) {
        view_ray_status.view_distance = 2000.0f;
    }
    if (player_state.death_timer == 0) {
        can_act = game_status.using_faced_object;
        pickup = view_ray_status.facing_item;
        if (pickup != NULL && can_act) {
            if (pickup->def != NULL && pickup->def->pickup_script != 0) {
                if ((pickup->flags & 1) == current_map_slot) {
                    Script_Start(pickup->def->pickup_script, 0);
                }
                can_act = FALSE;
            } else if (pickup_item == NULL) {
                MapItem_StartPickup(view_ray_status.facing_item);
                can_act = FALSE;
            }
        }
        user = view_ray_status.facing_entity;
        if (user != NULL) {
            if (can_act && user->spawn_info->talk_script_id != 0) {
                if (user->group_id == current_map_slot) {
                    if (user->spawn_info->item_script_id == 0 || Script_IsRunning(user->spawn_info->item_script_id) == 0) {
                        Script_Start(((Entity *)view_ray_status.facing_entity)->spawn_info->talk_script_id, 0);
                    }
                }
                game_status.using_faced_object = FALSE;
            }
        } else {
            used = view_ray_status.current;
            if (used != NULL && can_act && used->source->use_script != 0) {
                if ((MAP_OBJECT_BYTES(used)->map_slot & 1) == current_map_slot) {
                    if (used->source->item_use_script == 0 || Script_IsRunning(used->source->item_use_script) == 0) {
                        Script_Start(((MapObject *)view_ray_status.current)->source->use_script, 0);
                    }
                }
                game_status.using_faced_object = FALSE;
            }
        }
    }
    view_ray_status.nearest_entity_distance = nearest;
}

/* Moves the event runner's position and its copy from the frame before by offset (the other map slot became
   the current one). */
void Interact_OffsetViewRay(f32 *offset) {
    Vector_Add(view_ray_status.view_origin, view_ray_status.view_origin, offset);
    Vector_Add(view_ray_status.prev_view_origin, view_ray_status.prev_view_origin, offset);
}

/* Distance from pos to target if within radius on every axis and overall, else interact_out_of_range_distance. */
float Interact_GetDistanceWithin(float radius, float *pos, float *target) {
    float diff[4];
    float dist;

    if (pos[0] < target[0] - radius || target[0] + radius < pos[0]
        || pos[1] < target[1] - radius || target[1] + radius < pos[1]
        || pos[2] < target[2] - radius || target[2] + radius < pos[2]) {
        return interact_out_of_range_distance[0];
    }
    Vector_Subtract(diff, pos, target);
    dist = sqrtf(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    if (dist <= radius) {
        return dist;
    }
    return interact_out_of_range_distance[0];
}
