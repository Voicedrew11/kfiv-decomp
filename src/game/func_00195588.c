#include "common.h"
#include "kf_model.h"

float sinf(float);
float cosf(float);

/* Build the vector (-sin(angle), 0, -cos(angle), 1). */
void func_00195588(Vector4 *vec, float angle) {
    vec->x = -sinf(angle);
    vec->y = 0.0f;
    vec->z = -cosf(angle);
    vec->w = 1.0f;
}
