#include "kf_entity.h"

int EntityAi_StartAction(char *, void (*)(void), int);

int func_00185460(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;

    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_StartAction((char *)entity, entity->unkE0.move, unknown);
}
