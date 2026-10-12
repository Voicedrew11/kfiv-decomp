#include "common.h"

extern int D_003EF5F8[];
extern char D_00349682[];
int func_0019C688(int);
void func_001CE690(int, int, int, char, int, char, s16, int);

s32 func_0019D0F0(s32 value) {
    s32 slot;

    slot = func_0019C688(1);
    func_001CE690(slot, D_003EF5F8[0], 0, (char)value, 0x3C, D_00349682[0], 0, 0x3F);
    return slot;
}
