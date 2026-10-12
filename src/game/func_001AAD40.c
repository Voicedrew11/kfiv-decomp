#include "kf_effect.h"

void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
extern float D_00431AF0[16];

/* Per-frame update for a spray effect: applies gravity, moves the model and turns it by its spin. */
void func_001AAD40(EffectActor *effect) {
    SprayMotion *motion;
    float matrix[16];
    float offset[4];

    motion = (SprayMotion *)&effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    func_00216778(&effect->model->position, effect->unk64);
    func_00216778(&effect->model->rotation, effect->unk68);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (effect->def->gravity != 0.0f) {
            motion->accel.y += effect->def->gravity;
        }
        sceVu0AddVector(&motion->velocity, &motion->velocity, &motion->accel);
    }
    sceVu0RotMatrixZ(matrix, D_00431AF0, effect->model->rotation.z);
    sceVu0RotMatrixX(matrix, matrix, effect->model->rotation.x);
    sceVu0RotMatrixY(matrix, matrix, effect->model->rotation.y);
    sceVu0ApplyMatrix(offset, matrix, &motion->velocity.x);
    func_00216C48(&effect->model->position, &effect->model->position, offset);
    func_00216C48(&effect->model->rotation, &effect->model->rotation, &motion->spin);
    effect->frame += 1;
    if (effect->def->radius_half != 0 && effect->frame >= effect->def->keyframe_end) {
        effect->frame = 0;
    }
}
