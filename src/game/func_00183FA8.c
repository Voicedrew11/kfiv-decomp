#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

void func_00183FA8(Entity *entity, int unknown) {
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_001958B8((char *)entity, entity->unkE0.unk18, unknown);
}
