#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern int D_004468CC;
extern int D_004468D0;

/* Frees the spawner slots and effects that have no effect_param and belong to owner. */
void func_001A33A8(Entity *owner) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = D_003F1750;
    effect = D_003F3550;
    i = 0;
    for (; i < 0x60 && D_004468CC > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->effect_param == 0xFFFF && slot->owner_entity == owner) {
            func_001A2AF8(slot);
        }
    }
    i = 0;
    for (; i < 0x180 && D_004468D0 > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) && effect->effect_param == 0xFFFF && effect->owner_entity == owner) {
            func_001A2A18(effect);
        }
    }
}
