#include "common.h"

void func_00216CF0(void *, void *, void *, void *, void *, float *);

/* Cubic Hermite interpolation: weights the two points and two tangents by
   the Hermite basis at ratio and hands them to func_00216CF0. */
void func_0019A590(void *result, void *point0, void *point1, void *tangent0, void *tangent1, f32 ratio) {
    f32 basis[4];
    f32 squared = ratio * ratio;
    f32 cubed = squared * ratio;

    basis[0] = cubed + cubed - 3.0f * squared + 1.0f;
    basis[1] = cubed * -2.0f + 3.0f * squared;
    basis[2] = cubed - 2.0f * squared + ratio;
    basis[3] = cubed - squared;
    func_00216CF0(result, point0, point1, tangent0, tangent1, basis);
}
