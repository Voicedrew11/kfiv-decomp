#include "common.h"

extern s32 D_0038CC50[];
extern u32 D_0038CC74[];
extern s32 D_00446FD4;

void func_002148A8(s32 *command) {
    if (D_00446FD4 == 0) {
        D_00446FD4 = 1;
        D_0038CC50[9] |= 4;
        return;
    }
    if (!((D_0038CC74[0] >> 2) & 1)) {
        D_00446FD4 = 0;
        *command = 0;
    }
}
