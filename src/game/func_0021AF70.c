#include "kf_mc.h"

/* Start a memory card request: kind 0..2, slot 0..4. Returns 1 while a request is already running. */
int func_0021AF70(u32 kind, u32 slot) {
    if (D_01E4F278.state != 0) {
        return 1;
    }
    if ((kind >= 3U) || (slot >= 5U)) {
        return -1;
    }
    D_01E4F278.timeout = 0x33;
    D_01E4F278.state = 9;
    D_01E4F278.unk80 = kind;
    D_01E4F278.unk84 = slot;
    D_01E4F278.unk8C = 0;
    return 0;
}
