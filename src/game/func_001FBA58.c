#include "common.h"
#include "kf_state.h"

/* Returns the 0x18 word of the slot record at index. */
s32 func_001FBA58(s32 index) {
    return D_00428B80[index].unk18;
}
