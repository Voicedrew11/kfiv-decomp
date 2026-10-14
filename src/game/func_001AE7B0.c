#include "common.h"
#include "kf_effect.h"

extern f32 D_00431AF0[16];

int rand(void);
void sceVu0RotMatrixX(void *, void *, float);
void sceVu0RotMatrixY(void *, void *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
void Vector_Add(void *, void *, void *);

void func_001AE7B0(EffectActor *effect) {
    YawPitchMotion *motion;
    f32 rotation_matrix[16];

    motion = (YawPitchMotion *)&effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    motion->yaw = motion->yaw + ((float)(rand() & 0xFFFF) * 0.000030517578125f - 1.0f) * effect->def->unkDC;
    motion->pitch = motion->pitch + ((float)(rand() & 0xFFFF) * 0.000030517578125f - 1.0f) * effect->def->unkDC;
    if (motion->yaw > 0.15f) {
        motion->yaw = 0.15f;
    }
    if (motion->pitch > 0.15f) {
        motion->pitch = 0.15f;
    }
    if (motion->yaw < -0.15f) {
        motion->yaw = -0.15f;
    }
    if (motion->pitch < -0.15f) {
        motion->pitch = -0.15f;
    }
    sceVu0RotMatrixX(rotation_matrix, D_00431AF0, motion->yaw);
    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, motion->pitch);
    sceVu0ApplyMatrix(motion, rotation_matrix, motion);
    Vector_Add(&effect->model->position, motion->unk10, motion);
}
