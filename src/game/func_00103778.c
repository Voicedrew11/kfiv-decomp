#include "common.h"

extern u8 D_0034C928[0x8000];

void func_00103778(int arg0, int arg1, int arg2, int arg3) {
    if ((u32)arg0 < 0x80 && (u32)arg1 < 0x200 && (u32)arg2 < 4) {
        u8 mask = (u8)(1 << arg2);
        u8 *p;
        mask = (u8)(mask << ((arg1 % 2) * 4));
        p = &D_0034C928[arg1 / 2 + arg0 * 0x100];
        *p = *p & ~mask;
        if (arg3 != 0) {
            *p = *p | mask;
        }
    }
}
