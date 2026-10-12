#include "common.h"

extern char D_003679A0[];
extern char D_00413F80[];
void func_001CFA70(int, int, int, int, int, int, ...);

typedef struct {
    char pad0[0x14];
    u8 b14;
    u8 pad15;
    u16 h16;
} E22;

void func_001BBEE0(int arg0) {
    E22 *e = (E22 *)(D_003679A0 + arg0 * 0x22);
    u16 v;
    int n;
    int t;
    int *p;

    v = e->h16;
    if (v < 0x1E0) {
        n = v + 1;
        e->h16 = n;
        if ((u16)n == 0xB4 || (u16)n == 0x1E0) {
            if (e->b14 < 2) {
                e->b14++;
                func_001CFA70(0, 0, 0, 0, 0x2D, 0x2C);
            }
        }
    }
    p = (int *)D_00413F80;
    t = (p[53] + 1) % 100;
    p[53] = t;
    if (t == 0) {
        if (p[10] < 0x3E7) {
            p[10]++;
            func_001CFA70(0, 0, 0, 0, 0x2D, 0x2A);
        }
    }
}
