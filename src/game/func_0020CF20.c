#include "common.h"

float atan2f(float, float);
float sqrtf(float);
void Vector_Subtract(void *, void *, void *);

/* pitch of a point seen from an origin */
float func_0020CF20(void *point, void *origin) {
    float offset[3];

    Vector_Subtract(offset, point, origin);
    return atan2f(offset[1], sqrtf(offset[0] * offset[0] + offset[2] * offset[2]));
}
