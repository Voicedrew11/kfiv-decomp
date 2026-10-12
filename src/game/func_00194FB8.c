#include "common.h"

/* Wraps an angle in radians into the range [-pi, pi]. */
float func_00194FB8(float radians) {
    int turns;
    float wrapped;

    turns = radians / 6.2831855f;
    wrapped = radians - (float)turns * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}
