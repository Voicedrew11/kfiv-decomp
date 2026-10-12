#include "kf_mc.h"

/* Start a memory card request: kind 0..2, slot 0..4. Returns 1 while a request is already running. */
int func_0021AEA0(u32 kind, u32 slot, int buffer_a, int buffer_b, int extra) {
    if (D_01E4F278.state != 0) {
        return 1;
    }
    if ((kind >= 3U) || (slot >= 5U)) {
        return -1;
    }
    D_01E4F278.timeout = 0x3C;
    D_01E4F278.state = 8;
    D_01E4F278.unk80 = kind;
    D_01E4F278.unk84 = slot;
    D_01E4F278.unkA0 = buffer_a;
    D_01E4F278.unkA4 = buffer_b;
    D_01E4F278.unkAC = extra;
    D_01E4F278.unk90 = D_01E4F278.unk8C = 0;
    return 0;
}
