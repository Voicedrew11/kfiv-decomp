#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_map.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern Entity *D_004467E0;
extern MapSlot D_0038CD20[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[][0x200];
extern int (*D_00402D88[3])(ModelInstance *, void *, f32);

void func_001B29C8(Unk1B1470Request *, Unk1B1470Extra *);
void func_001B2F70(Unk1B1470Request *, Unk1B1470Extra *, Entity *, f32);
void func_001B3AC0(Unk1B1470Request *, Unk1B1470Extra *, MapObject *);
int func_001B3E78(char *, u8 *);
void func_001C21C0(MapObject *, f32 *, f32);
void func_0020DE48(void *, void *, float, float);
void func_0020F820(void *, void *, f32, f32, f32);

#define IS_PLAYER(owner) ((owner) == (Entity *)&D_00413F80)
#define LARGER(first, second) ((first) < (second) ? (second) : (first))

/* Runs a sphere-shaped attack request against everything it may hit, in this order: the
   map objects (all of them, the first hit is kept aside), the player, the entities and
   the map's own collision meshes. Returns what was hit: 1 the player, 2 an entity, 4 a
   map object, 8, 0x10 or 0x20 the map (by the kind of surface), 0 nothing. */
s32 func_001B1510(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    CollisionResultCopy object_hit;
    s32 object_result = 0;
    Entity *entity;
    EntityBodyPoint *body;
    MapObject *object;
    MapRecordBounds *record;
    void *point;
    f32 scale;
    f32 reach;
    s32 index; /* a map, or one of an entity's bodies */
    s32 count;
    s32 item; /* a map object or a map record */
    s32 kind;
    s32 result;

    if (request->unk4D & 4) {
        for (index = 0; index < 2; index++) {
            if (D_0038CD20[index].unk00 == 0) {
                continue;
            }
            count = D_004467B8[index];
            object = D_003A1A30[index];
            for (item = 0; item < count; item++, object++) {
                if (!(object->unk10 & 0x10000)) {
                    continue;
                }
                if ((request->unk4D & 0x80) && request->owner == (Entity *)object) {
                    continue;
                }
                func_001C21C0(object, request->position, request->unk30);
                if (COLLISION_RESULT->hit != 0) {
                    if (object_result == 0) {
                        object_result = 4;
                        // match: kept aside a doubleword at a time, each through *& (retail forms the address anew for every load)
                        object_hit.doublewords[0] = *&COLLISION_RESULT_COPY->doublewords[0];
                        object_hit.doublewords[1] = *&COLLISION_RESULT_COPY->doublewords[1];
                        object_hit.doublewords[2] = *&COLLISION_RESULT_COPY->doublewords[2];
                        object_hit.doublewords[3] = *&COLLISION_RESULT_COPY->doublewords[3];
                        object_hit.doublewords[4] = *&COLLISION_RESULT_COPY->doublewords[4];
                        object_hit.doublewords[5] = *&COLLISION_RESULT_COPY->doublewords[5];
                    }
                    *&COLLISION_RESULT->hit = 0; // match: the address is formed anew
                    func_001B3AC0(request, extra, object);
                }
            }
        }
    }
    if (request->unk4D & 1) {
        if (!((request->unk4D & 0x80) && IS_PLAYER(request->owner))) {
            func_0020F820(&D_00413F80, request, 50.0f, 230.0f, request->unk30);
            if (COLLISION_RESULT->hit != 0) {
                func_001B29C8(request, extra);
                return 1;
            }
        }
    }
    if (request->unk4D & 2) {
        for (entity = D_004467E0; entity != NULL; entity = entity->next) {
            if (!((s32)(entity->flags >> 1) & 1)) {
                continue;
            }
            if ((s32)(entity->flags >> 20) & 1) {
                continue;
            }
            if (((s32)(entity->flags >> 2) & 3) >= 2) {
                continue;
            }
            if ((request->unk4D & 0x80) && request->owner == entity) {
                continue;
            }
            if (!(entity->params->flags & 0x400)) {
                if (((EntityParamsWord *)entity->params)->unk00 & 0x8000000) {
                    func_0020DE48(entity->model->position, request, entity->radius, request->unk30);
                } else {
                    func_0020F820(entity->model->position, request, entity->radius, entity->height, request->unk30);
                }
                if (COLLISION_RESULT->hit != 0) {
                    func_001B2F70(request, extra, entity, 1.0f);
                    return 2;
                }
            } else {
                scale = LARGER(ENTITY_MODEL(entity)->unkE8, ENTITY_MODEL(entity)->unkE0);
                ENTITY_MODEL(entity)->unk1B4(ENTITY_MODEL(entity));
                body = entity->params->unk20;
                for (index = 0; index < 4; index++, body++) {
                    if (!(body->unk00.flags & ENTITY_BODY_USED)) {
                        continue;
                    }
                    point = (void *)ENTITY_MODEL(entity)->get_point(ENTITY_MODEL(entity), body->unk00.point);
                    if (!(body->unk00.flags & ENTITY_BODY_ROUND)) {
                        func_0020F820(point, request, body->unk04 * scale, body->unk06 * ENTITY_MODEL(entity)->unkE4,
                                      request->unk30);
                    } else {
                        func_0020DE48(point, request, body->unk04 * scale, request->unk30);
                    }
                    if (COLLISION_RESULT->hit != 0) {
                        func_001B2F70(request, extra, entity, body->unk08);
                        return 2;
                    }
                }
            }
        }
    }
    if ((UNK1B1470_REQUEST_UNIT(request) & (0xC000L << 29)) == (0xC000L << 29)) {
        kind = 0;
    } else if (request->unk4D & 8) {
        kind = 1;
    } else {
        kind = -1;
        if (request->unk4D & 0x10) {
            kind = 2;
        }
    }
    if (kind != -1) {
        for (index = 0; index < 2; index++) {
            if (D_0038CD20[index].unk00 == 0) {
                continue;
            }
            record = (MapRecordBounds *)D_0038CD20[index].records;
            count = D_0038CD20[index].header->record_count;
            for (item = 0; item < count; item++, record++) {
                if (record->unk30 == 0xFFFF) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                reach = request->unk30;
                if (request->position[0] < record->lowest[0] - reach || record->highest[0] + reach < request->position[0] ||
                    request->position[1] < record->lowest[1] - reach || record->highest[1] + reach < request->position[1] ||
                    request->position[2] < record->lowest[2] - reach || record->highest[2] + reach < request->position[2]) {
                    continue;
                }
                result = D_00402D88[kind](record->model, request, reach);
                if (COLLISION_RESULT->hit != 0) {
                    // match: called as void, retail reuses $2 at once
                    ((void (*)(char *, u8 *))func_001B3E78)((char *)request, (u8 *)extra);
                    switch (result) {
                    case 1:
                        result = 8;
                        break;
                    case 0:
                    case 2:
                        result = 0x10;
                        break;
                    case 5:
                        result = 0x20;
                        break;
                    default:
                        return result;
                    }
                    return result;
                }
            }
        }
    }
    if (object_result != 0) {
        // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
        *&COLLISION_RESULT_COPY->doublewords[0] = object_hit.doublewords[0];
        *&COLLISION_RESULT_COPY->doublewords[1] = object_hit.doublewords[1];
        *&COLLISION_RESULT_COPY->doublewords[2] = object_hit.doublewords[2];
        *&COLLISION_RESULT_COPY->doublewords[3] = object_hit.doublewords[3];
        *&COLLISION_RESULT_COPY->doublewords[4] = object_hit.doublewords[4];
        *&COLLISION_RESULT_COPY->doublewords[5] = object_hit.doublewords[5];
        return object_result;
    }
    return 0;
}
