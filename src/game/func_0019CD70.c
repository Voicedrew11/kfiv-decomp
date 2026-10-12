#include "common.h"

extern s32 D_00502170[][2];
int func_001CE8B0(u32);

void func_0019CD70(s8 slot) {
    if (slot > 0) {
        D_00502170[slot - 100][0] = 0;
        func_001CE8B0(slot);
    }
}
