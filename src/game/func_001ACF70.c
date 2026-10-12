#include "common.h"
#include "kf_effect.h"

/* Starts an effect that waits for its state flag to be set. */
void func_001ACF70(EffectActor *effect, EffectSlot *spawn_slot) {
    StateFlagMotion *motion;

    func_001AF928(effect, spawn_slot);
    motion = (StateFlagMotion *)&effect->motion;
    func_001AF640(effect, spawn_slot);
    motion->state = 0;
}
