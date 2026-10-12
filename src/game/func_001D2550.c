#include "kf_game.h"

/* A play time (seconds, minutes, hours, and hundreds of hours in mday) as seconds. */
int func_001D2550(ClockTime play_time) {
    return play_time.sec + (play_time.min + (play_time.hour + play_time.mday * 99) * 60) * 60;
}
