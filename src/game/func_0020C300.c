#include "kf_model.h"
#include "kf_game.h"

void sceVu0SubVector(void *, void *, void *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
float atan2f(float, float);
float sqrtf(float);

extern f32 D_00431B30[4][4];
extern CameraState D_004325A0;

#define SCRATCH_MATRIX ((f32 *)0x70000000)

/* Brings an angle back into -pi..pi. */
static inline f32 KF_Angle_Wrap(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Builds a model's matrix from a position, a rotation and a scale. Models flagged 0x100 or 0x200 are
   turned to face the camera (about x, about y) and keep only their own turn about z; models flagged
   0x400 turn about z alone. */
void func_0020C300(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 to_camera[4];
    f32 angle;
    f32 *scratch = SCRATCH_MATRIX;

    if (model->unk102 & 0x300) {
        sceVu0SubVector(to_camera, D_004325A0.position, position);
        sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431B30);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(model->rotation.z));
        if (model->unk102 & 0x100) {
            sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX,
                             atan2f(to_camera[1], sqrtf(to_camera[0] * to_camera[0] + to_camera[2] * to_camera[2])));
        }
        if (model->unk102 & 0x200) {
            angle = -atan2f(to_camera[2], to_camera[0]) - 1.5707964f;
            if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, angle);
        }
        sceVu0TransMatrix((f32 *)model->unk40, SCRATCH_MATRIX, &model->position.x);
    } else if (model->unk102 & 0x400) {
        sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431B30);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->z));
        sceVu0TransMatrix((f32 *)model->unk40, SCRATCH_MATRIX, &position->x);
    } else {
        sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431B30);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->z));
        sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->x));
        sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, KF_Angle_Wrap(rotation->y));
        sceVu0TransMatrix((f32 *)model->unk40, SCRATCH_MATRIX, &position->x);
    }
}
