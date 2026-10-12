#include "common.h"

void func_00216CF0(void *, void *, void *, void *, void *, float *);

/* Hermite interpolation: hands the four basis weights for the ratio to func_00216CF0. */
void func_00109100(void *result, void *point0, void *point1, void *tangent0, void *tangent1, f32 ratio) {
    f32 weights[4];
    f32 squared = ratio * ratio;
    f32 cubed = squared * ratio;

    weights[0] = cubed + cubed - 3.0f * squared + 1.0f;
    weights[1] = -2.0f * cubed + 3.0f * squared;
    weights[2] = cubed - (squared + squared) + ratio;
    weights[3] = cubed - squared;
    func_00216CF0(result, point0, point1, tangent0, tangent1, weights);
}
