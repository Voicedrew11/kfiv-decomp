#include "common.h"

float sqrtf(float);
void Vector_Subtract(void *, void *, void *);

/* Horizontal (XZ-plane) distance between two points. */
float func_0020CE90(void *origin, void *target) {
    float delta[3];

    Vector_Subtract(delta, origin, target);
    return sqrtf(delta[0] * delta[0] + delta[2] * delta[2]);
}
