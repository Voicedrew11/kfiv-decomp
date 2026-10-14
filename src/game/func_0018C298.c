#include "kf_entity.h"

extern f32 D_00431AF0[16];

double sqrt(double);
float atan2f(float, float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);

/* keeps a direction within pitch limits[0]..limits[1] and yaw limits[2]..limits[3] of
   `heading`; TRUE: it had to be changed */
s32 func_0018C298(f32 *direction, f32 *limits, f32 heading) {
    f32 local[4];
    f32 matrix[16];
    f32 pitch;
    f32 yaw;
    f32 length;
    s32 clamped = FALSE;

    sceVu0RotMatrixY(matrix, D_00431AF0, -heading);
    Vector_ApplyMatrix(local, matrix, direction);
    length = sqrt(local[0] * local[0] + local[2] * local[2]);
    pitch = atan2f(-local[1], length);
    yaw = atan2f(local[0], local[2]);
    if (pitch < limits[0]) {
        pitch = limits[0];
        clamped = TRUE;
    }
    if (limits[1] < pitch) {
        pitch = limits[1];
        clamped = TRUE;
    }
    if (yaw < limits[2]) {
        yaw = limits[2];
        clamped = TRUE;
    }
    if (limits[3] < yaw) {
        yaw = limits[3];
        clamped = TRUE;
    }
    if (clamped) {
        sceVu0RotMatrixX(matrix, D_00431AF0, pitch);
        sceVu0RotMatrixY(matrix, matrix, yaw);
        sceVu0RotMatrixY(matrix, matrix, heading);
        length = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
        local[0] = matrix[8];
        local[1] = matrix[9];
        local[2] = matrix[10];
        local[3] = matrix[11];
        Vector_Scale(direction, local, length);
    }
    return clamped;
}
