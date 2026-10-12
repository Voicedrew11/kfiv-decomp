#include "kf_mc.h"

/* Start memory card request 4 (read a directory): kind 0..2, slot 0..4. Returns 1 while a request is already running. */
int func_0021AD08(u32 kind, u32 slot, int name, int unkA8, int max_entries, int entries) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->state != 0) {
        return 1;
    }
    if (kind >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->unkB0 = entries;
    mc_state->state = 4;
    mc_state->unk80 = kind;
    mc_state->unk84 = slot;
    mc_state->unkA0 = name;
    mc_state->unkA8 = unkA8;
    mc_state->unkAC = max_entries;
    mc_state->unk8C = 0;
    return 0;
}
