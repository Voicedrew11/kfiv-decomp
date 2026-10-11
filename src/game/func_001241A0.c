#include "kf_entity.h"

void func_001243E8(int slot_index);
void func_001A32A8(Entity *);

/* Frees the users of a pool slot and marks the slot empty; keeps it if a small block still uses it. */
void func_001241A0(int slot_index) {
    Entity *entity;
    Entity *next;
    EntityPoolSlot *slot;

    slot = &D_003D6DF0[slot_index];
    entity = D_004470B0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            // match: nested ifs keep the signed pool_size compares
            if (entity->spawn_info->pool_size >= 0) {
                if (entity->spawn_info->pool_size < 5) {
                    return;
                }
            }
        }
        entity = entity->next;
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            // match: nested ifs keep the signed pool_size compares
            if (entity->spawn_info->pool_size >= 0) {
                if (entity->spawn_info->pool_size < 5) {
                    return;
                }
            }
        }
        entity = entity->next;
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            next = entity->next;
            func_00124CA0(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            func_001A32A8(entity);
            func_001243E8(entity->unkD3);
            func_00123AB8(entity);
            next = entity->next;
            func_00124D48(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    slot->unk4 = 0;
    slot->head.fields.unk2 = 0xFFFF;
    slot->head.word |= ENTITY_POOL_FLAG_100;
}
