#include "kf_entity.h"

void func_00124798(int index);

/* Releases the entity's work area and clears its pool and work-area indices. */
void func_00123AB8(Entity *entity) {
    func_00124798(entity->unkD4);
    entity->unkD4 = entity->unkD3 = -1;
}
