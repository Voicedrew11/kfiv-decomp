#include "kf_entity.h"
#include "kf_entity_world.h"

/* When the dying entity's partner is in state 2, restart it and reload its wait timer. */
void func_00143C28(Entity *entity) {
    Entity *partner;

    partner = Entity_GetBySpawnId(entity->spawn_info->variant);
    if (partner != NULL) {
        if ((partner->flags & 0xE) == 2) {
            partner->state = 2;
            partner->unkE0.unk68 = partner->params->unk43A;
        }
    }
}
