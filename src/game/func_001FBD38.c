#include "common.h"

extern s8 *D_0044733C;

s32 func_001FBD38(void) {
    s8 *entry;
    s8 *end;

    entry = D_0044733C;
    end = entry + 0x480;
    do {
        if (*entry != 0) {
            return 1;
        }
        entry += 0x180;
    } while ((s32)entry < (s32)end);
    return 0;
}
