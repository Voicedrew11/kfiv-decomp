#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_001777B0(Entity *, s32);

void func_001775F0(Entity *entity, s32 unknown) {
    if (((u8)entity->unkD1 == 0x17 || (u8)entity->unkD1 == 0x18) && entity->model->unkFC->unk88 == 0 &&
        entity->model->unkFC->unk82 >= 9 && entity->model->unkFC->unk82 < 0x2F) {
        entity->model->unkF8[5].unk68 &= ~(0x8000L << 25);
    } else {
        entity->model->unkF8[5].unk68 |= 0x8000L << 25;
    }
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
    switch (entity->unkCC) {
    case 0:
        func_00182FD8(entity, unknown);
        return;
    case 1:
        func_00183150(entity, unknown);
        return;
    case 2:
        func_001777B0(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}
