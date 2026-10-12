#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00140E88(Entity *, s32);
void func_001411B0(Entity *);

void func_00140CC0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
        if (entity->state == 2 && ENTITY_CONTROLLER(entity)->unk434 != 0) {
            entity->unkC8 |= 0x440;
        }
        return;
    }
    switch (entity->state) {
    case 0:
        if (unkE0->unk6C == 0) {
            func_001958B8((char *)entity, (void (*)(void))func_001411B0, unknown);
            unkE0->unk6C = 1;
            return;
        }
        func_00182FD8(entity, unknown);
        return;
    case 1:
        unkE0->unk6C = 0;
        func_00183150(entity, unknown);
        return;
    case 2:
        func_00140E88(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}
