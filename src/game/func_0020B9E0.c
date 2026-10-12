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

/* Builds a part's rotation matrix (x, z, y), and adds its offset when with_offset is set. */
void func_0020B9E0(ModelInstance *model, ModelPart *part, f32 *matrix, int with_offset) {
    sceVu0RotMatrixX(matrix, D_00431AF0, KF_Angle_Wrap(part->unk50));
    sceVu0RotMatrixZ(matrix, matrix, KF_Angle_Wrap(part->unk58));
    sceVu0RotMatrixY(matrix, matrix, KF_Angle_Wrap(part->unk54));
    if (with_offset) {
        sceVu0TransMatrix(matrix, matrix, part->unk40);
    }
}
