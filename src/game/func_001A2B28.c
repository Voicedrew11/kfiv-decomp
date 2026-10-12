#include "common.h"
#include "kf_effect.h"

extern int D_004468D0;
extern EffectActor D_003F3550[0x180];


/* Frees every effect that targets the given effect. */
void func_001A2B28(EffectActor *target) {
    EffectActor *effect;
    int i;

    if (D_004468D0 > 0) {
        effect = D_003F3550;
        for (i = 0; i < 0x180; i++, effect++) {
            if (effect->flags & EFFECT_FLAG_LIVE) {
                if (effect->target == target) {
                    func_001A2A18(effect);
                    if (D_004468D0 == 0) {
                        break;
                    }
                }
            }
        }
    }
}
