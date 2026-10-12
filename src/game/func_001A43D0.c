#include "common.h"
#include "kf_effect.h"

// Starts an effect with no end target.
void func_001A43D0(EffectActor *effect, EffectSlot *spawn_slot) {
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    effect->target = NULL;
}
