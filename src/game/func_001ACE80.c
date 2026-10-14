#include "common.h"
#include "kf_effect.h"

void func_001AF298(void *, void *);
void Vector_Copy(void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Holds its start pose, then flies off along the anchor model's heading. */
void func_001ACE80(EffectActor *effect) {
    AttachedMotion *motion;

    func_001AEC48(effect);
    motion = (AttachedMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->unkF2) {
        Vector_Copy(&effect->model->position, effect->start_position);
        Vector_Copy(&effect->model->rotation, effect->start_rotation);
    } else if (effect->def->unkF2 == effect->frame) {
        func_001AF538(-effect->def->speed, &motion->velocity, &(*motion->anchor)->rotation);
    } else {
        if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
            sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        }
        func_001AF298(effect, motion->unk14);
    }
    effect->frame++;
}
