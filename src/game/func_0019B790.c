#include "common.h"

/* Moves *value toward target by at most step; returns 1 once it has arrived. */
int func_0019B790(f32 target, f32 step, f32 *value) {
    if (!(0.0f <= step)) {
        step = -step;
    }
    if (target - 1.1920929e-7f < *value && *value < target + 1.1920929e-7f) {
        *value = target;
        return 1;
    }
    if (*value < target) {
        if (target - *value <= step) {
            *value = target;
            return 1;
        }
        *value += step;
        return 0;
    }
    if (*value - target <= step) {
        *value = target;
        return 1;
    }
    *value -= step;
    return 0;
}
