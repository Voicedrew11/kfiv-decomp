#include "common.h"
#include "kf_effect.h"

/* Advances the frame counter and scales the model's unkE0 and unkE4 by 10 * rotation.x. */
void func_001ADED0(EffectActor *effect) {
    ModelInstance *model;

    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    model = effect->model;
    effect->frame++;
    model->unkE0 = model->unkE0 * model->rotation.x * 10.0f;
    model->unkE4 = model->unkE4 * model->rotation.x * 10.0f;
}
