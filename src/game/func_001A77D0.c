#include "kf_effect.h"

void Vector_Add(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void sceVu0AddVector(void *, void *, void *);

/* Drifts an effect toward its end position, then advances its frame counter. */
void func_001A77D0(EffectActor *effect) {
    DriftMotion *motion;
    EffectDef *def;

    func_001AEC48(effect);
    motion = (DriftMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    def = effect->def;
    if (effect->frame < def->max_frames || def->max_frames == 0) {
        motion->unk20 += def->scale_per_frame;
        Vector_Add(&motion->velocity, &motion->end_position, &motion->velocity);
        Vector_Scale(&motion->velocity.x, &motion->velocity.x, motion->unk20);
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    effect->frame += 1;
}
