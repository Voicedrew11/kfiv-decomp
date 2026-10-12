#include "kf_effect.h"

/* Common update, then clears the motion's int at 0x10. */
void func_001AC270(EffectActor *effect, EffectSlot *spawn_slot) {
    ResetMotion *motion;

    motion = (ResetMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->unk10 = 0;
}
