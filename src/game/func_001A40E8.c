#include "common.h"
#include "kf_effect.h"

void func_001A3F20(EffectActor *effect);
void Vector_Scale(void *, void *, float);

/* Scales the model's unkE0 vector by the follower's scale factor. */
void func_001A40E8(EffectActor *effect) {
    FollowerMotion *motion;
    ModelInstance *model;

    motion = (FollowerMotion *)&effect->motion;
    func_001A3F20(effect);
    model = effect->model;
    Vector_Scale(&model->unkE0, &model->unkE0, motion->unk18);
}
