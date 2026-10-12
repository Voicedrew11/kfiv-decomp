#include "kf_mc.h"

int func_00210AF8(int, int);
int func_00210C28(int);

/* Starts a memory-card request once, then clears the done flag when it finishes. */
void func_00211E58(McState *state) {
    if (state->unk84 == 0) {
        state->unk84 = 1;
        state->state = func_00210AF8(state->unk00[2], 0);
        if (state->unk00[4] == 1) {
            state->unk00[0] = 0;
        }
    } else if (func_00210C28(state->state) == 0) {
        state->unk00[0] = 0;
    }
}
