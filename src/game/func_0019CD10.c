#include "common.h"

extern s32 D_00502170[][2];
int func_001CE9A0(int, u32, int);

void func_0019CD10(void) {
    s32 i;
    for (i = 0; i < 0x1A; i++) {
        if (D_00502170[i][0] > 0)
            func_001CE9A0(0, i + 100, D_00502170[i][1]);
    }
}
