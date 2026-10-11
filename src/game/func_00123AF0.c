#include "kf_entity.h"

/* Returns the pool block index in use (kinds 1 to 4) for a type id, or -1. */
int func_00123AF0(int type_id) {
    int i;
    EntityPoolSlot *slot;

    for (i = 0; i < ENTITY_POOL_SLOT_COUNT; i++) {
        slot = &D_003D6DF0[i];
        if ((u32)(slot->head.fields.kind - 1) < 4 && slot->head.fields.unk2 == type_id) {
            return i;
        }
    }
    return -1;
}
