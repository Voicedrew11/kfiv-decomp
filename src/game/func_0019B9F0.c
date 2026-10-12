#include "common.h"

float func_0019B838(float);

/* Ease an angle toward target over frames steps along the shortest way round; returns 1 once it has arrived. */
int func_0019B9F0(float target, float *angle, int frames) {
    float diff;

    *angle = func_0019B838(*angle);
    target = func_0019B838(target);
    if (*angle == target) {
        return 1;
    }
    if (frames < 2) {
        *angle = target;
        return 1;
    }
    diff = target - *angle;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    *angle = *angle + diff / frames;
    *angle = func_0019B838(*angle);
    return 0;
}
