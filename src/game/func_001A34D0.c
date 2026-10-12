#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern int D_004468CC;
extern int D_004468D0;


/* Frees every spawner slot and effect whose effect_param is the given id. */
void func_001A34D0(int effect_param) {
    EffectSlot *spawn_slot;
    EffectActor *effect;
    int i;

    spawn_slot = D_003F1750;
    effect = D_003F3550;
    for (i = 0; i < 0x60 && D_004468CC > 0; i++, spawn_slot++) {
        if ((spawn_slot->flags & EFFECT_SLOT_LIVE) && spawn_slot->effect_param == effect_param) {
            func_001A2AF8(spawn_slot);
        }
    }
    for (i = 0; i < 0x180 && D_004468D0 > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) && effect->effect_param == effect_param) {
            func_001A2A18(effect);
        }
    }
}
