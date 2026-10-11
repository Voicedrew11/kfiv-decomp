#include "kf_entity.h"

/* Adds a user to a pool block and clears its flags; a two-block type's second block gets the same flag clear. */
void func_00124370(int slot_index) {
    D_003D6DF0[slot_index].head.bits.flag_100 = 0;
    D_003D6DF0[slot_index].unk4++;
    D_003D6DF0[slot_index].head.bits.flag_200 = 0;
    if (slot_index < 4 && D_003D6DF0[slot_index + 1].head.fields.kind == ENTITY_POOL_EXTENSION) {
        D_003D6DF0[slot_index + 1].head.bits.flag_100 = 0;
        D_003D6DF0[slot_index + 1].head.bits.flag_200 = 0;
    }
}
