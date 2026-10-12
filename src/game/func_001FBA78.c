#include "common.h"

extern s8 *D_0044733C;

s32 func_001FBA78(void) {
    s8 *entry;
    s32 flags;
    s32 i;

    entry = D_0044733C;
    flags = 0;
    for (i = 0; i < 3; i++) {
        if (entry[1] != 0) {
            return 0xFF;
        }
        if (entry[0] != 0) {
            flags = (flags | (1 << i)) & 0xFF;
        }
        entry += 0x180;
    }
    return flags;
}
