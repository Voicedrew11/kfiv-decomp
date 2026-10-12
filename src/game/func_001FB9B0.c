#include "common.h"

extern s8 *D_0044733C;

s32 func_001FB9B0(void) {
    s8 *entry;
    s32 i;

    entry = D_0044733C + 1;
    for (i = 0; i < 3; i++) {
        if (*entry != 0) {
            return -1;
        }
        entry += 0x180;
    }
    return 1;
}
