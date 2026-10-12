#include "common.h"
#include "kf_state.h"

extern ModeState D_005CF410[];
extern char D_00428A80[];
extern char *D_00447334;
extern char *D_0044733C;
extern s8 D_00447340;
extern char D_004408F8[];
extern char D_00440928[];
extern char D_00440940[];
extern char D_00440950[];
extern char D_00440970[];
extern char D_00440980[];
extern char D_00446D08[];
extern char D_00446D10[];
extern char D_00446D18[];
extern char D_00440960[];

int sprintf(char *, const char *, ...);
void *memset(void *, int, u32);
s32 func_0021B0C0(void);
s32 func_0021B468(void);
s32 func_0021B458(void);
int func_0021B3A0(s32, s32);
int func_0021AD08(u32, u32, int, int, int, int);
int func_0021ADD0(u32, u32, int, int, int);
int func_001FB710(char *, int);
int func_001FB7D0(void);
s32 func_001FAFC0(void);

/* Steps the memory card load: check the card, read the files, then verify them. */
s32 func_001F9C38(void) {
    ModeState *mode = D_005CF410;
    SlotRecord *slot = &D_00428B80[mode->slot];
    s32 result = 0;

    switch (mode->step) {
    case 0:
        if (func_0021B0C0() > 0) {
            break;
        }
        if (slot->unk01 == 0 || SLOT_RECORD_BYTES(slot)->second != 2) {
            result = func_001FAFC0();
            break;
        }
        SLOT_RECORD_BYTES(slot)->first = 1;
        if (slot->unk02 == 0) {
            result = 1;
            break;
        }
        mode->step++;
    case 1:
        if (func_0021AD08(mode->slot, 0, (int)D_004408F8, 0, 4, (int)D_00447334) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 2:
        if (func_0021B0C0() > 0) {
            break;
        }
        if (func_0021B468() != 0) {
            result = func_001FAFC0();
            break;
        }
        if (func_0021B458() != 0) {
            mode->retries = 0;
            mode->step++;
            break;
        }
        if (func_0021B3A0(mode->slot, 0) != 0) {
            result = func_001FAFC0();
        } else {
            result = 1;
            slot->unk03 = 0;
            slot->unk04 = 0;
        }
        break;
    case 3:
        memset(D_00447334, 0, 0x2C0);
        if (func_0021AD08(mode->slot, 0, (int)D_00440928, 0, 0xB, (int)D_00447334) == 0) {
            mode->retries = 0;
            mode->step++;
        } else if (++mode->retries >= 5) {
            result = func_001FAFC0();
        }
        break;
    case 4:
        if (func_0021B0C0() > 0) {
            break;
        }
        if (func_0021B468() != 0) {
            result = func_001FAFC0();
            break;
        }
        if (func_0021B458() == 0) {
            if (func_0021B3A0(mode->slot, 0) != 0) {
                result = func_001FAFC0();
            } else {
                slot->unk04 = 0;
                result = 1;
            }
            break;
        }
        sprintf(D_00428A80, D_00446D08, D_00440940);
        if (func_001FB710(D_00428A80, 0xDBF0) == 0) {
            slot->unk05 = 0;
        } else {
            slot->unk06 = slot->unk05 = 1;
        }
        sprintf(D_00428A80, D_00440950);
        if (func_001FB710(D_00428A80, 0) != 0) {
            slot->unk06 = 1;
            result = func_001FAFC0();
            break;
        }
        sprintf(D_00428A80, D_00446D10, D_00440960, D_00446D18);
        if (func_001FB710(D_00428A80, 0) == 0) {
            sprintf(D_00428A80, D_00440970, D_00440980, D_00446D18, D_00440980, D_00446D18);
            mode->step++;
        } else {
            slot->unk04 = 0;
            slot->unk06 = 1;
            result = 1;
        }
        break;
    case 5:
        if (func_0021ADD0(mode->slot, 0, (int)D_00428A80, (int)D_0044733C, 0x480) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 6:
        if (func_0021B0C0() > 0) {
            break;
        }
        if (func_0021B468() == 0x48) {
            slot->unk04 = 1;
            func_001FB7D0();
            result = 1;
        } else {
            slot->unk03 = 0;
            slot->unk04 = 0;
            if (D_00447340 == 0) {
                result = 1;
            } else {
                result = func_001FAFC0();
            }
        }
        break;
    }
    return result;
}
