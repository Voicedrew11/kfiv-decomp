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

int func_001D2390(Func1D2390Date);

int func_001D24A8(Func1D2390Date arg0, Func1D2390Date arg1)
{
    int d0;
    int d1;
    char buf[0x30]; /* unused, but retail reserves the 0x30 bytes */

    d0 = func_001D2390(arg0);
    d1 = func_001D2390(arg1);
    if (arg0.year != arg1.year) {
        int y = arg0.year;

        if (y & 3) {
            d1 += (arg1.year - y) * 365;
        } else {
            d1 += (arg1.year - y) * 366;
        }
    }
    return (arg1.min - arg0.min) + ((arg1.hour - arg0.hour) + (d1 - d0) * 24) * 60;
}
