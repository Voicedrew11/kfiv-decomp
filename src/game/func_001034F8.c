#include "common.h"

extern char D_0034C918[];

void func_001034F8(int arg0) {
    if ((u32) arg0 < 0x80U) {
        D_0034C918[arg0 / 8] |= 1 << (arg0 % 8);
    }
}
