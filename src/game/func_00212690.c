#include "kf_entity.h"
#include "kf_script.h"

void *func_001267B0(int);
s32 func_001C8CB0(Entity *);

/* Run func_001C8CB0 on the looked-up object with its spawn_info->unk38 temporarily set, then clear the command. */
void func_00212690(ScriptSetValueCommand *command) {
    u16 saved_value;
    Entity *entity;
    EntitySpawnInfo *spawn_info;

    entity = func_001267B0(command->object_id.value);
    if (entity != NULL) {
        spawn_info = entity->spawn_info;
        saved_value = *(u16 *)&spawn_info->unk38; // match: the low 16 bits of the 64-bit field
        *(u16 *)&spawn_info->unk38 = command->value.value;
        func_001C8CB0(entity);
        spawn_info = entity->spawn_info;
        *(u16 *)&spawn_info->unk38 = saved_value;
    }
    command->active = 0;
}
