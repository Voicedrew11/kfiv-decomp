#include "common.h"

float sqrtf(float);
void KF_Vector_Subtract(void *, void *, void *);

/* Distance between two points. */
float func_0020CE28(void *origin, void *target) {
    float delta[3];

    KF_Vector_Subtract(delta, origin, target);
    return sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
}
