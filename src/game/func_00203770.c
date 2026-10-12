#include "kf_game.h"

extern PoolSlot D_0042A8F0[32];

/* Takes the part away from every slot of the pool. */
s32 func_00203770(void) {
    u32 i;

    for (i = 0; i < 32; i++) {
        D_0042A8F0[i].part = NULL;
    }
    return TRUE;
}
