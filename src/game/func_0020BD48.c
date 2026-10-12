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

/* Builds a matrix from the model's x and z angles, pushed 16 units along z, and applies D_004322E0 to it. */
void func_0020BD48(ModelInstance *model) {
    f32 matrix[16];
    f32 offset[4];

    memset(offset, 0, sizeof(offset));
    offset[2] = 16.0f;
    sceVu0RotMatrixX(matrix, D_00431AF0, KF_Angle_Wrap(model->rotation.x));
    sceVu0RotMatrixZ(matrix, matrix, KF_Angle_Wrap(model->rotation.z));
    matrix[4] = matrix[5] = matrix[6] = matrix[7] = 0.0f;
    sceVu0RotMatrixX(matrix, matrix, -1.5707964f);
    sceVu0TransMatrix(matrix, matrix, offset);
    func_00216B48(model, D_004322E0, matrix);
}
