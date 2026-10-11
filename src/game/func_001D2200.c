#include "common.h"

extern u8 D_00446A21;
extern u8 D_00446A22;
extern u8 D_00446A23;
extern u8 D_00446A25;
extern u8 D_00446A26;
extern u8 D_00446A27;

int mktime(void *);

typedef struct {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} Tm;

int func_001D2200(void) {
    Tm tm;

    tm.tm_sec = D_00446A21;
    tm.tm_min = D_00446A22;
    tm.tm_hour = D_00446A23;
    tm.tm_mday = D_00446A25;
    tm.tm_mon = D_00446A26 - 1;
    tm.tm_year = D_00446A27 + 0x64;
    mktime(&tm);
    return tm.tm_wday;
}
