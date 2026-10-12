#include "kf_game.h"

int mktime(int *);

int func_001D2390(ClockTime time) {
    int tm_fields[9];

    tm_fields[0] = time.sec;
    tm_fields[1] = time.min;
    tm_fields[2] = time.hour;
    tm_fields[3] = time.mday;
    tm_fields[4] = time.mon - 1;
    tm_fields[5] = time.year + 0x64;
    mktime(tm_fields);
    return tm_fields[7];
}
