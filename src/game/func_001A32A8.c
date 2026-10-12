#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern int D_004468CC;
extern int D_004468D0;

/* Frees every spawner slot and effect that the given entity owns. */
void func_001A32A8(Entity *owner) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = D_003F1750;
    effect = D_003F3550;
    for (i = 0; i < 0x60 && D_004468CC > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->owner_entity == owner) {
            func_001A2AF8(slot);
        }
    }
    for (i = 0; i < 0x180 && D_004468D0 > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) && effect->owner_entity == owner) {
            func_001A2A18(effect);
        }
    }
}
