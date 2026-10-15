#include "kf_entity.h"

void EntityAi_SetState(char *, u8);
int EntityAi_StartAction(char *, void (*)(void), int);

void func_001841E0(Entity *entity, int unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    unkE0->unk68 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_SetState((char *)entity, 0);
    EntityAi_StartAction((char *)entity, unkE0->move, unknown);
}
