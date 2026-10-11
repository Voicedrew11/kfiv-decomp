#include "common.h"

extern s32 D_00447358;
extern s32 D_00447370;
extern s32 D_00447374;
extern u16 D_00413DAA[];
extern u16 D_00446940;

/* State 4: hold the image until the hold time runs out or a skippable button is pressed. */
void func_00208450(void) {
    if (D_00447370 > 0) {
        D_00447370 -= 1;
        if (D_00447370 < 1) {
            D_00447358 += 1;
        }
    }
    if (D_00447374 != 0 && (D_00413DAA[0] & D_00446940) != 0) {
        D_00447358 += 1;
    }
}
