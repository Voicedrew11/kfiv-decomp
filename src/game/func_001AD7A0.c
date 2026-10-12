#include "kf_effect.h"

void func_0019B1E8(f32 duration, s32 index, void *actor);

void func_001AD7A0(EffectActor *effect) {
    func_001AEC48(effect);
    func_001AF0B0(effect);
    func_0019B1E8(100.0f * effect->model->unkE8, EFFECT_FLAG_PARTS(effect)->unk44, effect);
    effect->frame += 1;
}
