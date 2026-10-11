#include "common.h"

extern int D_00447028;
void sceGsSyncPath(int, int);

#define HW_10009000 (*(volatile u32 *)0x10009000)
#define HW_12001000 (*(volatile u64 *)0x12001000)

void func_00109800(void) {
    if (D_00447028 != 0) {
        D_00447028 = 0;
        do {
        } while (HW_10009000 & 0x100);
        sceGsSyncPath(0, 0);
        do {
        } while ((HW_12001000 & 2) == 0);
    }
}
