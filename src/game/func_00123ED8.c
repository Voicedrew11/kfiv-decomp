#include "kf_entity.h"

int func_00124008(int);
void func_001240B8(int);

/* Claims a pair of pool blocks starting at this one for a two-block type; returns its index or -1. */
int func_00123ED8(int pool_size) {
    EntityPoolSlot *slot = &D_003D6DF0[pool_size];
    EntityPoolSlot *next = &D_003D6DF0[pool_size + 1];
    int first_free;
    int second_free;

    if (slot->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    if (slot->head.fields.kind == ENTITY_POOL_FREE ||
        (slot->head.fields.kind == ENTITY_POOL_READY && slot->unk4 == 0 && slot->unk6 == 0)) {
        first_free = TRUE;
    } else {
        first_free = !func_00124008(pool_size);
    }
    if (!first_free) {
        return -1;
    }
    if (next->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    // the game's own slip: this result is overwritten by the test right below (an `else` is missing),
    // so a free or ready second block is always checked with func_00124008. It leaves a dead test in the code.
    if (next->head.fields.kind == ENTITY_POOL_FREE || next->head.fields.kind == ENTITY_POOL_READY) {
        second_free = TRUE;
    }
    if (next->head.fields.kind == ENTITY_POOL_EXTENSION && first_free) {
        second_free = TRUE;
    } else {
        second_free = !func_00124008(pool_size + 1);
    }
    if (!second_free) {
        return -1;
    }
    func_001240B8(pool_size);
    func_001240B8(pool_size + 1);
    slot->head.fields.unk2 = 0xFFFF;
    slot->unk4 = 0;
    slot->head.bits.flag_100 = TRUE;
    next->head.fields.unk2 = 0xFFFF;
    next->unk4 = 0;
    next->head.bits.flag_100 = TRUE;
    return pool_size;
}
