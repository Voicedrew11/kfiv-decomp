#include "common.h"

extern char D_00349682[];
extern int D_003EF5E8[];
extern int D_003EF648[];
int rand(void);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

int func_0019D020(int arg0) {
    int *p;
    int a0;
    int pan;

    if (arg0 < 0x22 || arg0 == 0x24 || arg0 == 0x25) {
        p = D_003EF5E8;
    } else if (arg0 < 0x28) {
        p = D_003EF648;
    } else {
        return -1;
    }
    a0 = func_0019C688(1);
    pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    func_001CE690(a0, p[4], 0, arg0, 0x3C, D_00349682[0], pan, 0x3F);
    return a0;
}
