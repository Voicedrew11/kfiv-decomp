#include "kf_effect.h"

/* Starts an effect whose int at motion+0x10 starts at zero. */
void func_001AE570(EffectActor *effect, EffectSlot *spawn_slot) {
    ResetMotion *motion = (ResetMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->unk10 = 0;
}
