#include "kf_effect.h"

void Vector_Add(void *, void *, void *);

/* Per-frame update of a drifting effect: moves its model along its velocity. */
void func_001ADC48(EffectActor *effect) {
    EffectMotion *motion = &effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    Vector_Add(&effect->model->position, &effect->model->position, motion);
    effect->frame += 1;
}
