#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds an XYZ2: a vertex, which draws. */
int func_0021F2F8(int packet, u32 pos_x, u32 pos_y, u32 depth) {
    return func_0021F098(packet, 5, pos_x | ((u64)pos_y << 16) | ((u64)depth << 32));
}
