#include "kf_effect.h"

void func_001A9E88(EffectActor *effect, EffectSlot *spawn_slot);

/* Turning effect that copies the definition's unkF2 into its unk20 in modes 3 and 5. */
void func_001A9F40(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    int mode;

    motion = (TurningMotion *)&effect->motion;
    func_001A9E88(effect, spawn_slot);
    mode = *(int *)0x70000018;
    if (mode == 3 || mode == 5) {
        motion->unk20 = effect->def->unkF2;
    }
}
