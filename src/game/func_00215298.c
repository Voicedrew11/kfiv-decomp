#include "common.h"

void *func_001267B0(int);
int func_00194368(float, void *);

int func_00215298(char *arg0) {
    char *temp_a0;
    char *temp_v0;

    temp_v0 = func_001267B0(*(int *)(arg0 + 0x8));
    if (temp_v0 != 0 && (*(s64 *)(temp_v0 + 0xD8) & 0xF) == 3 && *(u16 *)(temp_v0 + 0x2C) != 0) {
        temp_a0 = *(char **)temp_v0;
        if (*(int *)(*(char **)(temp_a0 + 0xFC) + 0x88) != 0) {
            return func_00194368(*(float *)(arg0 + 0x10), temp_a0);
        }
    }
    *(int *)arg0 = 0;
}
