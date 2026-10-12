#include "kf_game.h"

int func_001D25C8(ClockTime *);

extern ClockTime D_00446A20;

/* Read the wall clock into D_00446A20; gives sceCdReadClock's result. */
int func_001D2298(void) {
    return func_001D25C8(&D_00446A20);
}
