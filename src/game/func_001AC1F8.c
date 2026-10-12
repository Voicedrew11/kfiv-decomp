#include "kf_effect.h"

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(char *);

/* Advances the frame counter and scales the model's float at +0xE8 by scale_per_mille / 1000. */
void func_001AC1F8(EffectActor *effect) {
    ScalerMotion *motion;
    ModelInstance *model;

    func_001AEC48(effect);
    motion = (ScalerMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0((char *)effect);
    model = effect->model;
    effect->frame += 1;
    model->unkE8 *= motion->scale_per_mille / 1000.0f;
}
