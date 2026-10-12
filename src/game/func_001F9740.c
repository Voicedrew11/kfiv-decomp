#include "common.h"
#include "kf_state.h"

extern ModeState D_005CF410[];

void func_001F9610(SlotRecord *);

/* Starts mode 1 unless another mode is running; optionally resets both slot records. */
s32 func_001F9740(s8 reset) {
    s32 i;

    if (D_005CF410->state != 0 && D_005CF410->state != 0x63) {
        return -1;
    }
    D_005CF410->state = D_005CF410->unk12 = 1;
    for (i = 0; i < 2; i++) {
        if (SLOT_RECORD_BYTES(&D_00428B80[i])->first != 0) {
            SLOT_RECORD_BYTES(&D_00428B80[i])->first = 1;
        }
    }
    if (reset != 0) {
        func_001F9610(&D_00428B80[0]);
        func_001F9610(&D_00428B80[1]);
    }
    return 0;
}
