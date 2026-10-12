#include "kf_entity.h"

extern float D_00413F80[];

s32 func_00198D10(Entity *);
s32 func_0018F9C0(Entity *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00182240(Entity *);

void func_00131BB8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (func_00198D10(entity)) {
        func_0018F9C0(entity);
        func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    entity->target = D_00413F80;
    entity->unkC8 = func_00190F90(50.0f, entity, 1);
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
