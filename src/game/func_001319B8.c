#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

void func_001319B8(Entity *entity, s32 unknown) {
    entity->unkC8 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_001958B8((char *)entity, entity->unkE0.unk18, unknown);
}
