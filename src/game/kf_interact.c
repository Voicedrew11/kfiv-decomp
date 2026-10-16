#include "kf_game.h"
#include "kf_player.h"
#include "kf_map.h"
#include "kf_entity.h"
#include "kf_map_object.h"
#include "kf_collision.h"
#include "kf_vu0.h"

void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
int Timer_GetTicks(void);
void Interact_UpdateViewRay(void);
void Collision_CastRayAtMapLists(f32, void *, void *, s32);
float Interact_GetDistanceWithin(float, float *, float *);
void Collision_RayCylinder(f32, f32, f32, void *, void *, void *);
void Collision_HitRayObj(MapObject *, f32 *, f32 *, f32);
void sceVu0SubVector(void *, void *, void *);
void sceVu0Normalize(float *, float *);
float sceVu0InnerProduct(float *, float *);
float sqrtf(float);
int MapItem_IsPickable(void *);
int ModelMotion_GetLastFrame(char *); // match: its u16 is halved as an int
int Script_Start(int, int);
int Script_IsRunning(int);
int MapItem_StartPickup(ItemNode *);
extern GameStatus game_status;
extern PlayerState player_state;
extern EventRunner D_00420D00;
extern CameraState D_004325A0;
extern f32 D_00431AF0[16];
extern MapSlot map_slots[2];
extern MapObject map_slot_objects[][0x200];
extern s32 D_004467B8[2];
extern int D_00446770;
extern ItemNode *D_004469D4;
extern ItemNode *D_004469D8;
extern f32 D_00446A08[];
extern float D_00446A0C[];

/* Keep last frame's view ray, then rebuild it: from the player's eyes (or the camera, in a cutscene) along the
   direction looked in. */
