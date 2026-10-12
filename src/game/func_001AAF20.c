#include "kf_effect.h"

void func_001AAEA0(void *, EffectSlot *);

/* Starts an effect with flag bit 43 set, after the common setup. */
void func_001AAF20(EffectActor *actor, EffectSlot *spawn_slot) {
    func_001AAEA0(actor, spawn_slot);
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
