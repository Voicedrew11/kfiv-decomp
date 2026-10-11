#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_0015EA70(Entity *, s32);
void func_0015F0C0(Entity *);
void func_0015F5E8(Entity *);
void func_001958B8(char *, void (*)(void), int);

void func_0015E820(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (unkE0->unk6C > 0) {
        unkE0->unk6C--;
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7B = 30;
    }
    if (unkE0->unk7B > 0) {
        unkE0->unk7B--;
    }
    if (unkE0->unk68 == 1) {
        if (entity->unkC6 == 0) {
            func_001958B8((char *)entity, (void (*)(void))func_0015F5E8, 1);
        }
        return;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if (unkE0->unk6C == 0 && (s32)(entity->unk2C * 100.0f / entity->params->unk0A) < entity->params->unk436
            && rand() % 100 < entity->params->unk438) {
            func_001958B8((char *)entity, (void (*)(void))func_0015F0C0, 1);
            return;
        }
        if ((s32)(entity->flags >> 53) & 1) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->unkCC) {
            case 0:
                unkE0->unk68 = 0;
                func_00182FD8(entity, unknown);
                break;
            case 1:
                unkE0->unk68 = 0;
                func_00183150(entity, unknown);
                break;
            case 2:
                func_0015EA70(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
