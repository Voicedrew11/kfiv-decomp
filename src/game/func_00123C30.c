#include "kf_entity.h"

/* Claims a free pair of pool blocks for a two-block type, else the first reusable pair; returns its first index or -1. */
int func_00123C30(void) {
    int i;
    int j;
    int pair;
    int best;

    best = -1;
    for (i = 0; i < 2; i++) {
        pair = i * 2;
        if (D_003D6DF0[pair].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair].head.fields.kind == ENTITY_POOL_FREE && D_003D6DF0[pair + 1].head.fields.kind == ENTITY_POOL_FREE) {
            return pair;
        }
    }
    // match: a second counter, so the pair index gets the register retail uses
    for (j = 0; j < 2; j++) {
        pair = j * 2;
        if (D_003D6DF0[pair].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair].head.fields.kind != ENTITY_POOL_FREE) {
            if (D_003D6DF0[pair].head.fields.kind != ENTITY_POOL_READY) { continue; }
            if (D_003D6DF0[pair].unk4 != 0) { continue; }
            if (D_003D6DF0[pair].unk6 != 0) { continue; }
        }
        if (D_003D6DF0[pair + 1].head.fields.kind == ENTITY_POOL_FREE || D_003D6DF0[pair + 1].head.fields.kind == ENTITY_POOL_EXTENSION) {
            best = pair;
            break;
        }
        if (D_003D6DF0[pair + 1].head.fields.kind != ENTITY_POOL_READY) { continue; }
        if (D_003D6DF0[pair + 1].unk4 != 0) { continue; }
        if (D_003D6DF0[pair + 1].unk6 != 0) { continue; }
        best = pair;
        break;
    }
    if (best >= 0) {
        D_003D6DF0[best].head.fields.kind = ENTITY_POOL_FREE;
        if ((j = best + 1) < 5 && D_003D6DF0[j].head.fields.kind == ENTITY_POOL_EXTENSION) {
            D_003D6DF0[j].head.fields.kind = ENTITY_POOL_FREE;
        }
    }
    return best;
}
