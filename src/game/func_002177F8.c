#include "common.h"

extern char D_01E0DB00[];

void func_002177F8(int arg0, int arg1, u16 arg2, u16 arg3, u16 arg4) {
    int temp_v0;
    int var_t3;
    char *var_a1;
    char *var_t1;
    char *var_t2;
    char *var_v1;
    char *temp_a0;

    var_t3 = 0;
    temp_a0 = D_01E0DB00 + ((arg0 * 4 + arg1) * 0x1C0);
    var_t2 = temp_a0 + 0x124;
    var_t1 = temp_a0 + 0x127;
    var_a1 = temp_a0 + 0x126;
    var_v1 = temp_a0 + 0x125;
    *(u16 *)(temp_a0 + 0x164) = arg2 | *(u16 *)(temp_a0 + 0x164);
    do {
        temp_v0 = arg2 >> var_t3;
        var_t3 += 1;
        if (temp_v0 & 1) {
            *var_t2 = arg3;
            *var_v1 = arg4;
            *var_a1 = 0;
            *var_t1 = 0;
        }
        var_t1 += 4;
        var_a1 += 4;
        var_v1 += 4;
        var_t2 += 4;
    } while (var_t3 < 0x10);
}
