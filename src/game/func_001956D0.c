#include "common.h"

float atan2f(float, float);
float sqrtf(float);

/* Converts the direction vector dir to (pitch, yaw) angles in angles. */
void func_001956D0(float *angles, float *dir) {
    float yaw;

    angles[0] = -atan2f(dir[1], sqrtf(dir[0] * dir[0] + dir[2] * dir[2]));
    yaw = -atan2f(dir[2], dir[0]);
    yaw -= (float)(int)(yaw / 6.2831855f) * 6.2831855f;
    if (yaw > 3.1415927f) {
        yaw -= 6.2831855f;
    } else if (yaw < -3.1415927f) {
        yaw += 6.2831855f;
    }
    angles[1] = yaw;
    angles[2] = angles[3] = 0.0f;
}
