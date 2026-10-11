#include "common.h"

void *func_001267B0(int);

int func_0016A450(char *arg0) {
    char *s;
    int temp_v0_2;
    int var_s0;
    int var_s1;
    s64 temp_v1;
    char *temp_v0;

    s = arg0 + 0xE0;
    var_s1 = 0;
    var_s0 = 0;
    do {
        temp_v0 = func_001267B0(var_s0 + 0xA);
        if (temp_v0 != 0) {
            temp_v1 = *(s64 *)(temp_v0 + 0xD8);
            if (((temp_v1 & 3) == 1) && ((temp_v0_2 = (int)((temp_v1 << 0x1E) >> 0x20), ((temp_v0_2 & 3) == 0)) || ((temp_v0_2 & 3) == 3))) {
                var_s1 += 1;
            }
        }
        var_s0 += 1;
    } while (var_s0 < 8);
    *(char *)(*(char **)(arg0 + 0xC) + 0x410) = (int)((float)*(s16 *)(s + 0x6C) * (float)var_s1 * 0.125f);
    return 0;
}
