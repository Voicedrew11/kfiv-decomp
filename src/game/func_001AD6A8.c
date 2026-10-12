#include "kf_effect.h"

void func_0019B1E8(int, void *, float);
void func_00216778(void *, void *);

/* Copies the motion vector into the model position, then the target's model position into the motion. */
void func_001AD6A8(EffectActor *effect) {
    EffectMotion *motion;

    func_001AEC48(effect);
    motion = &effect->motion;
    func_001AF0B0(effect);
    func_0019B1E8(EFFECT_FLAG_PARTS(effect)->unk44, effect, 100.0f * effect->model->unkE8);
    func_00216778(&effect->model->position, motion);
    func_00216778(motion, &effect->target->model->position);
    effect->frame += 1;
}