void Interact_UpdateViewRay(void) {
    f32 rotation[16];
    f32 pitch;
    f32 heading;

    Vector_Copy(D_00420D00.prev_view_origin, D_00420D00.view_origin);
    Vector_Copy(D_00420D00.prev_view_direction, D_00420D00.view_direction);
    D_00420D00.prev_view_distance = D_00420D00.view_distance;
    if (game_status.camera_mode == 0) {
        Vector_Copy(D_00420D00.view_origin, &player_state);
        D_00420D00.view_origin[1] = D_00420D00.view_origin[1] - 220.0f + player_state.landing_dip_offset + D_004325A0.height_offset;
        pitch = player_state.rotation.x - player_state.view_tilt_pitch;
        if (pitch > 1.012291f) {
            pitch = 1.012291f;
        } else if (pitch < -1.012291f) {
            pitch = -1.012291f;
        }
        heading = player_state.rotation.y;
    } else {
        Vector_Copy(D_00420D00.view_origin, &D_004325A0);
        pitch = D_004325A0.pitch;
        heading = D_004325A0.yaw;
    }
    D_00420D00.view_direction[2] = 1.0f;
    D_00420D00.view_direction[0] = 0.0f;
    D_00420D00.view_direction[1] = 0.0f;
    D_00420D00.view_direction[3] = 0.0f;
    sceVu0RotMatrixX(rotation, D_00431AF0, pitch);
    Vector_ApplyMatrix(D_00420D00.view_direction, rotation, D_00420D00.view_direction);
    sceVu0RotMatrixY(rotation, D_00431AF0, heading);
    Vector_ApplyMatrix(D_00420D00.view_direction, rotation, D_00420D00.view_direction);
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
    nearest = D_00420D00.target_distance = D_00420D00.view_distance = D_00446A08[0];
    D_00420D00.facing_entity = NULL;
    D_00420D00.current = NULL;
    D_00420D00.facing_item = NULL;
    Collision_CastRayAtMapLists(2000.0f, D_00420D00.view_origin, D_00420D00.view_direction, 7);
    if (COLLISION_RESULT->hit != 0) {
        D_00420D00.view_distance = COLLISION_RESULT->distance;
        nearest = COLLISION_RESULT->distance;
    }
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if ((entity->flags & 0xE) != 2) {
            continue;
        }
        reach = entity->radius + 100.0f + 200.0f;
        distance = Interact_GetDistanceWithin(reach, (float *)&player_state, (float *)&ENTITY_MODEL(entity)->position);
        if (distance < nearest) {
            nearest = distance;
        }
        if (D_00420D00.view_distance <= distance) {
            continue;
        }
        if (D_00420D00.target_distance <= distance) {
            continue;
        }
        Collision_RayCylinder(entity->radius, entity->height, reach, &ENTITY_MODEL(entity)->position, D_00420D00.view_origin,
                      D_00420D00.view_direction);
        if (COLLISION_RESULT->hit != 0) {
            D_00420D00.target_distance = distance;
            D_00420D00.facing_entity = entity;
        }
    }
    for (map = 0; map < 2; map++) {
        if (map_slots[map].loaded == 0) {
            continue;
        }
        count = D_004467B8[map];
        object = map_slot_objects[map];
        for (i = 0; i < count; i++, object++) {
            if (!(object->state_flags & 0x10000)) {
                continue;
            }
            if (!(object->state_flags & 4)) {
                continue;
            }
            reach = object->interact_radius + 200.0f < 2000.0f ? 2000.0f : object->interact_radius + 200.0f;
            is_plain = object->source->collision_mesh_index == 0xFFFF && !(object->definition->kind == 1 || object->definition->kind == 2 ||
                       object->definition->kind == 6 || object->definition->kind == 0x11);
            if (is_plain) {
                sceVu0SubVector(delta, &object->model->position, &D_004325A0);
                sceVu0Normalize(delta, delta);
                if (sceVu0InnerProduct(delta, D_00420D00.view_direction) < 0.866025f) {
                    continue;
                }
                sceVu0SubVector(delta, &object->model->position, &D_004325A0);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            } else {
                Collision_HitRayObj(object, (f32 *)&D_004325A0, D_00420D00.view_direction, reach);
                if (COLLISION_RESULT->hit == 0) {
                    continue;
                }
                sceVu0SubVector(delta, COLLISION_RESULT, &D_004325A0);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            }
            if (distance < object->interact_radius + 100.0f && distance < D_00420D00.target_distance) {
                D_00420D00.facing_entity = NULL;
                D_00420D00.current = object;
                D_00420D00.target_distance = distance;
            }
            if (!is_plain && distance < D_00420D00.view_distance) {
                D_00420D00.view_distance = distance;
            }
        }
    }
    facing = D_00420D00.current;
    if (facing != NULL && facing->definition->facing_angle != 0.0f) {
        turn = MapObject_GetTurnToward(facing, &player_state.position);
        limit = ((MapObject *)D_00420D00.current)->definition->facing_angle;
        if (turn < -limit || limit < turn) {
            D_00420D00.current = NULL;
        }
    }
    for (node = D_004469D4; node != NULL; node = node->next) {
        if (!MapItem_IsPickable(node)) {
            continue;
        }
        if (node->model->position.y < player_state.position.y - 50.0f - 230.0f) {
            continue;
        }
        if (player_state.position.y + 50.0f < node->model->position.y) {
            continue;
        }
        sceVu0SubVector(delta, &node->model->position, &D_004325A0);
        distance = delta[0] * delta[0] + delta[2] * delta[2];
        if (90000.0f < distance) {
            continue;
        }
        sceVu0Normalize(unit, delta);
        if (sceVu0InnerProduct(unit, D_00420D00.view_direction) < 0.866025f) {
            continue;
        }
        distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
        if (D_00420D00.view_distance < distance) {
            continue;
        }
        def = node->def;
        if (def != NULL) {
            item_map = node->flags & 1;
            if (map_slots[item_map].loaded != 0) {
                object = NULL;
                switch (map_slots[item_map].tag) {
                case 0xC:
                    if (def->slot_id == 0xFA) {
                        object = MapObject_GetById(item_map, def->slot_id);
                    }
                    break;
                case 0x11:
                    if (def->slot_id == 0xC9) {
                        object = MapObject_GetById(item_map, def->slot_id + 0x32);
                    }
                    break;
                case 0x12:
                case 0x13:
                    if (def->slot_id == 0xC9 || def->slot_id == 0xCA) {
                        object = MapObject_GetById(item_map, def->slot_id + 0x32);
                    }
                    break;
                case 0x6D:
                    if (def->slot_id >= 0xFA && def->slot_id <= 0xFD) {
                        object = MapObject_GetById(item_map, def->slot_id);
                    }
                    break;
                }
                if (object != NULL && (object->state_flags & 0x10000) && MODEL_CHANNEL(object->model) != NULL) {
                    if (!(object->state_flags & 0x20000)) {
                        continue;
                    }
                    frame = MODEL_CHANNEL(object->model)->param;
                    if (frame != 1 && !(ModelMotion_GetLastFrame((char *)object->model) / 2 < frame)) {
                        continue;
                    }
                }
            }
        }
        D_00420D00.target_distance = distance;
        D_00420D00.facing_entity = NULL;
        D_00420D00.current = NULL;
        D_00420D00.facing_item = node;
    }
    if (D_00420D00.view_distance == D_00446A08[0]) {
        D_00420D00.view_distance = 2000.0f;
    }
    if (player_state.death_timer == 0) {
        can_act = game_status.bit1;
        pickup = D_00420D00.facing_item;
        if (pickup != NULL && can_act) {
            if (pickup->def != NULL && pickup->def->unk34 != 0) {
                if ((pickup->flags & 1) == D_00446770) {
                    Script_Start(pickup->def->unk34, 0);
                }
                can_act = FALSE;
            } else if (D_004469D8 == NULL) {
                MapItem_StartPickup(D_00420D00.facing_item);
                can_act = FALSE;
            }
        }
        user = D_00420D00.facing_entity;
        if (user != NULL) {
            if (can_act && user->spawn_info->talk_script_id != 0) {
                if (user->group_id == D_00446770) {
                    if (user->spawn_info->item_script_id == 0 || Script_IsRunning(user->spawn_info->item_script_id) == 0) {
                        Script_Start(((Entity *)D_00420D00.facing_entity)->spawn_info->talk_script_id, 0);
                    }
                }
                game_status.bit1 = FALSE;
            }
        } else {
            used = D_00420D00.current;
            if (used != NULL && can_act && used->source->use_script != 0) {
                if ((MAP_OBJECT_BYTES(used)->unk00 & 1) == D_00446770) {
                    if (used->source->item_use_script == 0 || Script_IsRunning(used->source->item_use_script) == 0) {
                        Script_Start(((MapObject *)D_00420D00.current)->source->use_script, 0);
                    }
                }
                game_status.bit1 = FALSE;
            }
        }
    }
    D_00420D00.nearest_entity_distance = nearest;
}

/* Moves the event runner's position and its copy from the frame before by offset (the other map slot became
   the current one). */
void Interact_OffsetViewRay(f32 *offset) {
    Vector_Add(D_00420D00.view_origin, D_00420D00.view_origin, offset);
    Vector_Add(D_00420D00.prev_view_origin, D_00420D00.prev_view_origin, offset);
}

/* Distance from pos to target if within radius on every axis and overall, else D_00446A0C. */
float Interact_GetDistanceWithin(float radius, float *pos, float *target) {
    float diff[4];
    float dist;

    if (pos[0] < target[0] - radius || target[0] + radius < pos[0]
        || pos[1] < target[1] - radius || target[1] + radius < pos[1]
        || pos[2] < target[2] - radius || target[2] + radius < pos[2]) {
        return D_00446A0C[0];
    }
    Vector_Subtract(diff, pos, target);
    dist = sqrtf(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    if (dist <= radius) {
        return dist;
    }
    return D_00446A0C[0];
}
