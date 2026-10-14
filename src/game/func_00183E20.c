#include "kf_entity.h"

extern EntityGroupRecord D_0034B6D0[];

void func_00190A30(char *);
int EntityAi_SetReaction(Entity *, void (*)(void), int);
void func_00184778(Entity *);

void func_00183E20(Entity *entity) {
    func_00190A30((char *)entity);
    entity->flags &= ~0x400;
    entity->target = NULL;
    entity->flags &= ~0x200;
    EntityAi_SetReaction(entity, (void (*)(void))func_00184778, 0);
    D_0034B6D0[ENTITY_CONTROLLER(entity)->unk43A].unk02 = 1;
}
