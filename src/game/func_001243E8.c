#include "kf_entity.h"

extern u32 D_00446ED4;

/* Drops one user of a pool block; when none are left, marks it released at the current frame. */
void func_001243E8(int slot_index) {
    EntityPoolSlot *slot;

    slot = &D_003D6DF0[slot_index];
    if (--slot->unk4 <= 0) {
        slot->unk6 = 2;
        slot->age = D_00446ED4;
        slot->unk4 = 0;
    }
}
