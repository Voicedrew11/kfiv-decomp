#include "common.h"
#include "kf_effect.h"

extern s64 D_0043EF60[2];
s16 func_0019AA28(float, void *, void *);

/* Spawns the effect: stores a value from the model and the constant D_0043EF60. */
void func_001AD730(EffectActor *effect, EffectSlot *spawn_slot) {
    s64 constant_copy[2];

    constant_copy[0] = D_0043EF60[0];
    constant_copy[1] = D_0043EF60[1];
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    EFFECT_FLAG_PARTS(effect)->unk44 = func_0019AA28(0.0f, &effect->model->position, constant_copy);
}
