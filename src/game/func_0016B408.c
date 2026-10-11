#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_0016B560(Entity *, s32);

void func_0016B408(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if ((s32)(entity->flags >> 53) & 1) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_0016B560(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if ((s16)entity->params->unk434 != 0 && (entity->flags & (0x8200L << 35))) {
        unkE0->unk7A = 1;
    } else {
        unkE0->unk7A = 0;
    }
}
