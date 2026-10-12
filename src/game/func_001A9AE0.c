#include "kf_effect.h"


extern EffectActor D_003F3550[0x180];

/* Keeps this effect alive while another effect of kind 0x23 is in use; otherwise ends it. */
void func_001A9AE0(EffectActor *effect) {
    EffectActor *candidate;
    int i;

    candidate = D_003F3550;
    for (i = 0; i < 0x180; i++, candidate++) {
        if ((candidate->flags & EFFECT_FLAG_LIVE) && candidate->def->unk100 == 0x23) {
            effect->frame++;
            return;
        }
    }
    func_001A2A18(effect);
}
