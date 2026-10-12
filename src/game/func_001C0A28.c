#include "common.h"

extern s32 D_0044717C;
extern s32 D_00447184;
extern s32 D_0057DF40[64];

/* frames left of the index-th queued trail point, oldest first */
s32 func_001C0A28(s32 index) {
    // match: retail accepts index == count, one past the newest point (func_001C08A0 tests index < count)
    if (index <= D_0044717C) {
        return D_0057DF40[(D_00447184 + index) % 64];
    }
    return 0;
}
