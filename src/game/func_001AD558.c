#include "kf_effect.h"

void sceVu0AddVector(void *, void *, void *);
void func_001AF298(void *, void *);
void func_001AD558(EffectActor *effect) {
    EndFrameMotion *motion;

    func_001AEC48(effect);
    motion = (EndFrameMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
    if (motion->done) {
        Effect_Free(effect);
    } else if (effect->frame == motion->end_frame) {
        Effect_Spawn(effect->def->end_effect, &motion->unk10, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        motion->done = 1;
    } else {
        func_001AF298(effect, &motion->done);
    }
    effect->frame++;
}
