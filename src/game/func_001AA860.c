#include "common.h"
#include "kf_effect.h"

void func_001AF298(void *, void *);
void func_00216778(void *, void *);

/* Update: runs the three keyed channels, copies the positions across, then advances the frame. */
void func_001AA860(EffectActor *effect) {
    char ended;

    ended = 0;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    func_00216778(&effect->model->position, &effect->motion);
    func_00216778(&effect->motion, &effect->target->model->position);
    func_001AF298(effect, &ended);
    effect->frame += 1;
}
