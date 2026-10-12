#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

void func_00196870(void *, int);
void func_0018E9C0(void *);
void func_001958B8(char *, void (*)(void), int);
void func_00135DA0(Entity *);
void func_00136040(Entity *, s32);

void func_00135EF0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 2);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_0018E9C0(entity);
    if (D_00413F80.position[0] < -4900.0f || D_00413F80.position[0] > -2500.0f ||
        D_00413F80.position[2] < -5350.0f || D_00413F80.position[2] > -3350.0f) {
        func_001958B8((char *)entity, (void (*)(void))func_00135DA0, unknown);
        return;
    }
    if (entity->state == 2) {
        func_00136040(entity, unknown);
        return;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
