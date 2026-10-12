#include "kf_model.h"
#include "kf_game.h"

void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void func_00216B48(void *, void *, void *);
float sinf(float);

extern f32 D_00431AF0[16];
extern f32 D_004322A0[16];
extern CameraState D_004325A0;
extern Vector4Arg D_00441490;

#define SCRATCH_MATRIX ((f32 *)0x70000000)

/* Brings an angle back into -pi..pi. */
static inline f32 KF_Angle_Wrap(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Builds the matrix of a flat, mirrored model that turns about z and rises with the camera's pitch,
   then applies D_004322A0. */
void func_0020C7A8(ModelInstance *model, Vector4 *rotation, f32 *scale) {
    Vector4Arg offset = D_00441490;
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431AF0);
    matrix = (f32 *)model->unk40;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] = -1.0f;
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->z));
    offset.y += offset.z * sinf(D_004325A0.unk10);
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &offset.x);
    func_00216B48(model, D_004322A0, matrix);
}
