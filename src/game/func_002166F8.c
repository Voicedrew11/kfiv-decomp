#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_script.h"

void func_002166F8(ScriptObjectCommand *command) {
    Entity *entity;
    s64 flags;

    entity = Entity_GetBySpawnId(command->object_id.value);
    if (entity != NULL) {
        flags = entity->flags;
        if (((flags & 3) == 3) && (entity->unk82 == 0) && ((flags & ((0x30L << 32) | 0xC)) == 0)) {
            entity->flags = flags & ~0x40000;
        }
    }
    command->active = 0;
}
