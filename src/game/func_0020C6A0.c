#include "kf_model.h"
#include "kf_vu0.h"

void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);

extern f32 D_00431AF0[16];
extern f32 D_00432620[16];
extern f32 D_004323E0[16];

/* Builds the matrix of a flat model: scaled by scale[0] and scale[1], turned about z by rotation[2], through
   D_00432620, moved to position, then through D_004323E0. */
void func_0020C6A0(ModelInstance *model, f32 *position, f32 *rotation, f32 *scale) {
    f32 *scratch = MODEL_SCRATCH_MATRIX;
    f32 *matrix;
    f32 angle;

    sceVu0CopyMatrix(MODEL_SCRATCH_MATRIX, D_00431AF0);
    matrix = (f32 *)model->unk40;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    angle = rotation[2] + 3.1415927f;
    sceVu0RotMatrixZ(MODEL_SCRATCH_MATRIX, MODEL_SCRATCH_MATRIX, angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f);
    Matrix_Multiply(MODEL_SCRATCH_MATRIX, D_00432620, MODEL_SCRATCH_MATRIX);
    sceVu0TransMatrix(matrix, MODEL_SCRATCH_MATRIX, position);
    Matrix_Multiply(model, D_004323E0, matrix);
}
