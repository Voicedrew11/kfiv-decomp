#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern EffectDef D_00502260[];
extern char D_00413F80[];
extern int D_004468CC;
extern int D_004468D0;

/* Frees the player's spawner slots and effects of the kinds 0x280, 0x282 and 0x284. */
void func_001A35D0(void) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = D_003F1750;
    effect = D_003F3550;
    for (i = 0; i < 0x60 && D_004468CC > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) &&
            (EFFECT_SLOT_FLAG_PARTS(slot)->def == &D_00502260[0x280] || EFFECT_SLOT_FLAG_PARTS(slot)->def == &D_00502260[0x282] ||
             EFFECT_SLOT_FLAG_PARTS(slot)->def == &D_00502260[0x284]) &&
            slot->owner_entity == (Entity *)D_00413F80) {
            func_001A2AF8(slot);
        }
    }
    for (i = 0; i < 0x180 && D_004468D0 > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) &&
            (effect->def == &D_00502260[0x280] || effect->def == &D_00502260[0x282] || effect->def == &D_00502260[0x284]) &&
            effect->owner_entity == (Entity *)D_00413F80) {
            func_001A2A18(effect);
        }
    }
}
