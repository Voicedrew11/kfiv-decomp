#include "kf_effect.h"

void Vector_Add(void *, void *, void *);
void Vector_Copy(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
extern float D_00431AF0[16];

/* Per-frame update of a spinning effect: turns its model by the spin angle. */
void func_001AE118(EffectActor *effect) {
    SpinAngleMotion *motion = (SpinAngleMotion *)&effect->motion;
    EffectDef *def;
    float spin_vector[4];
    float matrix[16];
    float angle;
    float spin_speed;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    def = effect->def;
    spin_speed = motion->unk24 + def->unkE0;
    spin_vector[1] = 0.0f;
    spin_vector[2] = 0.0f;
    spin_vector[3] = 0.0f;
    spin_vector[0] = motion->unk24 = spin_speed;
    angle = motion->unk20 + def->unkDC;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    motion->unk20 = angle;
    sceVu0RotMatrixZ(matrix, D_00431AF0, angle);
    sceVu0RotMatrixX(matrix, matrix, effect->model->rotation.x);
    sceVu0RotMatrixY(matrix, matrix, effect->model->rotation.y);
    sceVu0ApplyMatrix(spin_vector, matrix, spin_vector);
    Vector_Add(&motion->unk10, &motion->unk10, motion);
    Vector_Add(&effect->model->position, &motion->unk10, spin_vector);
    effect->frame += 1;
}
