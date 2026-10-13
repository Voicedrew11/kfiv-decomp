#include "kf_script.h"
#include "kf_entity_world.h"
#include "kf_entity.h"

int func_00192EB0(void *, int);

void func_00215230(ScriptCallCommand *command) {
    Entity *target;

    target = Entity_GetBySpawnId(command->target_id.value);
    if (target == NULL || (target->flags & 0xF) != 3 || target->unk2C == 0 ||
        func_00192EB0(target, command->first.value) != 0) {
        command->active = 0;
    }
}
