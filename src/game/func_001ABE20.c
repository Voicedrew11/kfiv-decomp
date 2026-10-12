#include "kf_effect.h"

void func_001AF298(void *, void *);
void func_00216778(void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Moves the model by its velocity scaled up each frame, then advances the frame counter. */
void func_001ABE20(EffectActor *effect) {
    GrowingMotion *motion;
    Vector4 offset;

    motion = (GrowingMotion *)&effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        func_00216778(&offset, motion);
        if (effect->def->scale_per_frame != 0.0f) {
            motion->scale += effect->def->scale_per_frame;
            offset.x *= motion->scale;
            offset.z *= motion->scale;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &offset);
    }
    func_001AF298(effect, motion->unk14);
    effect->frame += 1;
}
