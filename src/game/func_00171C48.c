#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
int func_0019A448(void *, void *, int, int, int, int, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
void func_00183150(Entity *, s32);
void func_00172320(Entity *, s32);
void func_00171E30(Entity *, s32);
void func_00172F00(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_00172738(Entity *);

void func_00171C48(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 & 0x40) {
        entity->flags &= ~0x10000000;
    } else {
        entity->flags |= 0x10000000;
    }
    if (!(entity->unkC6 & 0x45)) {
        func_0019A448(entity, &unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->unkCC) {
            case 0:
                unkE0->unk7A = 0;
                func_00172320(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                if (unkE0->unk7A == 0) {
                    func_001958B8((char *)entity, (void (*)(void))func_00172F00, unknown);
                    unkE0->unk7A = 1;
                } else {
                    func_00171E30(entity, unknown);
                }
                break;
            default:
                func_00172320(entity, unknown);
                break;
            }
        }
    }
    if (entity->unkC6 == 0 || (entity->unkC6 & 6)) {
        func_00172738(entity);
    }
}
