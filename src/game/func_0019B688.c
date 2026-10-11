#include "common.h"

void KF_Vector_Subtract(float *dst, float *a, float *b);

/* Cross product of (second - first) and (third - second), written to out. */
void func_0019B688(float *out, float *first, float *second, float *third) {
    float edge_ab[4];
    float edge_bc[4];

    KF_Vector_Subtract(edge_ab, second, first);
    KF_Vector_Subtract(edge_bc, third, second);
    out[0] = edge_ab[1] * edge_bc[2] - edge_ab[2] * edge_bc[1];
    out[1] = edge_ab[2] * edge_bc[0] - edge_ab[0] * edge_bc[2];
    out[2] = edge_ab[0] * edge_bc[1] - edge_ab[1] * edge_bc[0];
}
