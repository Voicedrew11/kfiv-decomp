#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
int func_00192EB0(void *, int);
float func_0019B838(float);
float atan2f(float, float);

/* Per frame, turn the target entity's heading a step toward facing the point (x, z), then count down. */
void func_00215920(ScriptMoveCommand *command) {
    Entity *target;
    f32 heading;

    target = func_001267B0(command->target_id.value);
    if (target == NULL || (target->flags & 0xF) != 3 || target->unk2C == 0) {
        command->active = 0;
        return;
    }
    heading = func_0019B838(atan2f(command->x.value - ENTITY_MODEL(target)->position.x,
                                   command->z.value - ENTITY_MODEL(target)->position.z) + 3.1415927f);
    if (heading == ENTITY_MODEL(target)->rotation.y) {
        command->active = 0;
        return;
    }
    ENTITY_MODEL(target)->rotation.y += func_0019B838(heading - ENTITY_MODEL(target)->rotation.y) / (f32)command->frames.value;
    if (command->unk2C.value != 0) {
        func_00192EB0(target, command->unk34.value);
    }
    if (--command->frames.value == 0) {
        command->active = 0;
    }
}
