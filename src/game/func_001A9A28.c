#include "common.h"
#include "kf_effect.h"

/* Per-frame update: moves the model's E4 value toward the definition's DC value by the step count. */
void func_001A9A28(EffectActor *effect) {
    ProgressMotion *motion;
    EffectDef *def;
    ModelInstance *model;

    motion = (ProgressMotion *)&effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    def = effect->def;
    model = effect->model;
    model->unkE4 = def->unkDC + model->unkE4 * ((float)motion->progress / (float)def->max_level);
    effect->frame += 1;
}
