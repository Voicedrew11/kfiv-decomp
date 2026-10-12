#include "common.h"

/* Turns the angle `angle` toward `target` by at most `step` radians and returns the wrapped result. */
f32 func_00195140(f32 angle, f32 target, f32 step) {
    f32 delta;
    f32 diff;
    int dir;

    if (angle == target) {
        return target;
    }
    if (step == 0.0f) {
        return angle;
    }
    delta = target - angle;
    diff = delta - (f32)(int)(delta / 6.2831855f) * 6.2831855f;
    if (diff > 3.1415927f) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    dir = (int)(diff / step);
    if (dir > 0) {
        angle += step;
    } else if (dir < 0) {
        angle -= step;
    } else {
        angle = target;
    }
    angle -= (f32)(int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}
