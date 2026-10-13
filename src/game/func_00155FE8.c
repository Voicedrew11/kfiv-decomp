#include "kf_entity.h"
#include "kf_map.h"
#include "kf_map_object.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];

void func_00196870(void *, int);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void func_00216D20(void *, void *);
void func_00194F48(f32 *, f32 *);
s32 func_0018A970(Entity *, f32 *, PlayerData *);
float func_00194F80(void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_001566A0(Entity *, s32);
void func_00156E88(Entity *);

void func_00155FE8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 position[4];
    f32 target[4];
    f32 delta[4];
    Entity *first;
    Entity *second;
    Entity *third;
    MapRecord *records;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
        func_00216778(position, entity->model->position);
        func_00216778(target, &D_00413F80);
        position[1] = entity->model->position[1] - entity->unk34 * 0.5f;
        target[1] = D_00413F80.position[1] - 115.0f;
        KF_Vector_Subtract(delta, target, position);
        func_00216D20(delta, delta);
        func_00194F48(position, target);
        first = (Entity *)KF_MapObject_GetById(entity->group_id, 0xA0);
        second = (Entity *)KF_MapObject_GetById(entity->group_id, 0xA1);
        third = (Entity *)KF_MapObject_GetById(entity->group_id, 0xA2);
        if (unkE0->unk7A == 0) {
            records = (MapRecord *)D_0038CD20[entity->group_id].records;
            if (entity->unk14 == &records[0x11]) {
                if (func_0018A970(first, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 1)
                    && (func_00194F80(&D_00413F80, first->model->position) < 500.0f
                        || func_00194F80(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x14) {
                    unkE0->unk7A = 2;
                    unkE0->unk7B |= 1;
                    func_001958B8((char *)entity, (void (*)(void))func_00156E88, unknown);
                    return;
                }
                if (func_0018A970(second, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 2)
                    && (func_00194F80(&D_00413F80, second->model->position) < 500.0f
                        || func_00194F80(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x16) {
                    unkE0->unk7A = 3;
                    unkE0->unk7B |= 2;
                    func_001958B8((char *)entity, (void (*)(void))func_00156E88, unknown);
                    return;
                }
            } else if (entity->unk14 == &records[0x14]) {
                if (func_0018A970(first, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 1)
                    && (func_00194F80(&D_00413F80, first->model->position) < 500.0f
                        || func_00194F80(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x11) {
                    unkE0->unk7A = 2;
                    unkE0->unk7B |= 1;
                    func_001958B8((char *)entity, (void (*)(void))func_00156E88, unknown);
                    return;
                }
                if (func_0018A970(third, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 4)
                    && (func_00194F80(&D_00413F80, third->model->position) < 500.0f
                        || func_00194F80(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x16) {
                    unkE0->unk7A = 1;
                    unkE0->unk7B |= 4;
                    func_001958B8((char *)entity, (void (*)(void))func_00156E88, unknown);
                    return;
                }
            } else if (entity->unk14 == &records[0x16]) {
                if (func_0018A970(second, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 2)
                    && (func_00194F80(&D_00413F80, second->model->position) < 500.0f
                        || func_00194F80(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x11) {
                    unkE0->unk7A = 3;
                    unkE0->unk7B |= 2;
                    func_001958B8((char *)entity, (void (*)(void))func_00156E88, unknown);
                    return;
                }
                if (func_0018A970(third, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 4)
                    && (func_00194F80(&D_00413F80, third->model->position) < 500.0f
                        || func_00194F80(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x14) {
                    unkE0->unk7A = 1;
                    unkE0->unk7B |= 4;
                    func_001958B8((char *)entity, (void (*)(void))func_00156E88, unknown);
                    return;
                }
            }
        }
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_001566A0(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
