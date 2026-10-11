#include "common.h"

float func_001957B8(float, float);
float func_00195388(float *, float, float);

void func_00147A20(char *arg0, int arg1) {
    char *s;
    char *q;
    float a;
    float r;

    s = arg0 + 0xE0;
    a = (float)*(s16 *)(*(char **)(arg0 + 0xC) + 0x436);
    q = (char *)(*(int *)(*(char **)arg0 + 0xF8) + 0x480);
    if (*(u16 *)(arg0 + 0x28) != 0) {
        a = a + a;
    }
    r = func_001957B8(*(float *)(*(char **)arg0 + 0xD4), *(float *)(arg0 + 0x68));
    if (1.5707964f < r) {
        r = 1.5707964f;
    }
    if (r < -1.5707964f) {
        r = -1.5707964f;
    }
    if (arg1 == 1) {
        func_00195388((float *)(s + 0x48), r, a);
        *(float *)(q + 0x54) = *(float *)(s + 0x48);
    } else if (arg1 == 2) {
        *(float *)(q + 0x54) = *(float *)(s + 0x48);
    } else {
        *(int *)(s + 0x48) = 0;
    }
}
