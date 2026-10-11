#include "common.h"

float atan2f(float, float);

float func_001954A8(float *origin, float *target) {
    float angle;
    float wrapped;

    angle = atan2f(target[0] - origin[0], target[2] - origin[2]);
    wrapped = angle + 3.1415927f;
    wrapped = wrapped - (float)(int)(wrapped / 6.2831855f) * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}
