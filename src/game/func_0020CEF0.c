#include "common.h"

float atan2f(float, float);
void Vector_Subtract(void *, void *, void *);

/* Heading (angle in the XZ plane) from target to origin. */
float func_0020CEF0(void *origin, void *target) {
    float delta[3];

    Vector_Subtract(delta, origin, target);
    return atan2f(delta[0], delta[2]);
}
