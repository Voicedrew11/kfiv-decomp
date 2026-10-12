#include "common.h"

/* Move current toward target by step, snapping onto target once less than one step away. */
f32 func_00195018(f32 current, f32 target, f32 step) {
    s32 steps;

    if (current == target) {
        return target;
    }
    if (step == 0.0f) {
        return current;
    }
    steps = (s32)((target - current) / step);
    if (steps > 0) {
        current += step;
    } else if (steps < 0) {
        current -= step;
    } else {
        current = target;
    }
    return current;
}
