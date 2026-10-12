#include "common.h"
#include "kf_effect.h"

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
int func_001B4F90(void *);
void func_001A2A18(void *);
int func_001A1E28(int, int, int, int, u16, int);
void sceVu0AddVector(void *, void *, void *);

void func_001A4CE8(EffectActor *effect) {
    ProjectileMotion *motion;
    ModelInstance *model;

    func_001AEC48(effect);
    motion = (ProjectileMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (func_001B4F90(effect) != 0 || motion->end_frame == effect->frame) {
        model = effect->model;
        func_001A1E28(effect->def->end_effect, (int)(&model->position), (int)(&model->rotation), effect->effect_arg, effect->effect_param, (int)effect->owner);
        func_001A2A18(effect);
    }
    effect->frame += 1;
}
