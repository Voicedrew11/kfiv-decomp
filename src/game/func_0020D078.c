#include "common.h"
#include <eeregs.h>

extern int D_00446F84;

/* Restarts the timer 0 clock. */
void func_0020D078(void) {
    D_00446F84 = 0;
    *T0_COUNT = 0;
}
