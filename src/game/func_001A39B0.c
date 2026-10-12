#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern EffectDef D_00502260[];
extern int D_004468CC;
extern int D_004468D0;


/* Returns 1 if a live spawner or effect uses an EffectDef from first_def to last_def. */
int func_001A39B0(int first_def, int last_def) {
    EffectDef *first;
    EffectDef *last;
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    if (last_def < first_def || first_def > 0 || last_def > 0) {
        return 0;
    }
    first = &D_00502260[first_def];
    last = &D_00502260[last_def];
    slot = D_003F1750;
    for (i = 0; i < 0x60 && D_004468CC > 0; i++, slot++) {
        if (slot->flags & EFFECT_SLOT_LIVE) {
            if (EFFECT_SLOT_FLAG_PARTS(slot)->def >= first && EFFECT_SLOT_FLAG_PARTS(slot)->def <= last) {
                return 1;
            }
        }
    }
    effect = D_003F3550;
    for (i = 0; i < 0x180 && D_004468D0 > 0; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            if (effect->def >= first && effect->def <= last) {
                return 1;
            }
        }
    }
    return 0;
}
