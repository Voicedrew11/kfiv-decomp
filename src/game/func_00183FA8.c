#include "kf_entity.h"

int EntityAi_StartAction(char *, void (*)(void), int);

void func_00183FA8(Entity *entity, int unknown) {
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_StartAction((char *)entity, entity->unkE0.move, unknown);
}
