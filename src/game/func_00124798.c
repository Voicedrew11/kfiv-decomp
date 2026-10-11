#include "common.h"

extern u8 D_004470A0[8];
extern int D_0044709C;

/* Marks work area `index` free and clears it. */
void func_00124798(int index) {
    u8 bit;
    int work_area;

    bit = 1 << (index % 8);
    D_004470A0[index / 8] &= ~bit;
    work_area = D_0044709C + index * 0x2200;
    memset((void *)work_area, 0, 0x2200);
}
