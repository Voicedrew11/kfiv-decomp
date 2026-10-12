#include "kf_game.h"
#include "kf_map.h"
#include "kf_entity.h"
#include "kf_player.h"
#include "kf_collision.h"

extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[2];
extern MapObject D_003A1A30[][0x200];
extern PlayerState D_00413F80;
extern EventRunner D_00420D00;
extern CameraState D_004325A0;
extern s32 D_004467B8[2];
extern int D_00446770;
extern ItemNode *D_004469D4;
extern ItemNode *D_004469D8;
extern f32 D_00446A08[];

int func_0020D088(void);
void func_001D0800(void);
void func_001C1858(f32, void *, void *, s32);
float func_001D1260(float, float *, float *);
void func_0020F2B8(f32, f32, f32, void *, void *, void *);
void func_001C2128(void *, int, int, float);
void sceVu0SubVector(void *, void *, void *);
void sceVu0Normalize(float *, float *);
float sceVu0InnerProduct(float *, float *);
float sqrtf(float);
f32 func_0011E708(MapObject *, Vector4 *);
int func_001C9B00(void *);
int func_0011E1F0(int, int);
int func_001090E0(char *); // match: its u16 is halved as an int
int func_002109C8(int, int);
int func_00210C48(int);
int func_001C9AB0(ModelNode *);

/* Finds what is in front of the player: how far the view reaches before a wall or a map object (unk40), and the
   entity, map object or item near enough to act on. Then, when an event may start (GameStatus.bit1), starts the
   script of whichever was found. */
