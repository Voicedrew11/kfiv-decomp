#include "common.h"

extern char *D_004471D4;

typedef struct {
    char b0;
    u8 b1;
    u8 b2;
    char b3;
} Slot;

int func_001CE8B0(u32 arg0) {
    u8 *p;
    int *ctl;
    Slot *s;
    int n;
    int m;

    if (*(u16 *)(D_004471D4 + 0x4D6) == 0) {
        return -1;
    }
    if (arg0 >= 0x7E) {
        return -1;
    }
    ctl = (int *)(*(int *)(D_004471D4 + 0x4CC) | 0x20000000);
    n = *ctl;
    if ((u32)(n + 0xA) >= 0x3FC) {
        return -2;
    }
    m = n + 4;
    p = (u8 *)(((int)ctl + m) | 0x20000000);
    s = (Slot *)(D_004471D4 + (arg0 * 4 + 0x240));
    *p++ = 0xFF;
    *p++ = ((u8 *)(D_004471D4 + (s->b0 << 5)))[0x18];
    *p++ = 7;
    *p++ = 0xFD;
    *p++ = 0x10;
    *p++ = s->b1;
    *p++ = s->b2;
    *p++ = arg0;
    p[1] = p[0] = 0;
    *ctl += 0xA;
    s->b3 = -1;
    return 0;
}
