#include "common.h"

extern char D_00349682[];
extern int D_003EF658[];
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

int func_0019D158(int value) {
    int result;

    result = func_0019C688(1);
    func_001CE690(result, D_003EF658[0], 0, (char)value, 0x3C, D_00349682[0], 0, 0x3F);
    return result;
}
