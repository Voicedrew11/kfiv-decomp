#include "common.h"

extern int D_005DDDC0[];

int FlushCache(int);
void sceGsSyncPath(int, int);

#define GIF_CTRL (*(volatile u32 *)0x1000A000)
#define GIF_MODE (*(volatile u32 *)0x1000A010)
#define GIF_STAT (*(volatile u32 *)0x1000A020)
#define GIF_TAG0 (*(volatile u32 *)0x1000A030)

void func_00207320(int arg0, s64 arg1) {
    int *p;

    p = D_005DDDC0;
    *p++ = 0x8001;
    *p++ = 0x10004000;
    *p++ = 0xE;
    *p++ = 0;
    *(s64 *)p = arg1;
    p += 2;
    *p++ = arg0;
    *p = 0;
    FlushCache(0);
    p++;
    while (GIF_CTRL & 0x100);
    GIF_STAT = (p - D_005DDDC0) / 4;
    GIF_MODE = (u32)D_005DDDC0 & 0x7FFFFFFF;
    GIF_TAG0 = 0;
    GIF_CTRL = 0x105;
    sceGsSyncPath(0, 0);
}
