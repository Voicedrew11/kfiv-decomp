#include "kf_entity.h"

/* Marks a pool slot QUEUED for a type id; a two-block type also marks the next slot as its extension. */
void func_00124318(int slot_index, int type_id, int two_blocks) {
    EntityPoolSlot *slot = &D_003D6DF0[slot_index];

    slot->head.fields.kind = ENTITY_POOL_QUEUED;
    slot->head.fields.unk2 = type_id;
    slot->unk4 = 0;
    slot->unk6 = 0;
    slot->age = 0;
    if (two_blocks) {
        slot++;
        slot->head.fields.kind = ENTITY_POOL_EXTENSION;
        slot->head.fields.unk2 = 0xFFFF;
        slot->unk4 = 0;
        slot->unk6 = 0;
        slot->age = 0;
    }
}
