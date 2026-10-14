#include "kf_effect.h"

typedef float Mat[4][4] __attribute__((aligned(16)));

extern Mat D_00431AF0;

void Vector_Copy(void *, void *);
void sceVu0RotMatrixZ(Mat, Mat, float);
void sceVu0ApplyMatrix(void *, void *, void *);

/* Spins the heading, drifts the model along the rotated x axis, and copies the target's position. */
void func_001AC2C0(EffectActor *effect) {
    HeadingAngleMotion *motion;
    DriftModel *model;
    Mat rotation_matrix;
    Vector4 forward;
    f32 heading;
    f32 zero;

    motion = (HeadingAngleMotion *)&effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    zero = 0.0f;
    forward.x = 1.0f;
    forward.y = forward.z = forward.w = zero;
    heading = motion->heading + effect->def->unkDC;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    motion->heading = heading;
    sceVu0RotMatrixZ(rotation_matrix, D_00431AF0, heading);
    sceVu0ApplyMatrix(&forward.x, rotation_matrix, &forward.x);
    model = (DriftModel *)effect->model;
    model->drift_x += effect->def->unkE0 * forward.x;
    model->drift_y += effect->def->unkE0 * forward.y;
    Vector_Copy(&effect->model->position, &effect->target->model->position);
    effect->frame += 1;
}
