#include "kf_model.h"

void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);

extern f32 D_00431AF0[16];

/* Brings an angle back into -pi..pi. */
static inline f32 KF_Angle_Wrap(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Builds a part's local matrix from its scale, its three angles (applied x, z, y) and its offset. */
void func_0020B8B0(ModelInstance *model, ModelPart *part, ModelPart *dest) {
    f32 *matrix = (f32 *)dest->unk00;

    sceVu0CopyMatrix(matrix, D_00431AF0);
    matrix[0] = part->unk60[0];
    matrix[5] = part->unk60[1];
    matrix[10] = part->unk60[2];
    sceVu0RotMatrixX(matrix, matrix, KF_Angle_Wrap(part->unk50));
    sceVu0RotMatrixZ(matrix, matrix, KF_Angle_Wrap(part->unk58));
    sceVu0RotMatrixY(matrix, matrix, KF_Angle_Wrap(part->unk54));
    sceVu0TransMatrix(matrix, matrix, part->unk40);
}
