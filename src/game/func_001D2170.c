#include "kf_game.h"

int func_001D2550(ClockTime);

extern ClockTime D_00446A28;

/* Seconds from the given play time to the current play time. */
int func_001D2170(ClockTime play_time) {
    return func_001D2550(D_00446A28) - func_001D2550(play_time);
}
