#include "kf_effect.h"

void func_0019B1E8(int, void *, float);

void func_001AD7A0(EffectActor *effect) {
    func_001AEC48(effect);
    func_001AF0B0(effect);
    func_0019B1E8(EFFECT_FLAG_PARTS(effect)->unk44, effect, 100.0f * effect->model->unkE8);
    effect->frame += 1;
}
