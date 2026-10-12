#include "common.h"

extern int D_005DDDC0[];

int FlushCache(int);
int sceGsSyncPath(int, int);

#define GIF_CTRL (*(volatile u32 *)0x1000A000)
#define GIF_MODE (*(volatile u32 *)0x1000A010)
#define GIF_STAT (*(volatile u32 *)0x1000A020)
#define GIF_TAG0 (*(volatile u32 *)0x1000A030)

s32 func_00207320(int reg_address, s64 reg_value) {
    int *p;

    p = D_005DDDC0;
    *p++ = 0x8001;
    *p++ = 0x10004000;
    *p++ = 0xE;
    *p++ = 0;
    *(s64 *)p = reg_value;
    p += 2;
    *p++ = reg_address;
    *p = 0;
    FlushCache(0);
    p++;
    while (GIF_CTRL & 0x100);
    GIF_STAT = (p - D_005DDDC0) / 4;
    GIF_MODE = (u32)D_005DDDC0 & 0x7FFFFFFF;
    GIF_TAG0 = 0;
    GIF_CTRL = 0x105;
    /* match: retail tail-calls sceGsSyncPath and returns its result; a written return defeats the tail call */
    sceGsSyncPath(0, 0);
}
