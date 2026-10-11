#include "common.h"

extern s16 D_005CF420[64];

s32 func_001FB6F8(void) {
    return (D_005CF420[0] == 0) ? 0 : -1;
}
