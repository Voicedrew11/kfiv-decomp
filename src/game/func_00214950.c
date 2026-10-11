#include "common.h"

extern int D_00446FDC;
extern char D_0038CC50[];
extern u32 D_0038CC74[];

void func_00214950(int *arg0) {
    if (D_00446FDC == 0) {
        int *p = (int *)D_0038CC50;

        D_00446FDC = 1;
        p[9] |= 0x10;
        return;
    }
    if (!((D_0038CC74[0] >> 4) & 1)) {
        D_00446FDC = 0;
        *arg0 = 0;
    }
}
