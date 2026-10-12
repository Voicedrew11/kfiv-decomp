#include "common.h"

extern s32 D_004468AC;
extern s32 (*const D_0043EDA8[])(void);

/* Runs the handler for the current slot; counts successes and wraps after four. */
s32 func_0019F8B0(void) {
    s32 result;

    result = D_0043EDA8[D_004468AC]();
    if (result >= 0) {
        D_004468AC += 1;
    }
    if ((u32)D_004468AC < 4U) {
        return 1;
    }
    D_004468AC = 0;
    return 0;
}
