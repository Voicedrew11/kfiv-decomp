#include "common.h"
#include "kf_state.h"

/* Resets a memory card slot record. */
void func_001F9610(SlotRecord *slot) {
    SLOT_RECORD_BYTES(slot)->first = 1;
    SLOT_RECORD_BYTES(slot)->second = 0;
    slot->unk18 = 0;
    slot->unk01 = 0;
    slot->unk02 = 0;
    slot->unk04 = 0;
    slot->unk03 = 0;
    slot->unk00 = 0;
    slot->unk05 = 0;
    slot->unk07 = 0;
    slot->unk06 = 0;
    slot->unk24 = 0;
}