void func_001D0970(void) {
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

    func_0020D088();
    func_001D0800();
    nearest = D_00420D00.unk4C = D_00420D00.unk40 = D_00446A08[0];
    D_00420D00.unk54 = NULL;
    D_00420D00.current = NULL;
    D_00420D00.unk58 = NULL;
    func_001C1858(2000.0f, D_00420D00.unk00, D_00420D00.unk10, 7);
    if (COLLISION_RESULT->hit != 0) {
        D_00420D00.unk40 = COLLISION_RESULT->distance;
        nearest = COLLISION_RESULT->distance;
    }
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if ((entity->flags & 0xE) != 2) {
            continue;
        }
        reach = entity->radius + 100.0f + 200.0f;
        distance = func_001D1260(reach, (float *)&D_00413F80, (float *)&ENTITY_MODEL(entity)->position);
        if (distance < nearest) {
            nearest = distance;
        }
        if (D_00420D00.unk40 <= distance) {
            continue;
        }
        if (D_00420D00.unk4C <= distance) {
            continue;
        }
        func_0020F2B8(entity->radius, entity->height, reach, &ENTITY_MODEL(entity)->position, D_00420D00.unk00,
                      D_00420D00.unk10);
        if (COLLISION_RESULT->hit != 0) {
            D_00420D00.unk4C = distance;
            D_00420D00.unk54 = entity;
        }
    }
    for (map = 0; map < 2; map++) {
        if (D_0038CD20[map].unk00 == 0) {
            continue;
        }
        count = D_004467B8[map];
        object = D_003A1A30[map];
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
                sceVu0SubVector(delta, &object->model->position, &D_004325A0);
                sceVu0Normalize(delta, delta);
                if (sceVu0InnerProduct(delta, D_00420D00.unk10) < 0.866025f) {
                    continue;
                }
                sceVu0SubVector(delta, &object->model->position, &D_004325A0);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            } else {
                func_001C2128(object, (int)&D_004325A0, (int)D_00420D00.unk10, reach);
                if (COLLISION_RESULT->hit == 0) {
                    continue;
                }
                sceVu0SubVector(delta, COLLISION_RESULT, &D_004325A0);
                distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            }
            if (distance < object->unk3C + 100.0f && distance < D_00420D00.unk4C) {
                D_00420D00.unk54 = NULL;
                D_00420D00.current = object;
                D_00420D00.unk4C = distance;
            }
            if (!is_plain && distance < D_00420D00.unk40) {
                D_00420D00.unk40 = distance;
            }
        }
    }
    facing = D_00420D00.current;
    if (facing != NULL && facing->unk04->unk64 != 0.0f) {
        turn = func_0011E708(facing, &D_00413F80.position);
        limit = ((MapObject *)D_00420D00.current)->unk04->unk64;
        if (turn < -limit || limit < turn) {
            D_00420D00.current = NULL;
        }
    }
    for (node = D_004469D4; node != NULL; node = node->next) {
        if (!func_001C9B00(node)) {
            continue;
        }
        if (node->model->position.y < D_00413F80.position.y - 50.0f - 230.0f) {
            continue;
        }
        if (D_00413F80.position.y + 50.0f < node->model->position.y) {
            continue;
        }
        sceVu0SubVector(delta, &node->model->position, &D_004325A0);
        distance = delta[0] * delta[0] + delta[2] * delta[2];
        if (90000.0f < distance) {
            continue;
        }
        sceVu0Normalize(unit, delta);
        if (sceVu0InnerProduct(unit, D_00420D00.unk10) < 0.866025f) {
            continue;
        }
        distance = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
        if (D_00420D00.unk40 < distance) {
            continue;
        }
        def = node->def;
        if (def != NULL) {
            item_map = node->flags & 1;
            if (D_0038CD20[item_map].unk00 != 0) {
                object = NULL;
                switch (D_0038CD20[item_map].tag) {
                case 0xC:
                    if (def->slot_id == 0xFA) {
                        object = (MapObject *)func_0011E1F0(item_map, def->slot_id);
                    }
                    break;
                case 0x11:
                    if (def->slot_id == 0xC9) {
                        object = (MapObject *)func_0011E1F0(item_map, def->slot_id + 0x32);
                    }
                    break;
                case 0x12:
                case 0x13:
                    if (def->slot_id == 0xC9 || def->slot_id == 0xCA) {
                        object = (MapObject *)func_0011E1F0(item_map, def->slot_id + 0x32);
                    }
                    break;
                case 0x6D:
                    if (def->slot_id >= 0xFA && def->slot_id <= 0xFD) {
                        object = (MapObject *)func_0011E1F0(item_map, def->slot_id);
                    }
                    break;
                }
                if (object != NULL && (object->unk10 & 0x10000) && MODEL_CHANNEL(object->model) != NULL) {
                    if (!(object->unk10 & 0x20000)) {
                        continue;
                    }
                    frame = MODEL_CHANNEL(object->model)->param;
                    if (frame != 1 && !(func_001090E0((char *)object->model) / 2 < frame)) {
                        continue;
                    }
                }
            }
        }
        D_00420D00.unk4C = distance;
        D_00420D00.unk54 = NULL;
        D_00420D00.current = NULL;
        D_00420D00.unk58 = node;
    }
    if (D_00420D00.unk40 == D_00446A08[0]) {
        D_00420D00.unk40 = 2000.0f;
    }
    if (D_00413F80.unk1A8 == 0) {
        can_act = D_0038CC50.bit1;
        pickup = D_00420D00.unk58;
        if (pickup != NULL && can_act) {
            if (pickup->def != NULL && pickup->def->unk34 != 0) {
                if ((pickup->flags & 1) == D_00446770) {
                    func_002109C8(pickup->def->unk34, 0);
                }
                can_act = FALSE;
            } else if (D_004469D8 == NULL) {
                func_001C9AB0(D_00420D00.unk58);
                can_act = FALSE;
            }
        }
        user = D_00420D00.unk54;
        if (user != NULL) {
            if (can_act && user->spawn_info->unk4E != 0) {
                if (user->group_id == D_00446770) {
                    if (user->spawn_info->unk58 == 0 || func_00210C48(user->spawn_info->unk58) == 0) {
                        func_002109C8(((Entity *)D_00420D00.unk54)->spawn_info->unk4E, 0);
                    }
                }
                D_0038CC50.bit1 = FALSE;
            }
        } else {
            used = D_00420D00.current;
            if (used != NULL && can_act && used->unk08->unk40 != 0) {
                if ((MAP_OBJECT_BYTES(used)->unk00 & 1) == D_00446770) {
                    if (used->unk08->unk58 == 0 || func_00210C48(used->unk08->unk58) == 0) {
                        func_002109C8(((MapObject *)D_00420D00.current)->unk08->unk40, 0);
                    }
                }
                D_0038CC50.bit1 = FALSE;
            }
        }
    }
    D_00420D00.unk48 = nearest;
}
