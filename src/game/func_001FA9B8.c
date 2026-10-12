#include "common.h"
#include "kf_state.h"

extern ModeState D_005CF410[];
extern char D_00428B40[];
extern char *D_00447334;
extern char D_004408F8[];
extern char D_00440928[];
extern char D_00440940[];
extern char D_00440950[];
extern char D_00440960[];
extern char D_00440970[];
extern char D_00440980[];
extern char D_00446D08[];
extern char D_00446D10[];
extern char D_00446D18[];

int sprintf(char *, const char *, ...);
void *memset(void *, int, u32);
s32 func_0021B0C0(void);
s32 func_0021B468(void);
s32 func_0021B458(void);
s32 func_0021B438(s32, s32);
s32 func_0021B3C0(s32, s32);
s32 func_0021B3A0(s32, s32);
int func_0021ACB8(u32, u32);
int func_0021AD08(u32, u32, int, int, int, int);
int func_001FB710(char *, int);
s32 func_001FAFC0(void);

/* Steps the memory card format over both cards: step / 7 is the card, step % 7 the stage. */
s32 func_001FA9B8(void) {
    ModeState *mode = D_005CF410;
    s32 index = mode->step / 7;
    SlotRecord *slot = &D_00428B80[index];
    s32 result = 0;

    switch (mode->step % 7) {
    case 0:
        if (func_0021B0C0() > 0) {
            break;
        }
        if (func_0021ACB8(index, 0) == 0) {
            goto next;
        }
        if (++mode->retries == 5) {
            SLOT_RECORD_BYTES(slot)->first = 2;
            slot->unk24 = 1;
            result = 1;
        }
        break;
    case 1:
        if (func_0021B0C0() > 0) {
            break;
        }
        switch (func_0021B468()) {
        case 0x64:
            SLOT_RECORD_BYTES(slot)->first = 2;
            slot->unk24 = 1;
            if (mode->step == 8) {
                goto done;
            }
            goto retry;
        case 3:
        case 4:
            if (++mode->retries == 5) {
                slot->unk01 = 1;
                slot->unk24 = 3;
                mode->step++;
                mode->retries = 0;
                slot->unk03 = 0;
                slot->unk04 = 0;
                slot->unk18 = 0;
                slot->unk02 = 0;
            }
            break;
        case 0:
        case 2:
            slot->unk02 = slot->unk01 = 1;
            slot->unk03 = 0;
            mode->step++;
            break;
        case 1:
        case 0x15:
        default:
            slot->unk24 = 2;
            slot->unk01 = 0;
            slot->unk02 = 0;
            slot->unk03 = 0;
            slot->unk04 = 0;
            slot->unk18 = 0;
            SLOT_RECORD_BYTES(slot)->first = 0;
            SLOT_RECORD_BYTES(slot)->second = 0;
            if (mode->step == 8) {
                goto done;
            }
            goto retry;
        }
        break;
    case 2:
        if ((SLOT_RECORD_BYTES(slot)->second = func_0021B438(index, 0)) == 2) {
            if (slot->unk02 != 0) {
                slot->unk18 = func_0021B3C0(index, 0);
                if (slot->unk18 < 0x96) {
                    mode->step++;
                    break;
                }
            }
            SLOT_RECORD_BYTES(slot)->first = 0;
        done:
            result = 1;
        }
        break;
    case 3:
        if (func_0021AD08(index, 0, (int)D_004408F8, 0, 0xB, (int)D_00447334) == 0) {
            goto next;
        }
        if (++mode->retries == 5) {
            SLOT_RECORD_BYTES(slot)->first = 2;
            if (mode->step >= 0xA) {
                goto done;
            }
            goto retry;
        }
        break;
    case 4:
        if (func_0021B0C0() > 0) {
            break;
        }
        if (func_0021B468() == 0) {
            SLOT_RECORD_BYTES(slot)->first = 0;
            if (func_0021B458() > 0) {
                goto next;
            }
            if (mode->step == 0xB) {
                goto done;
            }
            goto retry;
        }
        SLOT_RECORD_BYTES(slot)->first = 2;
        if (mode->step == 0xB) {
            goto done;
        }
        goto retry;
    case 5:
        memset(D_00447334, 0, 0x2C0);
        if (func_0021AD08(index, 0, (int)D_00440928, 0, 0xB, (int)D_00447334) != 0) {
            if (++mode->retries < 5) {
                break;
            }
            SLOT_RECORD_BYTES(slot)->first = 2;
            if (mode->step >= 0xC) {
                goto done;
            }
            goto retry;
        }
    next:
        mode->retries = 0;
        mode->step++;
        break;
    case 6:
        if (func_0021B0C0() > 0) {
            break;
        }
        if (func_0021B468() == 0) {
            if (func_0021B458() == 0) {
                if (func_0021B3A0(index, 0) == 0) {
                    slot->unk04 = 0;
                    // match: retail repeats the check tail here instead of falling into check
                    if (mode->step == 0xD) {
                        goto done;
                    }
                    goto retry;
                }
            check:
                if (mode->step == 0xD) {
                    goto done;
                }
                goto retry;
            }
            sprintf(D_00428B40, D_00446D08, D_00440940);
            if (func_001FB710(D_00428B40, 0xDBF0) != 0) {
                goto check;
            }
            sprintf(D_00428B40, D_00440950);
            if (func_001FB710(D_00428B40, 0) != 0) {
                slot->unk03 = 0;
                if (mode->step == 0xD) {
                    goto done;
                }
            retry:
                mode->step = 7;
                break;
            }
            sprintf(D_00428B40, D_00446D10, D_00440960, D_00446D18);
            if (func_001FB710(D_00428B40, 0) == 0) {
                sprintf(D_00428B40, D_00440970, D_00440980, D_00446D18, D_00440980, D_00446D18);
                slot->unk03 = 1;
                result = 1;
                mode->step++;
                break;
            }
            slot->unk03 = 0;
            if (mode->step != 0xD) {
                mode->step = 7;
            }
            goto done;
        }
        result = func_001FAFC0();
        break;
    }
    return result;
}
