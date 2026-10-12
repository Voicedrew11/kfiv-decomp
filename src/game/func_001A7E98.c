#include "common.h"
#include "kf_effect.h"

extern float D_004460A4;
extern float D_004460A8;

/* Spawns an effect that flies along its model's heading and spins at a random rate. */
void func_001A7E98(EffectActor *effect, EffectSlot *spawn_slot) {
    SpinMotion *motion;
    float random_fraction;
    float spin_rate;

    motion = (SpinMotion *)&effect->motion;
    ((void (*)(void))func_001AF928)(); // match: retail calls with no arguments
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
    motion->unk10 = (((float)(rand() & 0xFFFF) * 0.000030517578f) - 1.0f) * 0.5f;
    random_fraction = (float)(rand() & 0xFFFF);
    spin_rate = (float)((((random_fraction * 0.000030517578f) - 1.0f) * D_004460A4) + D_004460A8);
    motion->unk18 = 1.0f;
    motion->unk1D = 0;
    motion->spin_rate = spin_rate;
}
