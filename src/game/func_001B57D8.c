#include "common.h"

extern u8 D_0044694E;

void func_00217908(int, int, int);

void func_001B57D8(void) {
    if (D_0044694E == 0) {
        D_0044694E = 1;
        func_00217908(0, 0, 0);
    }
}
