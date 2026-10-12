#include "common.h"

/* Turns *angle toward target by one step; returns the signed difference to the target (radians). */
f32 func_00195258(f32 *angle, f32 target, f32 step) {
    f32 old = *angle;
    f32 diff;
    f32 wrapped;
    f32 next;
    s32 steps;

    if (old == target) {
        return 0.0f;
    }
    diff = target - old;
    if (step == 0.0f) {
        return diff;
    }
    wrapped = diff;
    wrapped -= (f32)(s32)(wrapped / 6.2831855f) * 6.2831855f;
    if (wrapped > 3.1415927f) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    steps = (s32)(wrapped / step);
    if (steps > 0) {
        *angle = old + step;
    } else if (steps < 0) {
        *angle = old - step;
    } else {
        *angle = target;
    }
    next = *angle;
    next -= (f32)(s32)(next / 6.2831855f) * 6.2831855f;
    if (next > 3.1415927f) {
        next -= 6.2831855f;
    } else if (next < -3.1415927f) {
        next += 6.2831855f;
    }
    *angle = next;
    return wrapped;
}
