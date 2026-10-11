#include "common.h"

/* Wraps the difference angle_to - angle_from into [-PI, PI). */
float func_001957B8(float angle_from, float angle_to) {
    float diff = angle_to - angle_from;

    diff -= (int)(diff / 6.2831855f) * 6.2831855f;
    if (diff > 3.1415927f) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    return diff;
}
