#include "common.h"

int func_001CBAC0(void);
void func_0019C7D0(s32);
extern u8 D_00349681[];
extern int D_00446858;
extern int D_0044685C;
extern int D_00446860;

/* Raises the shared volume by 3 (and applies it unless mode 0x12) and steps on once it passes the limit. */
int func_0019F0F8(int unused, int mode) {
    if (func_001CBAC0() != 0) {
        return -1;
    }
    D_0044685C = 1;
    D_00446860 += 3;
    if (mode != 0x12) {
        func_0019C7D0((D_00446860 < D_00349681[0]) ? D_00446860 : D_00349681[0]);
    }
    if (D_00446860 >= 0x100 || D_00349681[0] < D_00446860) {
        D_00446858 += 1;
        return 1;
    }
    return -1;
}
