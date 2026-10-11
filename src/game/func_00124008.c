#include "kf_entity.h"

/* TRUE if a one-block entity (pool_size 0-4) uses this pool slot, or the block it extends. */
int func_00124008(int slot_index) {
    Entity *entity;

    if (D_003D6DF0[slot_index].head.fields.kind == ENTITY_POOL_EXTENSION) {
        slot_index--;
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3 && entity->spawn_info->pool_size >= 0 && entity->spawn_info->pool_size < 5) {
            return TRUE;
        }
        entity = entity->next;
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3 && entity->spawn_info->pool_size >= 0 && entity->spawn_info->pool_size < 5) {
            return TRUE;
        }
        entity = entity->next;
    }
    return FALSE;
}
