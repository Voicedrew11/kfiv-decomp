#include "common.h"

int func_00115550(int, int);
extern char D_0038CD20[];

void func_001154D8(int arg0, int arg1) {
    int var_s0;
    u16 temp_a0_2;
    int var_s1;
    char *temp_a0;

    temp_a0 = &D_0038CD20[0] + arg0 * 0x60;
    var_s0 = *(int *)(temp_a0 + 0x14);
    if (var_s0 != 0) {
        temp_a0_2 = *(u16 *)(*(char **)(temp_a0 + 8) + 0x3C);
        if (temp_a0_2 != 0) {
            var_s1 = temp_a0_2;
            do {
                func_00115550(var_s0, arg1);
                var_s1 -= 1;
                var_s0 += 0x80;
            } while (var_s1 != 0);
        }
    }
}
