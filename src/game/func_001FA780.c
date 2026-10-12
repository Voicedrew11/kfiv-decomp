#include "common.h"
#include "kf_state.h"

extern ModeState D_005CF410[];

/* Starts mode 2 on the given slot if no mode is running. */
s32 func_001FA780(s32 slot) {
    if (D_005CF410->state != 0) {
        return -1;
    }
    D_005CF410->slot = slot;
    D_005CF410->retries = 0;
    D_005CF410->unk12 = 2;
    D_005CF410->phase = 0;
    D_005CF410->step = 0;
    D_005CF410->count = 0;
    D_005CF410->state = 2;
    return 0;
}
