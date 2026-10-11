#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00165280(Entity *, s32);

void func_001650A0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->distance_to_player < entity->radius + 300.0f + 100.0f && unkE0->unk7A == 1) {
        unkE0->unk7A = 0;
    } else if (entity->radius + 1000.0f + 100.0f < entity->distance_to_player && unkE0->unk7A == 0) {
        unkE0->unk7A = 1;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if ((s32)(entity->flags >> 53) & 1) {
            // match: called as returning nothing, so $2 is free for the next statement
            ((void (*)(void *, int))func_00182DE0)(entity, unknown);
            entity->unkC8 |= 0x440;
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_00165280(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
