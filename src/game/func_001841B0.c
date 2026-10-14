#include "kf_entity.h"

int EntityAi_StartAction(char *, void (*)(void), int);

void func_001841B0(Entity *entity, int unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u16 timer = unkE0->unk68;

    ENTITY_FLAGS(entity) |= 0x200;
    entity->unkC8 = timer;
    EntityAi_StartAction((char *)entity, unkE0->move, unknown);
}
