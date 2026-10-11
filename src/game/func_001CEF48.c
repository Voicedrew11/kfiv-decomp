#include "common.h"

void FlushCache(int);
extern char *D_004471D4;

int func_001CEF48(u32 arg0) {
    int *temp_v0;

    if (arg0 >= 5U) {
        return -1;
    }
    temp_v0 = (int *)(D_004471D4 + (arg0 * 0x18 + 0x440));
    if (*temp_v0 == -1) {
        return -1;
    }
    *temp_v0 = -1;
    FlushCache(0);
    return 0;
}
