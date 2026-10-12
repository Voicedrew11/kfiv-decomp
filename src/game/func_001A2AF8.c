#include "common.h"
#include "kf_effect.h"

extern int D_004468CC;

/* Frees a spawner slot: clears its live flag and counts one slot down. */
void func_001A2AF8(EffectSlot *slot) {
    EFFECT_SLOT_FLAG_BITS(slot)->live = 0;
    D_004468CC--;
}
