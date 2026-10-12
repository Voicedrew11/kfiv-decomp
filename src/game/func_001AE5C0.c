#include "kf_effect.h"

void func_00216778(void *, void *);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
int rand(void);
extern float D_00431AF0[16];

/* Per-frame update of a tilting effect: tilts its axis by a random angle and moves its model along the target. */
void func_001AE5C0(EffectActor *effect) {
    TiltMotion *motion = (TiltMotion *)&effect->motion;
    float matrix[16];
    float axis[4];
    float angle;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    axis[0] = 1.0f;
    axis[1] = axis[2] = axis[3] = 0.0f;
    angle = motion->spin_angle + effect->def->unkDC + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkE4;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    motion->spin_angle = angle;
    sceVu0RotMatrixZ(matrix, D_00431AF0, angle);
    sceVu0ApplyMatrix(axis, matrix, axis);
    effect->model->unkE0 = effect->model->unkE0 + effect->def->unkE0 * axis[0];
    ((TiltModel *)effect->model)->unkE4 = ((TiltModel *)effect->model)->unkE4 + effect->def->unkE0 * axis[1];
    func_00216778(&effect->model->position, &effect->target->model->position);
    effect->frame += 1;
}
