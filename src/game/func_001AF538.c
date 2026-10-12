#include "common.h"
#include "kf_effect.h"

void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
extern f32 D_00431AF0[16];

/* Sets out to a vector of the given length along z, turned by angles (x pitch, y heading). */
void func_001AF538(float length, Vector4 *out, Vector4 *angles) {
    f32 matrix[16];

    out->x = out->y = out->w = 0.0f;
    out->z = length;
    sceVu0RotMatrixX(matrix, D_00431AF0, angles->x);
    sceVu0RotMatrixY(matrix, matrix, angles->y);
    sceVu0ApplyMatrix(&out->x, matrix, &out->x);
}
