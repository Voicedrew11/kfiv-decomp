#include "common.h"
#include "kf_mc.h"

/* Start a memory card request. Fails while another request is in progress or the arguments are out of range. */
int func_0021B070(u32 unit, u32 index, int request_value) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->state != 0) {
        return 1;
    }
    if (unit >= 3U || index >= 5U) {
        return -1;
    }
    mc_state->unkA0 = request_value;
    mc_state->state = 0xD;
    mc_state->unk84 = index;
    mc_state->unk80 = unit;
    mc_state->unk8C = 0;
    return 0;
}
