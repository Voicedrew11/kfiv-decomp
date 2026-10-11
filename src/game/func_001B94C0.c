#include "common.h"

int func_001BB308(int);
extern char D_00413F80[];
extern u64 D_00414448[];
extern int D_00446ED4;

void func_001B94C0(void) {
    char *p;
    u64 temp_d;
    int temp_v0;
    u64 temp_v1;
    int temp_v1_2;
    int var_a0;

    p = D_00413F80;
    temp_d = (D_00414448[0] >> 39) & 1;
    temp_v1 = ((*(u64 *)(p + 0x1C8)) >> 39) & 1;
    if (temp_v1 != temp_d) {
        if (temp_v1 != 0) {
            temp_v0 = (*(int *)(p + 0xAC) * 9) + 0x78;
            *(int *)(p + 0xCC) = *(int *)(p + 0xD0) = temp_v0;
        } else {
            *(int *)(p + 0xD0) = *(int *)(p + 0xCC) = 0;
        }
    } else if (temp_v1 != 0) {
        var_a0 = *(int *)(p + 0xCC);
        temp_v1_2 = (*(int *)(p + 0xAC) * 9) + 0x78;
        *(int *)(p + 0xD0) = temp_v1_2;
        if (temp_v1_2 < var_a0) {
            *(int *)(p + 0xCC) = temp_v1_2;
            var_a0 = temp_v1_2;
        }
        if (var_a0 > 0) {
            if ((*(u8 *)(p + 0x45) != 0x9A) || ((D_00446ED4 & 1) != 0)) {
                *(int *)(p + 0xCC) = var_a0 - 1;
            }
        } else {
            func_001BB308(1);
        }
    }
}
