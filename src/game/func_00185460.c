#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

int func_00185460(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;

    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_001958B8((char *)entity, entity->unkE0.unk18, unknown);
}
