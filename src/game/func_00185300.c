#include "kf_entity.h"

extern EntityGroupRecord D_0034B6D0[];

void func_00190A30(Entity *);
void func_0019A310(Entity *, void (*)(void), int);
void func_00184778(void);

void func_00185300(Entity *entity) {
    func_00190A30(entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    func_0019A310(entity, func_00184778, 0);
    D_0034B6D0[ENTITY_CONTROLLER(entity)->unk43A].unk02 = 1;
    if (((u64)entity->flags >> 38) & 3) {
        entity->flags |= 0x20000000;
    }
}
