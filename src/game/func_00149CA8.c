#include "kf_entity.h"

extern u32 D_00446ED4;
extern u8 D_00349690[];

void func_00149ED8(Entity *, s32);

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00149610(Entity *, s32);

void func_00149CA8(Entity *entity, s32 unknown) {
    if (entity->model->unkFC->unk88 == 0 &&
        ((entity->unkD1 == 0x17 && entity->model->unkFC->unk82 >= 7 && entity->model->unkFC->unk82 < 0x24) ||
         (entity->unkD1 == 0x18 && entity->model->unkFC->unk82 >= 0x19 && entity->model->unkFC->unk82 < 0x32))) {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 |= 0x8000L << 25;
    } else {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 &= ~(0x8000L << 25);
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (D_00349690[entity->spawn_info->unk42 + entity->spawn_info->variant * 64] != 0) {
        func_00149ED8(entity, unknown);
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
        func_00182FD8(entity, unknown);
        return;
    case 1:
        func_00183150(entity, unknown);
        return;
    case 2:
        func_00149610(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}
