#include "kf_entity.h"

extern EntityPoolSlot D_003D6DF0[];

/* Claims the first free pool block, else the unused block with the lowest size; returns its index. */
int func_00123B40(void) {
    int i;
    int best;
    EntityPoolSlot *slot;

    best = -1;
    for (i = 0; i < 5; i++) {
        slot = &D_003D6DF0[i];
        if ((slot->head.word >> 9) & 1) { continue; }
        if ((slot->head.word >> 8) & 1) { continue; }
        if (slot->head.fields.kind == 0) { return i; }
        if (slot->head.fields.kind != 4) { continue; }
        if (slot->unk4 != 0) { continue; }
        if (slot->unk6 != 0) { continue; }
        if (best == -1 || (u32)slot->age < (u32)D_003D6DF0[best].age) {
            best = i;
        }
    }
    if (best >= 0) {
        D_003D6DF0[best].head.fields.kind = 0;
        if ((i = best + 1) < 5 && D_003D6DF0[i].head.fields.kind == 5) {
            D_003D6DF0[i].head.fields.kind = 0;
        }
    }
    return best;
}
