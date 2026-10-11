#include "common.h"

extern u8 D_0034C4E0[];

void func_00212008(int *arg0) {
    u32 temp_v1;
    u8 *temp_a1;

    temp_v1 = arg0[2];
    if (temp_v1 < 0x32U) {
        temp_a1 = D_0034C4E0 + temp_v1;
        if (*temp_a1 < 3U) {
            *temp_a1 = *temp_a1 + 1;
        }
    }
    arg0[0] = 0;
}
