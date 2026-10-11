#include "common.h"

extern char D_0038CD20[];
extern int D_00446770;

void func_00214480(int *arg0) {
    u32 idx;
    char *p;
    char *row;

    row = D_0038CD20 + D_00446770 * 0x60;
    idx = arg0[2];
    if (idx < 0x20) {
        p = (char *)((idx << 7) + ((int *)row)[4]);
        *(float *)(p + 0x70) = (float)arg0[4] / 255.0f;
        *(float *)(p + 0x74) = (float)arg0[6] / 255.0f;
        *(float *)(p + 0x78) = (float)arg0[8] / 255.0f;
    }
    arg0[0] = 0;
}
