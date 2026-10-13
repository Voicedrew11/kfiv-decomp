#include "kf_script.h"
#include "kf_entity_world.h"
#include "kf_entity.h"

/* Script command: set the entity's animation speed (0x80 is normal). */
void func_00215BA0(ScriptCallCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        entity->unkD6 = command->first.value;
        entity->model->unk15C = entity->unkD6;
    }
    command->active = 0;
}
