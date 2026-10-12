#include "kf_menu.h"

extern MenuScreenState D_005CF2F0;

/* TRUE while either of the menu screen's two counters is above 0. */
s32 func_001E4068(void) {
    if (D_005CF2F0.unk19 > 0 || D_005CF2F0.unk1A > 0) {
        return TRUE;
    }
    return FALSE;
}
