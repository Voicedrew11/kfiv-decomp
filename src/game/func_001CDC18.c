#include "common.h"

int func_001CBAE0();
extern char *D_004471D4;

int func_001CDC18(int arg0) {
    char *temp_s0;
    char *var_s1;
    int var_s2;
    int var_s3;

    temp_s0 = D_004471D4 + (arg0 << 5);
    *(int *) (temp_s0 + 0x1C) = 0;
    var_s1 = D_004471D4 + 0x200;
    if (*(u16 *) (D_004471D4 + 0x4D6) != 0) {
        func_001CBAE0(1, 0x50);
    }
    var_s2 = 4;
    var_s3 = (func_001CBAE0(1, 0x1010, *(s16 *) (temp_s0 + 0x18), *(int *) (temp_s0 + 0xC), *(int *) (temp_s0 + 4), *(char *) (temp_s0 + 0x1B)) != 0) ? -1 : 0;
    while (--var_s2 != -1) {
        if (*(int *) (var_s1 + 8) == arg0 && func_001CBAE0(1, 0x2070, *(int *) (var_s1 + 0xC), *(int *) (temp_s0 + 0xC), *(int *) (temp_s0 + 4), *(char *) (temp_s0 + 0x1B), 0) != 0) {
            var_s3 = -1;
            break;
        }
        var_s1 += 0x10;
    }
    if (*(u16 *) (D_004471D4 + 0x4D6) != 0) {
        func_001CBAE0(1, 0x40);
    }
    return var_s3;
}
