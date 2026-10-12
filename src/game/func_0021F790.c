#include "common.h"

extern s32 D_00437824[];
extern s32 D_0043782C[];
extern s32 D_00437838[];

void func_0021E6D0(s32 *);

int func_0021F790(void) {
    if (D_00437824[0] != 0 && D_0043782C[0] != 0) {
        func_0021E6D0(D_00437838);
        D_0043782C[0] = 0;
    }
    __asm__ volatile("sync");
    __asm__ volatile("ei");
    return 0;
}
