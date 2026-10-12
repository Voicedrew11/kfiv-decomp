#include "common.h"

float func_0019B838(float);
float func_0019B898(float);

/* Return the angular distance between two angles in units of step, at least 1 unless they are equal. */
int func_0019BAD0(float from, float target, float step) {
    float start, end, forward, backward, count;

    start = func_0019B838(from);
    end = func_0019B838(target);
    if (start == end) {
        return 0;
    }
    forward = func_0019B898(end - start);
    backward = func_0019B898(start - end);
    count = func_0019B838(forward - backward);
    count = (0.0f <= count ? count : -count) / step;
    if (count <= 1.0f) {
        return 1;
    }
    return (int)count;
}
