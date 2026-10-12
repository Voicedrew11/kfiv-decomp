#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a UV: texel coordinates. */
int func_0021F278(int packet, u32 tex_u, u32 tex_v) {
    return func_0021F098(packet, 3, tex_u | ((u64)tex_v << 16));
}
