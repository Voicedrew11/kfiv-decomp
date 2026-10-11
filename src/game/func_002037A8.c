#include "common.h"

void func_00216CF0(void *, void *, void *, void *, void *, float *);

/* Blends two points and their tangents with the cubic Hermite weights for phase. */
void func_002037A8(float phase, void *out, void *start, void *end, void *start_tangent, void *end_tangent) {
    float weights[4];
    float t_sq = phase * phase;
    float t_cube = t_sq * phase;

    weights[0] = t_cube + t_cube - 3.0f * t_sq + 1.0f;
    weights[1] = t_cube * -2.0f + 3.0f * t_sq;
    weights[2] = t_cube - 2.0f * t_sq + phase;
    weights[3] = t_cube - t_sq;
    func_00216CF0(out, start, end, start_tangent, end_tangent, weights);
}
