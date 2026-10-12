#include "common.h"

int func_001CE690(u32, int, char, char, char, char, s16, char);
s32 func_001CE8B0(u32);
extern s8 D_00349680[];
extern s32 D_003EF618[];

void func_0019C4A0(void) {
    s32 i;

    for (i = 0x64; i < 0x7E; i++) {
        func_001CE690(i, D_003EF618[4], 0, 0, 0x3C, D_00349680[2], 0, 0x3F);
        func_001CE8B0(i);
    }
}
