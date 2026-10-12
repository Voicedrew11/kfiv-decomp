#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;

int rand(void);
int func_00162A48(char *, float, float);
s32 func_0018EEE0(Entity *, f32, f32);
float func_00195478(float *, char *);
float func_001957B8(float, float);
s32 func_00197F00(Entity *);
s32 func_0018F648(Entity *);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
float func_00194FB8(float);
s32 func_0018EC78(Entity *, f32);
s16 func_00190BE0(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_00161A48(Entity *);
void func_001620D8(Entity *);
void func_00162410(Entity *);
void func_001625A0(Entity *);

void func_001601B0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    entity->flags &= ~0x3800;
    if (func_00162A48((char *)entity, 600.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_001625A0, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 1.0471976f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00162410, 1);
        return;
    }
    entity->flags |= 0x200;
    func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position));
    unkE0->unk5C = 100.0f;
    unkE0->unk60 = 250.0f;
    if (entity->unkA2 == 0 && func_00197F00(entity)) {
        func_0018F648(entity);
        func_001958B8((char *)entity, (void (*)(void))func_00161A48, unknown);
        return;
    }
    mode = func_0018FD50(entity, 0);
    if (entity->unkA0 == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, mode);
        if (entity->unkCD == 1) {
            entity->target = D_00413F80;
            angle = func_001957B8(D_00413F80[5], entity->unk68);
            if (entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                func_001958B8((char *)entity, (void (*)(void))func_001620D8, unknown);
                return;
            }
            if (func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = 30;
        } else {
            unkE0->unk78 = -1;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 |= 0x201;
        } else if (side < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
        }
        break;
    case 1:
        if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] + 3.1415927f))) {
            unkE0->unk78 = rand() % 3 + 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
            entity->unkC8 |= 2;
        }
        break;
    case 2:
        if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] - 1.5707964f))) {
            unkE0->unk78 = 3;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
            entity->unkC8 |= 4;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 3:
        if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] + 1.5707964f))) {
            unkE0->unk78 = 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
            entity->unkC8 |= 8;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 4:
        entity->unkC8 |= func_00190BE0(entity);
        break;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
