#include "kf_effect.h"

void sceVu0AddVector(void *, void *, void *);
void func_001AF5B0(void *, void *);
void func_001AF298(void *, void *);

void func_001AD1B8(EffectActor *effect) {
    FallingLateMotion *motion;

    func_001AEC48(effect);
    motion = (FallingLateMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (effect->def->radius_half < effect->frame) {
            motion->velocity.y += effect->def->gravity;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        func_001AF5B0(&effect->model->rotation, &motion->velocity);
    }
    func_001AF298(effect, motion->unk1C);
    effect->frame++;
}
