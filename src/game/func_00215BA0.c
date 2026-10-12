#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);

/* Script command: set the entity's animation speed (0x80 is normal). */
void func_00215BA0(ScriptCallCommand *command) {
    Entity *entity;

    entity = func_001267B0(command->target_id.value);
    if (entity != NULL) {
        entity->unkD6 = command->first.value;
        entity->model->unk15C = entity->unkD6;
    }
    command->active = 0;
}
