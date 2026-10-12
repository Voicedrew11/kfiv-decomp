#include "common.h"

extern u8 D_00446B39;

/* Steps the 7-frame counter, wrapping back to 0. */
void func_001D8298(void) {
    D_00446B39 = (s8)(D_00446B39 + 1) % 7;
}
