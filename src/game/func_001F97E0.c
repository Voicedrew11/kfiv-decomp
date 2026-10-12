#include "common.h"
#include "kf_state.h"

extern ModeState D_005CF3F0[];
extern char *D_00447334;
extern char D_004408F8[];

s32 func_0021B0C0(void);
s32 func_0021B468(void);
s32 func_0021B458(void);
int func_0021ACB8(u32, u32);
int func_0021B3A0(s32, s32);
int func_0021B3C0(s32, s32);
int func_0021B438(int, int);
int func_0021AD08(u32, u32, int, int, int, int);

/* Steps the check of both memory cards: five steps per card (probe, card type, free space, open the
   directory, look for the save), then the same for the second card. Returns 1 when finished. */
s32 func_001F97E0(void) {
    ModeState *mode = D_005CF3F0;
    s32 card;
    SlotRecord *slot;
    s32 status;

    if (mode->step >= 10) {
        return 1;
    }
    card = mode->step / 5;
    slot = &D_00428B80[card];
    switch (mode->step % 5) {
    case 0:
        if (func_0021ACB8(card, 0) != 0) {
            if (++mode->retries == 5) {
                slot->unk00 = 1;
                SLOT_RECORD_BYTES(slot)->first = 2;
                slot->unk24 = 1;
                mode->retries = 0;
                return 1;
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 1:
        if (func_0021B0C0() > 0) {
            return 0;
        }
        switch (func_0021B468()) {
        case 0x64:
            slot->unk00 = 1;
            SLOT_RECORD_BYTES(slot)->first = 2;
            slot->unk24 = 1;
            if (mode->step == 6) {
                return 1;
            }
            mode->step = 5;
            break;
        case 3:
        case 4:
        case 0x15:
            if (++mode->retries == 5) {
                slot->unk01 = 1;
                slot->unk02 = 0;
                slot->unk03 = 0;
                slot->unk04 = 0;
                slot->unk18 = 0;
                slot->unk24 = 3;
                mode->step++;
                mode->retries = 0;
            }
            break;
        case 0:
        case 2:
            status = func_0021B3A0(card, 0);
            if (status <= 0) {
                if (status >= -2) {
                    slot->unk02 = slot->unk01 = 1;
                    mode->step++;
                    break;
                }
            }
            SLOT_RECORD_BYTES(slot)->second = 0;
            slot->unk01 = 0;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
            slot->unk24 = 2;
            if (mode->step == 6) {
                return 1;
            }
            mode->step = 5;
            return 0;
        case 1:
        default:
            SLOT_RECORD_BYTES(slot)->first = 0;
            SLOT_RECORD_BYTES(slot)->second = 0;
            slot->unk01 = 0;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
            slot->unk24 = 2;
            if (mode->step == 6) {
                mode->step = 10;
                return 1;
            }
            mode->step = 5;
            break;
        }
        break;
    case 2:
        SLOT_RECORD_BYTES(slot)->second = func_0021B438(card, 0);
        if (slot->unk02 != 0 && SLOT_RECORD_BYTES(slot)->second == 2) {
            slot->unk18 = func_0021B3C0(card, 0);
            mode->step++;
            mode->retries = 0;
            SLOT_RECORD_BYTES(slot)->first = 0;
        } else {
            SLOT_RECORD_BYTES(slot)->first = 0;
            if (mode->step == 7) {
                return 1;
            }
            mode->step = 5;
        }
        break;
    case 3:
        if (func_0021AD08(card, 0, (int)D_004408F8, 0, 0xB, (int)D_00447334) != 0) {
            if (++mode->retries == 5) {
                if (mode->step == 8) {
                    return 1;
                }
                mode->step = 5;
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 4:
        if (func_0021B0C0() > 0) {
            return 0;
        }
        if (func_0021B468() == 0) {
            if (func_0021B458() > 0) {
                slot->unk03 = 1;
            } else {
                slot->unk03 = 0;
            }
            if (++mode->step >= 10) {
                return 1;
            }
        } else {
            if (mode->step == 9) {
                return 1;
            }
            mode->step = 5;
        }
        break;
    }
    return 0;
}
