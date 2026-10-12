#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_0018E9C0(void *);
void func_001958B8(char *, void (*)(void), int);
void func_0014EBA8(Entity *, s32);
void func_0014F3F8(Entity *);

void func_0014EA70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (entity->unkC6 == 0) {
        func_0018E9C0(entity);
        if (entity->unk2C < (s32)(entity->params->unk0A * 0.75f)) {
            func_001958B8((char *)entity, (void (*)(void))func_0014F3F8, unknown);
            return;
        }
        switch (entity->state) {
        case 2:
            func_0014EBA8(entity, unknown);
            break;
        case 0:
        case 1:
        default:
            func_001958B8((char *)entity, unkE0->unk18, unknown);
            break;
        }
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    }
}
