#include "common.h"

u16 func_001090E0(char *arg0) {
    char *p;
    char **slot;

    p = *(char **)(arg0 + 0xFC);
    slot = (char **)(p + (u8)p[0x81] * 4);
    return *(u16 *)(*slot - 4);
}
