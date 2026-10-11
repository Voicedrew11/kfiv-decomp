#include "common.h"

typedef struct {
    char pad0[0xC0];
    float f0, f1, f2, f3;
    float rx, ry, rz;
} Cam;

void *memset(void *, int, u32);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);

void func_00153F60(Cam **arg0, float *arg1, float *arg2) {
    float m[16];
    float v[4];

    memset(m, 0, 0x40);
    m[15] = m[10] = m[5] = m[0] = 1.0f;
    v[0] = -(*arg0)->f0;
    v[1] = -(*arg0)->f1;
    v[2] = -(*arg0)->f2;
    v[3] = -(*arg0)->f3;
    sceVu0TransMatrix(m, m, v);
    sceVu0RotMatrixY(m, m, -(*arg0)->ry);
    sceVu0RotMatrixZ(m, m, -(*arg0)->rz);
    sceVu0RotMatrixX(m, m, -(*arg0)->rx);
    sceVu0ApplyMatrix(arg1, m, arg2);
}
