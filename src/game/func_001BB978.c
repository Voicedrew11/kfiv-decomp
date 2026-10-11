#include "common.h"

extern char D_0038CD20[];
extern int D_00446770;
extern char D_00413F80[];
extern int D_00414524[];

void func_001BB978(void) {
    char *s;
    char *row;
    char c;
    int n;
    float f;
    float g;

    row = D_0038CD20 + D_00446770 * 0x60;
    s = D_00413F80;
    c =*(char *)((*(u16 *)(s + 0x252) << 7) + ((int *)row)[5] + 0x4C);
    *(int *)(s + 0x2A4) = c;
    if (c - D_00414524[0] < 0) {
        *(int *)(s + 0x2AC) = -0x30;
    }
    n = *(int *)(s + 0x2AC);
    if (n != 0) {
        g = *(float *)(s + 0x2A8) + (float)n;
        *(int *)(s + 0x2AC) = n / 2;
        *(float *)(s + 0x2A8) = g;
        return;
    }
    f = *(float *)(s + 0x2A8);
    if (f < 0.0f) {
        g = f + 0.5f;
        *(float *)(s + 0x2A8) = g;
        if (g >= 0.0f) {
            *(float *)(s + 0x2A8) = 0.0f;
        }
    } else if (f > 0.0f) {
        g = f - 4.0f;
        *(float *)(s + 0x2A8) = g;
        if (g <= 0.0f) {
            *(float *)(s + 0x2A8) = 0.0f;
        }
    }
}
