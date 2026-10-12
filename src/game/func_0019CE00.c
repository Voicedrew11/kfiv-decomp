#include "common.h"

extern s32 D_003EF688[];
extern s8 D_00349682[];

void func_001CE690(int, int, int, char, int, char, s16, int);

void func_0019CE00(int mode) {
    func_001CE690(2, D_003EF688[0], 0, (char)mode, 0x3C, D_00349682[0], 0, 0x3F);
}
