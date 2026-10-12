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

/* Builds a model's matrix from its rotation and position; flagged models only turn about z and take D_00432620. */
void func_0020BB18(ModelInstance *model) {
    f32 *matrix;

    if (!(model->unk102 & 0x300)) {
        matrix = (f32 *)model->unk40;
        sceVu0RotMatrixX(matrix, D_00431AF0, KF_Angle_Wrap(model->rotation.x));
        sceVu0RotMatrixY(matrix, matrix, KF_Angle_Wrap(model->rotation.y));
        sceVu0RotMatrixZ(matrix, matrix, KF_Angle_Wrap(model->rotation.z));
        sceVu0TransMatrix(matrix, matrix, &model->position.x);
    } else {
        matrix = (f32 *)model->unk40;
        sceVu0RotMatrixZ(matrix, D_00431AF0, KF_Angle_Wrap(model->rotation.z));
        func_00216B48(matrix, D_00432620, matrix);
        sceVu0TransMatrix(matrix, matrix, &model->position.x);
    }
}
