#include "common.h"

extern s32 D_003EF688[];
extern s8 D_00349682[];

int func_001CE690(u32, int, char, char, char, char, s16, char);

void func_0019CE00(int mode) {
    func_001CE690(2, D_003EF688[0], 0, (char)mode, 0x3C, D_00349682[0], 0, 0x3F);
}
