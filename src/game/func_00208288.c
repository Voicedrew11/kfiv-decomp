#include "common.h"

extern int D_00447358;
extern int D_0044736C;

/* Aborts the splash image: jumps to the close state (6) with a short end delay. */
void func_00208288(void) {
    if (D_00447358 != 0) {
        D_00447358 = 6;
        D_0044736C = 2;
    }
}
