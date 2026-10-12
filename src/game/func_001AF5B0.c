#include "kf_model.h"

float atan2f(float, float);
float sqrtf(float);

/* Turns a direction vector into pitch (x) and heading (y) angles. */
void func_001AF5B0(Vector4 *angles, Vector4 *vec) {
    angles->x = atan2f(-vec->y, sqrtf(vec->x * vec->x + vec->z * vec->z));
    angles->y = atan2f(vec->x, vec->z);
}
