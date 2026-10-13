#include "kf_pad.h"

extern u8 D_0044694E;


void func_001B57D8(void) {
    if (D_0044694E == 0) {
        D_0044694E = 1;
        Pad_SetRumble(0, 0, 0);
    }
}
