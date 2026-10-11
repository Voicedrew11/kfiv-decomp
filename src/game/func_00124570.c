#include "kf_entity.h"

/* Frees each pool slot whose type no live entity uses any more. */
void func_00124570(void) {
    EntityPoolSlot *slot;
    Entity *entity;
    Entity *list_a;
    Entity *list_b;
    Entity *list_c;
    int slot_index;

    list_a = D_004470A8;
    list_b = D_004470B0;
    list_c = D_004467E0;
    slot_index = 0;
    slot = D_003D6DF0;
    while (slot_index < ENTITY_POOL_SLOT_COUNT) {
        if ((slot->head.word & 0x2FF) == (ENTITY_POOL_FLAG_200 | ENTITY_POOL_READY)) {
            for (entity = list_a; entity != NULL; entity = entity->next) {
                if (entity->spawn_info->type_id == slot->head.fields.unk2) {
                    goto in_use;
                }
            }
            for (entity = list_b; entity != NULL; entity = entity->next) {
                if (entity->spawn_info->type_id == slot->head.fields.unk2) {
                    goto in_use;
                }
            }
            for (entity = list_c; entity != NULL; entity = entity->next) {
                if (entity->spawn_info->type_id == slot->head.fields.unk2) {
                    goto in_use;
                }
            }
            slot->head.fields.kind = ENTITY_POOL_FREE;
            slot->head.fields.unk2 = 0xFFFF;
            slot->head.bits.flag_100 = 0;
            slot->head.bits.flag_200 = 0;
            if (slot_index < 4 && slot[1].head.fields.kind == ENTITY_POOL_EXTENSION) {
                slot[1].head.fields.kind = ENTITY_POOL_FREE;
                slot[1].head.fields.unk2 = 0xFFFF;
                slot[1].head.bits.flag_100 = 0;
                slot[1].head.bits.flag_200 = 0;
            }
        }
in_use:
        slot_index++;
        slot++;
    }
}
