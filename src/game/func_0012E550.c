#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00180EA8(Entity *);
void func_0012E6B0(Entity *, s32);
void func_0012E7F0(Entity *, s32);

void func_0012E550(Entity *entity, s32 unknown) {
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
        return;
    }
    switch (entity->state) {
    case 0:
        if (entity->prev_state != 0) {
            func_001958B8((char *)entity, (void (*)(void))func_00180EA8, unknown);
        } else {
            func_0012E6B0(entity, unknown);
        }
        return;
    case 1:
        break;
    case 2:
        func_0012E7F0(entity, unknown);
        return;
    }
    func_0012E6B0(entity, unknown);
}
