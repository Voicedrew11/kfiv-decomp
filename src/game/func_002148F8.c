#include "common.h"

extern int D_0038CC50[];
extern u32 D_0038CC74[];
extern int D_00446FD8;

void func_002148F8(int *command) {
    if (D_00446FD8 == 0) {
        int *status = D_0038CC50;
        int value;

        status[9] |= 8;
        value = command[2];
        D_00446FD8 = 1;
        status[11] = value;
        return;
    }
    if (!((D_0038CC74[0] >> 3) & 1)) {
        D_00446FD8 = 0;
        *command = 0;
    }
}
