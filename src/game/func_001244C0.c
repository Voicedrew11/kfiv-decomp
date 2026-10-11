#include "kf_entity.h"

/* Frees every entity on lists A and B whose type id matches the slot's, then marks the slot free. */
void func_001244C0(EntityPoolSlot *slot) {
    Entity *entity;

    entity = D_004470A8;
    while (entity != NULL) {
        if (entity->spawn_info->type_id == slot->head.fields.unk2) {
            entity = func_00124C08(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (entity->spawn_info->type_id == slot->head.fields.unk2) {
            entity = func_00124CA0(entity);
        } else {
            entity = entity->next;
        }
    }

    slot->head.fields.kind = ENTITY_POOL_FREE;
    slot->head.fields.unk2 = 0xFFFF;
    slot->head.bits.flag_100 = 0;
    slot->head.bits.flag_200 = 0;
}
