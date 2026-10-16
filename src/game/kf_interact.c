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
extern EventRunner view_ray_status;
extern CameraState camera_state;
extern f32 identity_matrix[16];
extern MapSlot map_slots[2];
extern MapObject map_slot_objects[][0x200];
extern s32 map_object_counts[2];
extern int current_map_slot;
extern ItemNode *lying_items;
extern ItemNode *pickup_item;
extern f32 D_00446A08[];
extern float D_00446A0C[];

/* Keep last frame's view ray, then rebuild it: from the player's eyes (or the camera, in a cutscene) along the
   direction looked in. */
void Interact_UpdateViewRay(void) {
    f32 rotation[16];
    f32 pitch;
    f32 heading;

    Vector_Copy(view_ray_status.unk20, view_ray_status.unk00);
    Vector_Copy(view_ray_status.unk30, view_ray_status.unk10);
    view_ray_status.unk44 = view_ray_status.unk40;
    if (game_status.unk03 == 0) {
        Vector_Copy(view_ray_status.unk00, &player_state);
        view_ray_status.unk00[1] = view_ray_status.unk00[1] - 220.0f + player_state.unk1F0 + camera_state.unk4C;
        pitch = player_state.rotation.x - player_state.unk238;
        if (pitch > 1.012291f) {
            pitch = 1.012291f;
        } else if (pitch < -1.012291f) {
            pitch = -1.012291f;
        }
        heading = player_state.rotation.y;
    } else {
        Vector_Copy(view_ray_status.unk00, &camera_state);
        pitch = camera_state.unk10;
        heading = camera_state.unk14;
    }
    view_ray_status.unk10[2] = 1.0f;
    view_ray_status.unk10[0] = 0.0f;
    view_ray_status.unk10[1] = 0.0f;
    view_ray_status.unk10[3] = 0.0f;
    sceVu0RotMatrixX(rotation, identity_matrix, pitch);
    Vector_ApplyMatrix(view_ray_status.unk10, rotation, view_ray_status.unk10);
    sceVu0RotMatrixY(rotation, identity_matrix, heading);
    Vector_ApplyMatrix(view_ray_status.unk10, rotation, view_ray_status.unk10);
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
    nearest = view_ray_status.unk4C = view_ray_status.unk40 = D_00446A08[0];
    view_ray_status.unk54 = NULL;
    view_ray_status.current = NULL;
    view_ray_status.unk58 = NULL;
    Collision_CastRayAtMapLists(2000.0f, view_ray_status.unk00, view_ray_status.unk10, 7);
    if (COLLISION_RESULT->hit != 0) {
        view_ray_status.unk40 = COLLISION_RESULT->distance;
        nearest = COLLISION_RESULT->distance;
    }
    for (entity = active_entities_head; entity != NULL; entity = entity->next) {
        if ((entity->flags & 0xE) != 2) {
            continue;
        }
        reach = entity->radius + 100.0f + 200.0f;
        distance = Interact_GetDistanceWithin(reach, (float *)&player_state, (float *)&ENTITY_MODEL(entity)->position);
        if (distance < nearest) {
            nearest = distance;
        }
        if (view_ray_status.unk40 <= distance) {
            continue;
        }
        if (view_ray_status.unk4C <= distance) {
            continue;
        }
        Collision_RayCylinder(entity->radius, entity->height, reach, &ENTITY_MODEL(entity)->position, view_ray_status.unk00,
                      view_ray_status.unk10);
        if (COLLISION_RESULT->hit != 0) {
            view_ray_status.unk4C = distance;
            view_ray_status.unk54 = entity;
        }
    }
    for (map = 0; map < 2; map++) {
        if (map_slots[map].unk00 == 0) {
            continue;
        }
        count = map_object_counts[map];
        object = map_slot_objects[map];
        for (i = 0; i < count; i++, object++) {
            if (!(object->unk10 & 0x10000)) {
                continue;
            }
            if (!(object->unk10 & 4)) {
                continue;
            }
            reach = object->unk3C + 200.0f < 2000.0f ? 2000.0f : object->unk3C + 200.0f;
            is_plain = object->unk08->unk34 == 0xFFFF && !(object->unk04->kind == 1 || object->unk04->kind == 2 ||
                       object->unk04->kind == 6 || object->unk04->kind == 0x11);
            if (is_plain) {
                sceVu0SubVector(delta, &object->model->position, &camera_state);
                sceVu0Normalize(delta, delta);
                if (sceVu0InnerProduct(delta, view_ray_status.unk10) < 0.866025f) {
                    continue;
                }
                sceVu0SubVector(delta, &object->model->position, &camera_state);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            } else {
                Collision_HitRayObj(object, (f32 *)&camera_state, view_ray_status.unk10, reach);
                if (COLLISION_RESULT->hit == 0) {
                    continue;
                }
                sceVu0SubVector(delta, COLLISION_RESULT, &camera_state);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            }
            if (distance < object->unk3C + 100.0f && distance < view_ray_status.unk4C) {
                view_ray_status.unk54 = NULL;
                view_ray_status.current = object;
                view_ray_status.unk4C = distance;
            }
            if (!is_plain && distance < view_ray_status.unk40) {
                view_ray_status.unk40 = distance;
            }
        }
    }
    facing = view_ray_status.current;
    if (facing != NULL && facing->unk04->unk64 != 0.0f) {
        turn = MapObject_GetTurnToward(facing, &player_state.position);
        limit = ((MapObject *)view_ray_status.current)->unk04->unk64;
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
        if (sceVu0InnerProduct(unit, view_ray_status.unk10) < 0.866025f) {
            continue;
        }
        distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
        if (view_ray_status.unk40 < distance) {
            continue;
        }
        def = node->def;
        if (def != NULL) {
            item_map = node->flags & 1;
            if (map_slots[item_map].unk00 != 0) {
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
                if (object != NULL && (object->unk10 & 0x10000) && MODEL_CHANNEL(object->model) != NULL) {
                    if (!(object->unk10 & 0x20000)) {
                        continue;
                    }
                    frame = MODEL_CHANNEL(object->model)->param;
                    if (frame != 1 && !(ModelMotion_GetLastFrame((char *)object->model) / 2 < frame)) {
                        continue;
                    }
                }
            }
        }
        view_ray_status.unk4C = distance;
        view_ray_status.unk54 = NULL;
        view_ray_status.current = NULL;
        view_ray_status.unk58 = node;
    }
    if (view_ray_status.unk40 == D_00446A08[0]) {
        view_ray_status.unk40 = 2000.0f;
    }
    if (player_state.unk1A8 == 0) {
        can_act = game_status.bit1;
        pickup = view_ray_status.unk58;
        if (pickup != NULL && can_act) {
            if (pickup->def != NULL && pickup->def->unk34 != 0) {
                if ((pickup->flags & 1) == current_map_slot) {
                    Script_Start(pickup->def->unk34, 0);
                }
                can_act = FALSE;
            } else if (pickup_item == NULL) {
                MapItem_StartPickup(view_ray_status.unk58);
                can_act = FALSE;
            }
        }
        user = view_ray_status.unk54;
        if (user != NULL) {
            if (can_act && user->spawn_info->unk4E != 0) {
                if (user->group_id == current_map_slot) {
                    if (user->spawn_info->unk58 == 0 || Script_IsRunning(user->spawn_info->unk58) == 0) {
                        Script_Start(((Entity *)view_ray_status.unk54)->spawn_info->unk4E, 0);
                    }
                }
                game_status.bit1 = FALSE;
            }
        } else {
            used = view_ray_status.current;
            if (used != NULL && can_act && used->unk08->unk40 != 0) {
                if ((MAP_OBJECT_BYTES(used)->unk00 & 1) == current_map_slot) {
                    if (used->unk08->unk58 == 0 || Script_IsRunning(used->unk08->unk58) == 0) {
                        Script_Start(((MapObject *)view_ray_status.current)->unk08->unk40, 0);
                    }
                }
                game_status.bit1 = FALSE;
            }
        }
    }
    view_ray_status.unk48 = nearest;
}

/* Moves the event runner's position and its copy from the frame before by offset (the other map slot became
   the current one). */
void Interact_OffsetViewRay(f32 *offset) {
    Vector_Add(view_ray_status.unk00, view_ray_status.unk00, offset);
    Vector_Add(view_ray_status.unk20, view_ray_status.unk20, offset);
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
