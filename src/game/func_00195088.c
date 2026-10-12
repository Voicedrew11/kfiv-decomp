#include "common.h"

/* Moves *current one step toward target (snapping on arrival) and returns the distance left. */
float func_00195088(float *current, float target, float step) {
    float diff;
    int steps;

    if (*current == target) {
        return 0.0f;
    }
    diff = target - *current;
    if (step == 0.0f) {
        return diff;
    }
    steps = (int)(diff / step);
    if (steps > 0) {
        *current = *current + step;
    } else if (steps < 0) {
        *current = *current - step;
    } else {
        *current = target;
    }
    return diff;
}
