#include "kf_model.h"

void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void func_00216B48(void *, void *, void *);
void *memset(void *, int, u32);

extern f32 D_00431AF0[16];
extern f32 D_004322E0[16];
extern f32 D_004323E0[16];
extern f32 D_00432620[16];

/* Brings an angle back into -pi..pi. */
static inline f32 KF_Angle_Wrap(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

#define SCRATCH_MATRIX ((f32 *)0x70000000)

/* Builds a model's matrix from a scale, a rotation (z, y, x) and a position, then applies D_004323E0. */
void func_0020C178(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431AF0);
    matrix = (f32 *)model->unk40;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] *= scale[2];
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->z));
    sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->y));
    sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->x));
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &position->x);
    func_00216B48(model, D_004323E0, matrix);
}
