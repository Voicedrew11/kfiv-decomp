#include "common.h"

extern u8 D_00369830[];

int func_001F92D0(void) {
    int i;

    for (i = 0; i < 0x17C; i++) {
        if (D_00369830[i] != 0 && (i < 0x2F || i == 0x36 || i >= 0x3C) && i != 0 && i != 0x96 && i != 0xDC &&
            i != 0xFA && i != 0x113 && i != 0x12C && i != 0x145 && i != 0x15E && i != 0x168 && i != 0x172 && i != 0) {
            return TRUE;
        }
    }
    return FALSE;
}
