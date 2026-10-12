#include "kf_entity.h"

extern u32 D_00446ED4;
extern u8 D_00349690[];

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00183240(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00130490(Entity *);

void func_00130300(Entity *entity, s32 unknown) {
    s32 index;

    if (entity->spawn_info->unk40 == 1 && entity->spawn_info->variant == 1) {
        index = entity->spawn_info->unk43;
        index += entity->spawn_info->unk42 * 64;
        if (D_00349690[index] != 0) {
            func_001958B8((char *)entity, (void (*)(void))func_00130490, 1);
            return;
        }
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) != 0x6000000) {
            func_00196CC0((char *)entity);
        }
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
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
                func_00183240(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
