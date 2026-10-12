#include "common.h"
#include "kf_effect.h"

void func_001AA330(EffectActor *effect, EffectSlot *spawn_slot);

/* Spawn callback: a turning effect that, in game modes 3 and 5, takes the definition's interval. */
void func_001AA3E0(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    int mode;
    char frame_pad[0x10]; // match: unused, but retail reserves the 0x10 bytes

    motion = (TurningMotion *)&effect->motion;
    func_001AA330(effect, spawn_slot);
    mode = *(int *)0x70000018;
    if (mode == 3 || mode == 5) {
        motion->unk20 = effect->def->interval;
    }
}
