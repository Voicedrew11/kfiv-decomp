#include "common.h"

extern u8 D_00446D70[4];

/* Resets the four color bytes at D_00446D70 (red, green, blue, alpha) to 0x80. */
s32 func_001FDD08(void) {
    D_00446D70[0] = 0x80;
    D_00446D70[1] = 0x80;
    D_00446D70[2] = 0x80;
    D_00446D70[3] = 0x80;
    return 1;
}
