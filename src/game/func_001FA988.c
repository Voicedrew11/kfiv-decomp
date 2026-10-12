#include "kf_state.h"

extern ModeState D_005CF410[];

s32 func_001FA988(void) {
    D_005CF410->retries = 0;
    D_005CF410->unk12 = 6;
    D_005CF410->phase = 0;
    D_005CF410->step = 0;
    D_005CF410->count = 0;
    D_005CF410->state = 6;

    return 0;
}
