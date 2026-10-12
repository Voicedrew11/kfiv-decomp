#include "kf_game.h"

int func_001D2590(ClockTime);

extern ClockTime D_00446A28;

/* Minutes from the given play time to the current play time. */
int func_001D21B8(ClockTime play_time) {
    return func_001D2590(D_00446A28) - func_001D2590(play_time);
}
