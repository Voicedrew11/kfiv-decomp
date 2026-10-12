#include "kf_entity.h"

s32 func_001503A8(Entity *);
s32 func_001992F0(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_001958F8(Entity *, void (*)(void), int, int, int);
void func_0014EFB0(Entity *);
void func_0014F240(Entity *);

void func_0014EBA8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (func_001503A8(entity)) {
        func_001958B8((char *)entity, (void (*)(void))func_0014F240, unknown);
    } else if (entity->unkA2 == 0 && func_001992F0(entity, 0)) {
        func_001958F8(entity, (void (*)(void))func_0014EFB0, unknown, 1, 0);
    } else {
        func_001958B8((char *)entity, unkE0->unk18, unknown);
    }
}
