#include "kf_effect.h"

/* Starts an effect that has no per-kind state: runs the common start helpers. */
void func_001A9AA8(EffectActor *actor, EffectSlot *spawn_slot) {
    func_001AF640(actor, spawn_slot);
    func_001AF928(actor, spawn_slot);
}
