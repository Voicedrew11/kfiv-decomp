#include "common.h"

extern s32 D_00447358;
extern s32 D_00447364;
extern s32 D_00447380;
extern s32 D_00447384;
extern s32 D_0044736C;

/* State 5: fade the image out over the fade-out frames, then advance to closing. */
void func_002084A0(void) {
    D_00447384 += 1;
    if (D_00447384 < D_00447380) {
        D_00447364 = 0x80 - (D_00447384 << 7) / D_00447380;
        return;
    }
    D_0044736C = 2;
    D_00447364 = 0;
    D_00447358 += 1;
}
