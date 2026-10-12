#include "common.h"
#include "kf_effect.h"



/* Sets flag bit 58 on the effect after the common update. */
void func_001A3EE8(EffectActor *effect, EffectSlot *spawn_slot) {
    func_001A3E30(effect, spawn_slot);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
