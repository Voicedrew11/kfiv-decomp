#include "common.h"

extern int D_0038CC50[];
extern u32 D_0038CC74[];
extern int D_00446FE0;

void func_002149A0(int *command) {
    if (D_00446FE0 == 0) {
        int *status = D_0038CC50;

        D_00446FE0 = 1;
        status[9] |= 0x20;
        return;
    }
    if (!((D_0038CC74[0] >> 5) & 1)) {
        D_00446FE0 = 0;
        *command = 0;
    }
}
