#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_map.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerArmPoints D_00414780;
extern PlayerArmPoints D_004147A0;
extern Entity *D_004467E0;
extern MapSlot D_0038CD20[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[][0x200];

float sqrtf(float);
void KF_Vector_Scale(float *, float *, float);
void KF_Vector_Subtract(void *, void *, void *);
void func_00216778(void *, void *);
void func_001B29C8(Unk1B1470Request *, Unk1B1470Extra *);
void func_001B2F70(Unk1B1470Request *, Unk1B1470Extra *, Entity *, f32);
void func_001B3AC0(Unk1B1470Request *, Unk1B1470Extra *, MapObject *);
int func_001B3E78(char *, u8 *);
void func_001C1C80(f32 *, f32 *);
void func_001C2128(void *, int, int, float);
void func_0020FA58(void *, void *, f32, f32, f32, f32, f32, f32);
void func_0020FCF8(void *, void *, f32, f32, f32, f32, f32, f32, f32, f32, f32);
void func_00210488(void *, void *, f32, f32, f32, f32, f32);
void func_00210678(void *, void *, f32, f32, f32, f32, f32, f32, f32, f32);

#define IS_PLAYER(owner) ((owner) == (Entity *)&D_00413F80)
#define LARGER(first, second) ((first) < (second) ? (second) : (first))

/* Runs a swept attack request (shape 3) against everything it may hit: the player, the
   entities and, for the player's own weapon, the map objects and the map along the two
   edges the weapon moved through. Returns what was hit: 1 the player, 2 an entity, 4 a
   map object, 0x10 the map, 0 nothing. */
s32 func_001B1B90(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    f32 tip[4];
    f32 direction[4];
    Entity *entity;
    EntityBodyPoint *body;
    MapObject *object;
    void *point;
    f32 width_scale;
    f32 scale;
    f32 length;
    s32 index; /* a map, or one of an entity's bodies */
    s32 count;
    s32 item;

    if (request->unk4D & 1) {
        if (!((request->unk4D & 0x80) && IS_PLAYER(request->owner))) {
            func_0020FA58(&D_00413F80, request, 50.0f, 230.0f, request->unk30, request->unk34, request->unk3C,
                          request->unk38);
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
                if (IS_PLAYER(request->owner)) {
                    func_0020FCF8(entity->model->position, request, entity->radius, entity->height, request->unk30,
                                  request->unk78, request->unk7C, request->unk3C, request->unk38, request->unk44,
                                  request->unk40);
                } else {
                    func_0020FA58(entity->model->position, request, entity->radius, entity->height, request->unk30,
                                  request->unk34, request->unk3C, request->unk38);
                }
                if (COLLISION_RESULT->hit != 0) {
                    func_001B2F70(request, extra, entity, 1.0f);
                    return 2;
                }
            } else {
                width_scale = LARGER(ENTITY_MODEL(entity)->unkE8, ENTITY_MODEL(entity)->unkE0);
                scale = ENTITY_MODEL(entity)->unkE0;
                if (scale < ENTITY_MODEL(entity)->unkE4) {
                    scale = ENTITY_MODEL(entity)->unkE4;
                }
                if (scale < ENTITY_MODEL(entity)->unkE8) {
                    scale = ENTITY_MODEL(entity)->unkE8;
                }
                ENTITY_MODEL(entity)->unk1B4(ENTITY_MODEL(entity));
                body = entity->params->unk20;
                for (index = 0; index < 4; index++, body++) {
                    if (!(body->unk00.flags & ENTITY_BODY_USED)) {
                        continue;
                    }
                    point = (void *)ENTITY_MODEL(entity)->get_point(ENTITY_MODEL(entity), body->unk00.point);
                    if (!(body->unk00.flags & ENTITY_BODY_ROUND)) {
                        if (IS_PLAYER(request->owner)) {
                            func_0020FCF8(point, request, body->unk04 * width_scale, body->unk06 * ENTITY_MODEL(entity)->unkE4,
                                          request->unk30, request->unk78, request->unk7C, request->unk3C, request->unk38,
                                          request->unk44, request->unk40);
                        } else {
                            func_0020FA58(point, request, body->unk04 * width_scale, body->unk06 * ENTITY_MODEL(entity)->unkE4,
                                          request->unk30, request->unk34, request->unk3C, request->unk38);
                        }
                    } else {
                        if (IS_PLAYER(request->owner)) {
                            func_00210678(point, request, body->unk04 * scale, request->unk30, request->unk78, request->unk7C,
                                          request->unk3C, request->unk38, request->unk44, request->unk40);
                        } else {
                            func_00210488(point, request, body->unk04 * scale, request->unk30, request->unk34,
                                          request->unk3C, request->unk38);
                        }
                    }
                    if (COLLISION_RESULT->hit != 0) {
                        func_001B2F70(request, extra, entity, body->unk08);
                        return 2;
                    }
                }
            }
        }
    }
    if (!IS_PLAYER(request->owner)) {
        return 0;
    }
    if (request->unk4D & 4) {
        func_00216778(tip, request);
        tip[1] += request->unk34;
        for (index = 0; index < 2; index++) {
            if (D_0038CD20[index].unk00 == 0) {
                continue;
            }
            count = D_004467B8[index];
            object = D_003A1A30[index];
            for (item = 0; item < count; item++, object++) {
                KF_Vector_Subtract(direction, &D_00414780.points[1], &D_00414780.points[0]);
                length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
                KF_Vector_Scale(direction, direction, 1.0f / length);
                func_001C2128(object, (int)&D_00414780.points[0], (int)direction, length);
                if (COLLISION_RESULT->hit != 0) {
                    func_001B3AC0(request, extra, object);
                    return 4;
                }
                KF_Vector_Subtract(direction, &D_00414780.points[1], &D_004147A0.points[1]);
                length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
                KF_Vector_Scale(direction, direction, 1.0f / length);
                func_001C2128(object, (int)&D_004147A0.points[1], (int)direction, length);
                if (COLLISION_RESULT->hit != 0) {
                    func_001B3AC0(request, extra, object);
                    return 4;
                }
            }
        }
    }
    if (!IS_PLAYER(request->owner)) {
        return 0;
    }
    func_001C1C80((f32 *)&D_00414780.points[0], (f32 *)&D_00414780.points[1]);
    if (COLLISION_RESULT->hit != 0) {
        func_001B3E78((char *)request, (u8 *)extra);
        return 0x10;
    }
    func_001C1C80((f32 *)&D_00414780.points[1], (f32 *)&D_004147A0.points[1]);
    if (COLLISION_RESULT->hit != 0) {
        func_001B3E78((char *)request, (u8 *)extra);
        return 0x10;
    }
    return 0;
}
