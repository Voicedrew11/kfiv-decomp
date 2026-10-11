#include "common.h"

typedef struct {
    char p0[0xBC];
    u8 a[2];
    char pb[2];
    u8 b[6];
    char pc[6];
    s8 state;
    s8 sel;
    char p1[10];
    u64 bits;
} Actor;

static inline void adjust(u8 *p, int d) {
    int v = *p + d;
    int hi = 0xC0;

    if (v < 0x40) v = 0x40;
    if (v > hi) v = hi;
    *p = v;
}

void func_00190A30(Actor *a) {
    int i;
    int n;
    u8 *b;

    if (a->state == 2) {
        for (i = 0; i < 2; i++) {
            if (a->sel == i) {
                adjust(&a->a[i], -10);
            } else {
                adjust(&a->a[i], 5);
            }
        }
        if (a->sel == 0) {
            n = 3;
            if ((a->bits >> 57) & 1) n = 0;
            for (i = 0, b = a->b; i < 6; i++) {
                if (((int)(a->bits >> 58) & 3) + n == i) {
                    adjust(&b[i], -10);
                } else {
                    adjust(&b[i], 5);
                }
            }
        }
    }
}
