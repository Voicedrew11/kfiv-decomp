#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
int func_00192EB0(void *, int);

void func_00215230(ScriptCallCommand *command) {
    Entity *target;

    target = func_001267B0(command->target_id.value);
    if (target == NULL || (target->flags & 0xF) != 3 || target->unk2C == 0 ||
        func_00192EB0(target, command->first.value) != 0) {
        command->active = 0;
    }
}
