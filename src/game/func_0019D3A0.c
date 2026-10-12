#include "common.h"

extern char D_004470E1;
extern u8 D_00349682[];
extern int D_003EF658[];
int rand(void);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

int func_0019D3A0(u8 arg0, int arg1) {
    int pan;
    int a0;

    if (D_004470E1 == 0) {
        return -1;
    }
    if (arg1 < 0) {
        arg1 = 0;
    }
    arg1 = (int) ((float) arg1 * (float) D_00349682[0] * 0.015625f);
    if (arg1 >= 128) {
        arg1 = 127;
    }
    pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    a0 = func_0019C688(3);
    if (arg1 != 0) {
        func_001CE690(a0, D_003EF658[0], 0, arg0, 0x3C, arg1, pan, 0x3F);
    }
    return 1;
}
