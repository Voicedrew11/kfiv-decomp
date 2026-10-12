#include "common.h"

extern int D_00447048;
extern int D_0044704C;
extern int D_00447050;
extern int D_00468190[4];
extern void *memset(void *dst, int c, u32 n);

void func_0010AE58(void) {
    D_00447048 = 0;
    D_0044704C = 0;
    memset(D_00468190, 0, sizeof(D_00468190));
    D_00447050 = 0;
}
