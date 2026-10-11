#include "kf_entity.h"

extern int D_00413F80[];
s16 func_00190F90(float, void *, int);
void func_001958B8(void *, void (*)(void), int);

void func_00131B38(Entity *entity, int invoke_now) {
    EntityUnkE0 *sub = &entity->unkE0;

    entity->target = D_00413F80;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    entity->unkC8 = func_00190F90(50.0f, entity, 1);
    func_001958B8(entity, sub->unk18, invoke_now);
}
