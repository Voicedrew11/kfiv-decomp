#include "kf_entity.h"

extern u32 D_00446ED4;

int func_0019A448(void *, void *, int, int, int, int, int);
void func_00196870(void *, int);
void func_00196C28(char *);
void func_0018E9C0(void *);
void func_00134B50(Entity *, s32);
void func_00134DC8(Entity *, s32);
void func_00134F68(Entity *, s32);

void func_001349E8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (!(entity->unkC6 & 0x40)) {
        func_0019A448(entity, unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196C28((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00134B50(entity, unknown);
        return;
    }
    if (unkE0->unk7A == 0) {
        func_00134DC8(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        func_00134F68(entity, unknown);
        return;
    }
    func_00134DC8(entity, unknown);
}
