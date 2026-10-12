#include "kf_entity.h"

float atan2f(float, float);
float sqrtf(float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);

/* func_00194FB8, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

/* takes the three angles (x, y, z) out of a rotation matrix */
void func_0018C4B8(void *angles_arg, void *matrix) {
    f32 *angles = angles_arg;
    f32 local[16];
    f32 axis[4];
    f32 turned[4];

    sceVu0CopyMatrix(local, matrix);
    local[12] = 0.0f;
    local[13] = 0.0f;
    local[14] = 0.0f;
    local[15] = 1.0f;
    axis[0] = 1.0f;
    axis[1] = 0.0f;
    axis[2] = 0.0f;
    axis[3] = 0.0f;
    func_00216BC0(turned, local, axis);
    angles[1] = -atan2f(turned[2], turned[0]);
    angles[2] = atan2f(turned[1], sqrtf(turned[0] * turned[0] + turned[2] * turned[2]));
    sceVu0RotMatrixY(local, local, -angles[1]);
    sceVu0RotMatrixZ(local, local, -angles[2]);
    axis[2] = 1.0f;
    axis[0] = 0.0f;
    axis[1] = 0.0f;
    axis[3] = 0.0f;
    func_00216BC0(turned, local, axis);
    angles[0] = atan2f(-turned[1], turned[2]);
    if (local[5] < 0.0f) {
        angles[0] = 3.1415927f - angles[0];
    }
    angles[0] = wrap_angle(angles[0]);
    angles[1] = wrap_angle(angles[1]);
    angles[2] = wrap_angle(angles[2]);
}
