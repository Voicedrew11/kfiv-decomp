#include "kf_effect.h"

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(char *);
void func_00216C48(void *, void *, void *);

/* Advances the frame counter and moves the model by its velocity. */
void func_001ADC48(EffectActor *effect) {
    EffectMotion *motion;

    func_001AEC48(effect);
    motion = &effect->motion;
    func_001AED58(effect);
    func_001AF0B0((char *)effect);
    func_00216C48(&effect->model->position, &effect->model->position, &motion->velocity);
    effect->frame += 1;
}
