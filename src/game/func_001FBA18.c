#include "kf_state.h"

s32 func_001FBA18(s32 index, s32 count, s32 extra) {
    s32 limit = count * 0x5A;

    if (extra != 0) {
        limit += 0x96;
    }
    return (D_00428B80[index].unk18 >= limit) ? 1 : -1;
}
