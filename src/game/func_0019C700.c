#include "common.h"

extern s8 D_00500158[];

/* Claims the first free slot of D_00500158 and returns its id (0x54 + index). */
s8 func_0019C700(void) {
    s8 *flag;
    s32 i;

    flag = D_00500158;
    for (i = 0; i < 16; i++, flag++) {
        if (*flag < 0) {
            *flag = 1;
            return i + 0x54;
        }
    }
}
