#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_0018E9C0(void *);
void func_0012A370(Entity *, s32);
void func_0012A478(Entity *, s32);

void func_0012A290(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    entity->flags |= 0x200;
    func_0018E9C0(entity);
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        func_0012A478(entity, unknown);
        return;
    }
    func_0012A370(entity, unknown);
}
