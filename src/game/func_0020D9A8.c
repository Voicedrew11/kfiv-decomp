#include "common.h"

extern int D_01DFAF80[];

int FlushCache(int);

#define GIF_CTRL (*(volatile u32 *)0x1000A000)
#define GIF_MODE (*(volatile u32 *)0x1000A010)
#define GIF_STAT (*(volatile u32 *)0x1000A020)
#define GIF_TAG0 (*(volatile u32 *)0x1000A030)

void func_0020D9A8(void) {
    u32 *p;

    p = (u32 *)D_01DFAF80;
    *p++ = 0x70000002;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0x8001;
    *p++ = 0x10004000;
    *p++ = 0xE;
    *p++ = 0;
    *p++ = 0xFFFFFFFFU;
    *p++ = 0;
    *p++ = 0x3F;
    *p = 0;
    FlushCache(0);
    while (GIF_CTRL & 0x100);
    GIF_MODE = 0;
    GIF_TAG0 = (u32)D_01DFAF80;
    GIF_CTRL = 0x105;
}
