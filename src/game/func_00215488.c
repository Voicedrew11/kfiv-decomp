#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
void func_00195898(char *, u8);

/* Script command: put the entity in state 6 once, then wait until it is back in state 0. */
void func_00215488(ScriptWaitCommand *command) {
    Entity *entity;

    entity = func_001267B0(command->object_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        if (entity->unkC6 != 0) {
            return;
        }
        if (command->started == 0) {
            func_00195898((char *)entity, 6);
            command->started = 1;
        }
        if (entity->state != 0) {
            return;
        }
    }
    command->active = 0;
}
