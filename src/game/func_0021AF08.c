#include "kf_mc.h"

/* Start memory card request 12: kind 0..2, slot 0..4. Returns 1 while a request is already running. */
int func_0021AF08(u32 kind, u32 slot, int unkA0, int unkB4, int unkA8) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->state != 0) {
        return 1;
    }
    if (kind >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->timeout = 0x46;
    mc_state->state = 0xC;
    mc_state->unk80 = kind;
    mc_state->unk84 = slot;
    mc_state->unkA0 = unkA0;
    mc_state->unkB4 = unkB4;
    mc_state->unkA8 = unkA8;
    mc_state->unk90 = mc_state->unk8C = 0;
    return 0;
}
