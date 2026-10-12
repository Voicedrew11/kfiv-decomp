#include "common.h"
#include "kf_effect.h"

/* Advances the frame counter and scales the model's float at +0xE8 by scale_per_mille / 1000. */
void func_001ACCA0(EffectActor *effect) {
    ScaleRateMotion *motion;
    ModelInstance *model;

    func_001AEC48(effect);
    motion = (ScaleRateMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    model = effect->model;
    effect->frame += 1;
    model->unkE8 *= motion->scale_per_mille / 1000.0f;
}
