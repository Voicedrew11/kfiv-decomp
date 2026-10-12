#include "common.h"

extern s32 D_00421050[12];

/* Empties the 12 entry slots. */
void func_001DC0E0(void) {
    s32 i;
    s32 empty = -1;

    for (i = 11; i >= 0; i--) {
        D_00421050[i] = empty;
    }
}
