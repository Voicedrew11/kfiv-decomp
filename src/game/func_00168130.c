#include "kf_entity.h"
#include "kf_entity_world.h"

void func_00168130(Entity *entity) {
    Entity *other = Entity_GetBySpawnId(entity->unkE0.unk7B);

    if (other != NULL) {
        other->unkE0.unk7A = 1;
    }
}
