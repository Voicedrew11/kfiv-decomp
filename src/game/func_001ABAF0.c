#include "common.h"
#include "kf_effect.h"

extern int D_004468E8;

void func_001AF298(EffectActor *effect, void *unk10_z);
void Vector_Copy(void *dst, void *src);
void sceVu0AddVector(void *out, void *a, void *b);

/* Drifting effect: moves by its velocity, which grows by unk10.y each frame, until def->unkF0. */
void func_001ABAF0(EffectActor *effect) {
    Vector4 velocity;
    EffectMotion *motion = &effect->motion;
    EffectDef *def;
    u16 max_frames;
    float scale;

    D_004468E8 = 0;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    max_frames = effect->def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        Vector_Copy(&velocity, &motion->velocity);
        def = effect->def;
        if (def->scale_per_frame != 0.0f) {
            scale = motion->unk10.y + def->scale_per_frame;
            motion->unk10.y = scale;
            velocity.x *= scale;
            velocity.z *= scale;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &velocity);
    }
    if (effect->def->unkF0 < effect->frame) {
        func_001AF298(effect, &motion->unk10.z);
    }
    effect->frame += 1;
}
