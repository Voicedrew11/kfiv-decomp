#include "kf_state.h"

int func_0021B338(void);

extern ModeState D_005CF410[];
extern ModeState D_005CF3F0[];

s32 func_001F9438(void) {
    if (func_0021B338() != 0) {
        return -1;
    }
    D_005CF410->state = 0;
    D_005CF410->step = 0;
    D_005CF410->retries = 0;
    D_005CF3F0[0] = D_005CF410[0];
    return 0;
}
