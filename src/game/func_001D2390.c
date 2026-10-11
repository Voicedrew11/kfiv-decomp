#include "common.h"

typedef struct {
    char f0;
    u8 sec;
    u8 min;
    u8 hour;
    char f4;
    u8 mday;
    u8 mon;
    u8 year;
} Func1D2390Date;

int mktime(int *);

int func_001D2390(Func1D2390Date arg0) {
    int tm[9];

    tm[0] = arg0.sec;
    tm[1] = arg0.min;
    tm[2] = arg0.hour;
    tm[3] = arg0.mday;
    tm[4] = arg0.mon - 1;
    tm[5] = arg0.year + 0x64;
    mktime(tm);
    return tm[7];
}
