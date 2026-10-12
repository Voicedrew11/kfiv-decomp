#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds an XYOFFSET_1. */
int func_0021F430(int packet, u32 offset_x, u32 offset_y) {
    return func_0021F098(packet, GS_REG_XYOFFSET_1, offset_x | ((u64)offset_y << 32));
}
