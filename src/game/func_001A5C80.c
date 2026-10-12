#include "common.h"
#include "kf_effect.h"

/* Starts an effect of this kind: the shared setup, then the second setup step. */
void func_001A5C80(EffectActor *effect, EffectSlot *spawn_slot) {
    func_001AF640(effect, spawn_slot);
    func_001AF928(effect, spawn_slot);
}
