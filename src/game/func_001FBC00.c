#include "common.h"
#include "kf_state.h"

/* Returns the unk02 byte of the slot record at index. */
s8 func_001FBC00(s32 index) {
    return D_00428B80[index].unk02;
}
