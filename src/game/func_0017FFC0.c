#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00180170(Entity *, s32);

void func_0017FFC0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (unkE0->unk6E > 0) {
        unkE0->unk6E--;
    }
    if (!(entity->unkC6 & 0x1000) && unkE0->unk6E == 0) {
        unkE0->unk68 = entity->params->unk3D8[0].record.unk00;
        unkE0->unk6A = entity->params->unk240[1].unk00;
        unkE0->unk6C = (u8)entity->params->unk350[0];
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
        func_00182FD8(entity, unknown);
        return;
    case 1:
        func_00183150(entity, unknown);
        return;
    case 2:
        func_00180170(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}
