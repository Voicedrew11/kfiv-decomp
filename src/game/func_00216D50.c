#include "common.h"
#include <eeregs.h>

int AddIntcHandler(int, void *, int);
int EnableIntc(int);

/* Programs one of the four EE timers (mode and compare value); timers 0 and 1 also get an interrupt handler.
   Nothing calls it. */
int func_00216D50(u32 timer, int mode, int compare, void *handler) {
    if (timer >= 4) {
        return -1;
    }
    // match: the timers are 0x800 bytes apart, this steps 0x2000, so only timer 0 is reached
    T0_MODE[timer * 0x800] = mode;
    T0_MODE[timer * 0x800 + 4] = compare; // match: same wrong step
    if (timer < 2) {
        AddIntcHandler(timer + 9, handler, -1);
        EnableIntc(timer + 9);
    }
    return 0;
}
