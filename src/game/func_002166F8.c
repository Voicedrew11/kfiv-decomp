#include "kf_entity.h"
#include "kf_script.h"

void *func_001267B0(int);

void func_002166F8(ScriptObjectCommand *command) {
    Entity *entity;
    s64 flags;

    entity = func_001267B0(command->object_id.value);
    if (entity != NULL) {
        flags = entity->flags;
        if (((flags & 3) == 3) && (entity->unk82 == 0) && ((flags & ((0x30L << 32) | 0xC)) == 0)) {
            entity->flags = flags & ~0x40000;
        }
    }
    command->active = 0;
}
