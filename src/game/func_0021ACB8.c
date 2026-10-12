#include "kf_mc.h"

/* Start memory card request 3: kind 0..2, slot 0..4. Returns 1 while a request is already running. */
int func_0021ACB8(u32 kind, u32 slot) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->state != 0) {
        return 1;
    }
    if (kind >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->unk84 = slot;
    mc_state->state = 3;
    mc_state->unk80 = kind;
    mc_state->unk8C = 0;
    return 0;
}
