#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_map.h"
#include "kf_script.h"

extern Entity D_003BEDF0[][0x80];
extern MapSlot D_0038CD20[];

int func_00103548(int, int);
int func_00211CF8(ScriptArg *, int);

/* Looks up an entity's map entry index and stores it in the command's first argument. */
void func_00212388(ScriptCallCommand *command) {
    Entity *entity;
    int map_slot;
    int entry_index;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        map_slot = entity->group_id;
        entry_index = func_00103548(
            D_0038CD20[map_slot].tag,
            (s32)(((u32)((char *)entity - (char *)D_003BEDF0[map_slot])) / sizeof(Entity))); // match: divu
        func_00211CF8(&command->first, entry_index);
    } else {
        func_00211CF8(&command->first, -1);
    }
    command->active = 0;
}
