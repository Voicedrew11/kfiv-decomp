#include "common.h"
#include "kf_effect.h"

int rand(void);

/* Starts an effect and stores a random factor in 1 +- def->unkDC in motion.unk10.z. */
void func_001A4078(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    func_001A3E30(effect, spawn_slot);
    motion = &effect->motion;
    motion->unk10.z = ((float)(rand() & 0xFFFF) * 0.000030517578f - 1.0f) * effect->def->unkDC + 1.0f;
}
