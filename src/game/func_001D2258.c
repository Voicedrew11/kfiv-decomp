#include "kf_game.h"

int func_001D2550(ClockTime);

/* Seconds from one play time to another. */
int func_001D2258(ClockTime start, ClockTime end) {
    int start_seconds = func_001D2550(start);

    return func_001D2550(end) - start_seconds;
}
