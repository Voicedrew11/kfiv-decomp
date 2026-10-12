#include "common.h"
#include "kf_state.h"

void func_001F9610(SlotRecord *);

/* Resets the slot record at index. */
void func_001FBB50(s32 index) {
    func_001F9610(&D_00428B80[index]);
}
