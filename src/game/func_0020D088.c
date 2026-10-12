#include "common.h"
#include <eeregs.h>

extern int D_00446F84;

/* Timer 0 ticks since the last restart (the counter overflows every 60000). */
int func_0020D088(void) {
    return D_00446F84 * 60000 + *T0_COUNT;
}
