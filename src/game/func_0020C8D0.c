#include "common.h"

void sceVu0CopyMatrix(void *, void *);
float sinf(float);
float cosf(float);

extern f32 D_00431AF0[16];

/* Brings an angle back into -pi..pi. */
static inline f32 KF_Angle_Wrap(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Builds the rotation matrix of three angles: a turn by the second, a tilt by the first, a turn by the third. */
void func_0020C8D0(f32 (*matrix)[4], f32 *angles) {
    f32 cos_tilt = cosf(KF_Angle_Wrap(angles[0]));
    f32 sin_tilt = sinf(KF_Angle_Wrap(angles[0]));
    f32 cos_first = cosf(KF_Angle_Wrap(angles[1]));
    f32 sin_first = sinf(KF_Angle_Wrap(angles[1]));
    f32 cos_last = cosf(KF_Angle_Wrap(angles[2]));
    f32 sin_last = sinf(KF_Angle_Wrap(angles[2]));

    sceVu0CopyMatrix(matrix, D_00431AF0);
    // match: the order of these nine statements decides the order of the multiplies
    matrix[2][2] = cos_tilt * cos_first * cos_last - sin_first * sin_last;
    matrix[2][0] = -cos_tilt * cos_first * sin_last - sin_first * cos_last;
    matrix[0][2] = cos_tilt * sin_first * cos_last + cos_first * sin_last;
    matrix[2][1] = sin_tilt * cos_first;
    matrix[0][0] = -cos_tilt * sin_first * sin_last + cos_first * cos_last;
    matrix[0][1] = sin_tilt * sin_first;
    matrix[1][1] = cos_tilt;
    matrix[1][2] = -sin_tilt * cos_last;
    matrix[1][0] = sin_tilt * sin_last;
}
