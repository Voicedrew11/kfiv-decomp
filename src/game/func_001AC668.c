#include "kf_effect.h"

void func_001AF298(void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Falls under the definition's gravity, moving the model along the velocity. */
void func_001AC668(EffectActor *effect) {
    EffectMotion *motion;
    u8 hit_flag;

    hit_flag = 0;
    func_001AEC48(effect);
    motion = &effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    motion->velocity.y += effect->def->gravity;
    sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
    func_001AF298(effect, &hit_flag);
    effect->frame += 1;
}
