#include "common.h"

/* Wraps an angle in radians into [-pi, pi]. */
float func_001B09A0(float angle) {
    float wrapped;

    wrapped = angle - (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}
