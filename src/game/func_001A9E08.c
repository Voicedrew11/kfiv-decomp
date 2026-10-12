#include "kf_effect.h"

void sceVu0AddVector(void *, void *, void *);

/* Per-frame update: moves the model by its velocity until the definition's frame limit. */
void func_001A9E08(EffectActor *effect) {
    EffectMotion *motion;

    motion = &effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    effect->frame += 1;
}
