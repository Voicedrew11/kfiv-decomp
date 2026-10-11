#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
int func_00192EB0(void *, int);

/* Each frame, run the callback on the target entity until the frame count runs out. */
void func_002151C0(ScriptMotionCommand *command) {
    Entity *entity;

    entity = func_001267B0(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        func_00192EB0(entity, command->motion.value);
        if (--command->frames.value != 0) {
            return;
        }
    }
    command->active = 0;
}
