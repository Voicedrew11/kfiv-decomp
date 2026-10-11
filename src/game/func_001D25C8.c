#include "common.h"

typedef struct {
    u8 stat, second, minute, hour, pad, day, month, year;
} sceCdCLOCK;
int sceCdReadClock(sceCdCLOCK *);
#define BCD2DEC(x) (((x) & 0xF) + ((x) >> 4) * 10)

int func_001D25C8(sceCdCLOCK *clk)
{
    int ret;

    ret = sceCdReadClock(clk);
    clk->second = BCD2DEC(clk->second);
    clk->minute = BCD2DEC(clk->minute);
    clk->hour = BCD2DEC(clk->hour);
    clk->day = BCD2DEC(clk->day);
    clk->month = BCD2DEC(clk->month);
    clk->year = BCD2DEC(clk->year);
    return ret;
}
