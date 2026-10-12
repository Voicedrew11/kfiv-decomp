#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern int D_004468CC;

/* Frees every spawner slot that the given effect spawned. */
void func_001A2BC8(EffectActor *owner) {
    EffectSlot *slot;
    int i;

    if (D_004468CC > 0) {
        i = 0;
        slot = D_003F1750;
        for (; i < 0x60; i++, slot++) {
            if ((slot->flags & EFFECT_SLOT_LIVE) && slot->owner_effect == owner) {
                func_001A2AF8(slot);
                if (D_004468CC == 0) {
                    break;
                }
            }
        }
    }
}
