#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
int func_00192EB0(void *, int);
float func_00194FB8(float);

/* Per frame, shift the target entity's rotation by the command's deltas (wrapped), then count down. */
void func_00215850(ScriptMoveCommand *command) {
    Entity *target;

    target = func_001267B0(command->target_id.value);
    if (target != NULL && (target->flags & 0xF) == 3 && target->unk2C != 0) {
        ENTITY_MODEL(target)->rotation.x = func_00194FB8(ENTITY_MODEL(target)->rotation.x + command->x.value);
        ENTITY_MODEL(target)->rotation.y = func_00194FB8(ENTITY_MODEL(target)->rotation.y + command->y.value);
        ENTITY_MODEL(target)->rotation.z = func_00194FB8(ENTITY_MODEL(target)->rotation.z + command->z.value);
        if (command->unk2C.value != 0) {
            func_00192EB0(target, command->unk34.value);
        }
        if (--command->frames.value != 0) {
            return;
        }
    }
    command->active = 0;
}
