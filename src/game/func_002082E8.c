#include "common.h"

extern s32 D_00447358;
extern s32 D_00447368;

void func_001B9478(void);

/* State 1: count down the start delay, then advance to loading the image. */
void func_002082E8(void) {
    func_001B9478();
    D_00447368 -= 1;
    if (D_00447368 < 0) {
        D_00447358 += 1;
    }
}
