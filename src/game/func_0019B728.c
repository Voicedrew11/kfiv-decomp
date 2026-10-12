#include "common.h"

/* Moves *value toward target by |step|; returns 1 once it reaches target. */
int func_0019B728(int *value, int target, int step) {
    int current = *value;

    step = ABS(step);
    if (current == target) {
        return TRUE;
    }
    if (current < target) {
        if (target - current <= step) {
            *value = target;
            return TRUE;
        }
        *value = current + step;
        return FALSE;
    } else {
        if (current - target <= step) {
            *value = target;
            return TRUE;
        }
        *value = current - step;
        return FALSE;
    }
}
