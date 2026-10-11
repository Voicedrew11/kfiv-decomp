#include "kf_entity.h"

int func_0019A448(void *, void *, int, int, int, int, int);

void func_0017E560(Entity *entity) {
    entity->flags &= ~0x3800ULL;
    func_0019A448(entity, entity->unkE0.unk80, 0, 3, 5, ENTITY_CONTROLLER(entity)->unkC4, 0);
    func_0019A448(entity, entity->unkE0.unk88, 1, 3, 5, ENTITY_CONTROLLER(entity)->unkC4, 0);
}
