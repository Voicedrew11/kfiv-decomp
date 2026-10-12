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

/* Builds a model's matrix from its scale, rotation and position, then applies D_004323E0 to a copy. */
void func_0020BE70(ModelInstance *model) {
    f32 work[16];
    f32 *matrix;

    sceVu0CopyMatrix(work, D_00431AF0);
    matrix = (f32 *)model->unk40;
    work[0] *= model->unkE0;
    work[5] *= model->unkE4;
    work[10] *= model->unkE8;
    sceVu0RotMatrixX(matrix, work, KF_Angle_Wrap(model->rotation.x));
    sceVu0RotMatrixY(matrix, matrix, KF_Angle_Wrap(model->rotation.y));
    sceVu0RotMatrixZ(matrix, matrix, KF_Angle_Wrap(model->rotation.z));
    sceVu0TransMatrix(matrix, matrix, &model->position.x);
    sceVu0CopyMatrix(work, matrix);
    work[5] = work[7] = 0.0f;
    work[6] = work[4] = -0.707f;
    func_00216B48(model, D_004323E0, work);
}
