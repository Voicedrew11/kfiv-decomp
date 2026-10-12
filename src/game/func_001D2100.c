#include "kf_game.h"

int func_001D23F0(ClockTime, ClockTime);

extern ClockTime D_00446A10;
extern ClockTime D_00446A20;

/* Seconds of wall-clock time between the clock start and the last clock read. */
int func_001D2100(void) {
    return func_001D23F0(D_00446A10, D_00446A20);
}
