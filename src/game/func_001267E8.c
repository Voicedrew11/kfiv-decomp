#include "kf_entity.h"

extern void *D_004FEB90[][0x100];

/* Looks up an entity of the given map by id. */
void *func_001267E8(int map_index, int entity_id) {
    if ((u32)entity_id >= 0x100) {
        return NULL;
    }
    return D_004FEB90[map_index][entity_id];
}
