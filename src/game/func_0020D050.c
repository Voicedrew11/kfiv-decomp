#include "common.h"
#include <eeregs.h>

extern int D_00446F84;

/* Timer 0 overflow handler: counts the overflow and acknowledges it. */
void func_0020D050(void) {
    D_00446F84++;
    *T0_MODE |= 0x400;
}
