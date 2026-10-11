#include "common.h"

extern u8 D_004470A0[8];
extern int D_0044709C;

/* Marks work area `index` in use, clears it, and returns its address. */
int func_00124720(int index) {
    int work_area;

    D_004470A0[index / 8] |= 1 << (index % 8);
    work_area = D_0044709C + index * 0x2200;
    memset((void *)work_area, 0, 0x2200);
    return work_area;
}
