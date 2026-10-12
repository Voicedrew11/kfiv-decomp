#include "common.h"
#include "kf_state.h"

/* Clears the unk07 byte of the slot record at index. */
void func_001FBB68(s32 index) {
    D_00428B80[index].unk07 = 0;
}
