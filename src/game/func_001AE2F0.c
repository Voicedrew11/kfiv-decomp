#include "kf_effect.h"

void Vector_Add(void *, void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
int rand(void);
extern float D_00431AF0[16];

/* Per-frame update of a tumbling effect: turns its velocity by a random angle, bounces it off the floor. */
void func_001AE2F0(EffectActor *effect) {
    EffectMotion *motion = &effect->motion;
    float matrix[16];
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    sceVu0RotMatrixX(matrix, D_00431AF0, ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkDC);
    sceVu0RotMatrixY(matrix, matrix, ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkDC);
    sceVu0ApplyMatrix(&motion->velocity.x, matrix, &motion->velocity.x);
    if (effect->def->unkE0 < effect->model->position.y && 0.0f < motion->velocity.y) {
        motion->velocity.y = -motion->velocity.y;
    }
    Vector_Add(&effect->model->position, &effect->model->position, motion);
    effect->frame += 1;
}
