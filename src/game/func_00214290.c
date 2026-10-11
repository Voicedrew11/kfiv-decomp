#include "common.h"

extern char D_0038CD20[];
extern int D_00446770;

void func_00214290(int *arg0) {
    u32 idx;
    char *row;
    char *p;
    float *fp;
    float f7;
    float f4;
    float f6;
    float f2;
    float f3;
    int a;
    int b;
    int c;

    row = D_0038CD20 + D_00446770 * 0x60;
    idx = arg0[2];
    f7 = ((float *)arg0)[4];
    f4 = ((float *)arg0)[6];
    f6 = ((float *)arg0)[8];
    f2 = ((float *)arg0)[10];
    a = arg0[12];
    b = arg0[14];
    c = arg0[16];
    if (idx < 0x20) {
        f3 = f2 - f6;
        p = (char *)((idx << 5) + ((int *)row)[3]);
        *(s64 *)(p + 0x10) = (s64)((c << 16) + (b << 8) + a);
        fp = (float *)p;
        fp[0] = fp[1] = 0.0f;
        *(float *)(p + 0xC) = f2 * f6 * (f4 - f7) / f3;
        *(float *)(p + 0x8) = ((f7 - f4) * (f2 + f6) / f3 + (f7 + f4)) * 0.5f;
    }
    arg0[0] = 0;
}
