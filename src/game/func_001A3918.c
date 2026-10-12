#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern int D_004468CC;

/* Frees every live spawner slot whose effect_param is effect_param, while any are live. */
void func_001A3918(int effect_param) {
    EffectSlot *slot;
    int i;

    slot = D_003F1750;
    for (i = 0; i < 0x60 && D_004468CC > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->effect_param == effect_param) {
            func_001A2AF8(slot);
        }
    }
}
