#include "kf_script.h"
#include "kf_entity.h"

void *func_001267B0(int);
int func_00211CF8(ScriptArg *, int);

/* Script command: set flag bit 18 on the entity if it may be set, and store whether it was. */
void func_00216660(ScriptLookupCommand *command) {
    Entity *entity;
    s64 flags;

    entity = func_001267B0(command->input.value);
    if (entity != NULL) {
        flags = entity->flags;
        if (((flags & 3) == 3) && (entity->unk82 == 0) && ((flags & ((0x30L << 32) | 0xC)) == 0)) {
            entity->flags = flags | 0x40000;
            func_00211CF8(&command->result, TRUE);
            command->active = 0;
            return;
        }
    }
    func_00211CF8(&command->result, FALSE);
    command->active = 0;
}
