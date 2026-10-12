#include "kf_game.h"

int func_001D2390(ClockTime);

int func_001D24A8(ClockTime start, ClockTime end)
{
    int start_day;
    int end_day;
    char buf[0x30]; /* unused, but retail reserves the 0x30 bytes */

    start_day = func_001D2390(start);
    end_day = func_001D2390(end);
    if (start.year != end.year) {
        int year = start.year;

        if (year & 3) {
            end_day += (end.year - year) * 365;
        } else {
            end_day += (end.year - year) * 366;
        }
    }
    return (end.min - start.min) + ((end.hour - start.hour) + (end_day - start_day) * 24) * 60;
}
