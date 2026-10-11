#include "common.h"

float sinf(float);
float cosf(float);

/* Build the vector (sin(angle), 0, cos(angle), 1). */
void func_00195538(float *vec, float angle) {
    float sin_angle;

    sin_angle = sinf(angle);
    ((int *)vec)[1] = 0;
    vec[0] = sin_angle;
    vec[2] = cosf(angle);
    vec[3] = 1.0f;
}
