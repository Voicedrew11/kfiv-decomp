#include "kf_script.h"
#include "kf_entity_world.h"
#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);

/* Script command: call func_00192C98 on the target entity with two arguments. */
void func_00215158(ScriptCallCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        func_00192C98(entity, command->first.value, 0, command->second.value, 1);
    }
    command->active = 0;
}
