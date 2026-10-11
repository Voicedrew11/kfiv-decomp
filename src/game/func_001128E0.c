#include "common.h"

extern char D_0038CD20[];
extern char D_0038CDE0[];
extern int D_00446770;
extern int D_00446778[2];
extern int D_00446780[2];
extern int D_00446788;
extern int D_0044678C;

void *memset(void *, int, u32);

void func_001128E0(void) {
    D_00446770 = 0;
    memset(D_0038CD20, 0, 0xC0);
    memset(D_00446778, 0, 8);
    memset(D_00446780, 0, 8);
    memset(D_0038CDE0, 0, 0x7F8);
    D_00446788 = 0;
    D_0044678C = 0;
}
