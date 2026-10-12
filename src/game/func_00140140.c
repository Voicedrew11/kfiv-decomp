#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
s32 func_001992F0(Entity *, s32);
void func_001B4EC8(s32);
int func_00192C98(void *, int, int, u32, int);
void func_0018DB58(Entity *, s32, s32);

void func_00140140(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s8 previous;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        previous = unkE0->unk7A;
        if (!(s32)(entity->flags >> 63) && func_001992F0(entity, 0)) {
            func_001B4EC8((s16)entity->params->unk434);
            unkE0->unk7A = 1;
            ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
            func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
        } else {
            unkE0->unk7A = 0;
            ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
            func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
        }
        if (previous != unkE0->unk7A) {
            func_0018DB58(entity, 1, 0);
        }
    }
}
