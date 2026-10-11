#include "common.h"

extern u8 D_004470A0[8];

/* Returns the index of the first clear bit among the 60 flag bits, or -1. */
int func_001246A8(void) {
    u8 *flags = D_004470A0;
    int i;
    int j;
    u8 bit;

    for (i = 0; i < 8; i++, flags++) {
        bit = 1;
        for (j = 0; j < 8 && i * 8 + j < 60; j++) {
            if (!(*flags & bit)) {
                return i * 8 + j;
            }
            bit <<= 1;
        }
    }
    return -1;
}
