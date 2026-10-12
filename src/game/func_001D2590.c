#include "kf_game.h"

/* A play time as minutes. */
int func_001D2590(ClockTime play_time) {
    return play_time.min + (play_time.hour + play_time.mday * 99) * 60;
}
