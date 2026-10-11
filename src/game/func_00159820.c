#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

void func_00196870(void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
void func_0015B428(Entity *, s32);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_001599D8(Entity *, s32);
void func_00159D28(Entity *);
void func_0015AE88(Entity *);

void func_00159820(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7C = 30;
    }
    if (unkE0->unk7C > 0) {
        unkE0->unk7C--;
    }
    if (unkE0->unk68 == 1) {
        if (entity->unkC6 == 0) {
            func_001958B8((char *)entity, (void (*)(void))func_0015AE88, 1);
            return;
        }
    } else if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_0015B428(entity, unknown);
        } else {
            switch (entity->unkCC) {
            case 0:
                unkE0->unk7A = 0;
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                if (unkE0->unk7A == 0) {
                    func_001958B8((char *)entity, (void (*)(void))func_00159D28, unknown);
                } else {
                    func_001599D8(entity, unknown);
                }
                break;
            default:
                unkE0->unk7A = 0;
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
