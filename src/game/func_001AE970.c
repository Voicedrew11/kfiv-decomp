#include "common.h"
#include "kf_effect.h"

extern u8 D_0034A019[];

void KF_Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);
void *memset(void *, int, u32);

void func_001AE970(EffectActor *effect) {
    Vector4 ground;
    Vector4 to_ground;
    EffectMotion *motion;

    motion = &effect->motion;
    memset(&ground, 0, 16);
    ground.y = -270.0f;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (D_0034A019[0] != 0 && effect->def->radius_half < effect->frame) {
            KF_Vector_Subtract(&to_ground, &ground, &effect->model->position);
            func_001AFC88(&motion->velocity, &to_ground, effect->def->speed, effect->def->unkDC);
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
    }
    effect->frame += 1;
}
