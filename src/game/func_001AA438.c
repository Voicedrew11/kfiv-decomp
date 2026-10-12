#include "kf_effect.h"

void func_001AA3E0(EffectActor *, EffectSlot *);

/* Turning effect like func_001AA3E0 that also sets flag bit 0x8000 << 43. */
void func_001AA438(EffectActor *effect, EffectSlot *spawn_slot) {
    func_001AA3E0(effect, spawn_slot);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
