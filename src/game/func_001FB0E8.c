#include "kf_state.h"

extern ModeState D_005CF410[];

int func_0021B208(int, int);

/* Sets the slot's message and clears what is known about the card after a card operation failed. */
void func_001FB0E8(void) {
    ModeState *mode = D_005CF410;
    SlotRecord *slot = &D_00428B80[mode->slot];
    s32 answer = func_0021B208(mode->slot, 0);

    switch ((s16)(mode->unk12 - 2)) {
    case 0:
        if (answer == 0) {
            slot->unk24 = 6;
        } else {
            slot->unk24 = 6;
            if (answer == 2) {
                slot->unk02 = 0;
            } else if (answer < 0) {
                SLOT_RECORD_BYTES(slot)->second = 0;
                slot->unk01 = 0;
                slot->unk02 = 0;
                slot->unk03 = 0;
                slot->unk04 = 0;
            }
        }
        break;
    case 1:
        if (answer == 0) {
            slot->unk24 = 10;
        } else if (answer == 1) {
            slot->unk24 = 4;
            slot->unk03 = 0;
            slot->unk04 = 0;
        } else if (answer == 2) {
            slot->unk24 = 4;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
        } else {
            slot->unk24 = 5;
            SLOT_RECORD_BYTES(slot)->second = 0;
            slot->unk01 = 0;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
        }
        break;
    case 2:
        if (answer < 0) {
            slot->unk24 = 9;
            SLOT_RECORD_BYTES(slot)->second = 0;
            slot->unk01 = 0;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
        } else if (answer == 1) {
            slot->unk24 = 9;
            slot->unk04 = 0;
        } else if (answer == 2) {
            slot->unk24 = 9;
            slot->unk02 = 0;
            slot->unk04 = 0;
        } else {
            slot->unk24 = 9;
        }
        break;
    case 3:
        if (answer < 0) {
            slot->unk24 = 7;
            SLOT_RECORD_BYTES(slot)->second = 0;
            slot->unk01 = 0;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
        } else if (answer == 1) {
            slot->unk24 = 7;
            slot->unk04 = 0;
        } else if (answer == 2) {
            slot->unk24 = 7;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
        } else {
            slot->unk24 = 7;
        }
        break;
    case 4:
    default:
        slot->unk24 = 1;
        break;
    }
}
