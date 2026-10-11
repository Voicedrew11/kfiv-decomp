#include "common.h"

extern int D_0044736C;
extern int D_00447358;
extern int D_00447360;
extern int D_0038CC80[3];

void func_00102228(void);

/* Counts down the end delay, then releases the splash and closes its file. */
void func_002084F8(void) {
    D_0044736C -= 1;
    if (D_0044736C < 0) {
        D_0038CC80[0] = 0;
        D_00447358 = 0;
        if (D_00447360 != 0) {
            func_00102228();
            D_00447360 = 0;
        }
    }
}
