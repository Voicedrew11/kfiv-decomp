#include "common.h"
#include "kf_effect.h"

extern s32 D_00446868;
extern int D_004468B4;
extern char D_004470E0;
extern char D_004470E1;

/* Counts a finished sound step and sets the flag for the next one. */
int func_001A0FC8(void) {
    D_00446868 += 1;
    D_004468B4 = 0;
    if (D_004470E0 != 0) {
        D_004470E1 = TRUE;
        return 1;
    }
    D_004470E0 = TRUE;
    return 0;
}
