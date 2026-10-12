#include "common.h"
#include "kf_game.h"
#include "kf_mc.h"
#include "kf_state.h"

extern ModeState D_005CF410[];
extern ScratchSlot *D_0044733C;
extern char D_00428AC0[];
extern char D_00440940[];
extern char D_00440970[];
extern char D_00440980[];
extern char D_004409B0[];
extern char D_004409C0[];
extern char D_00446D10[];
extern char D_00446D18[];
extern char D_00446D20[];
extern char D_00446D28[];

int sprintf(char *, const char *, ...);
s32 func_0021B0C0(void);
s32 func_0021B468(void);
int func_0021ACB8(u32, u32);
int func_0021B020(u32, u32, int);
int func_0021AE38(u32, u32, int, int, int);
int func_0021AD68(u32, u32, int, int, int);
void func_001FB8A0(void *, s32);
s32 func_001FAFC0(void);

/* Steps the memory card save: format, create the icon and data files, then write them. */
s32 func_001FA028(void) {
    ModeState *mode = D_005CF410;
    SlotRecord *slot = &D_00428B80[mode->slot];
    s32 result = 0;
    s32 file = mode->file;
    s32 status;
    char *flag;

    switch (mode->step) {
    case 0:
        if (func_0021B0C0() <= 0) {
            if (func_0021ACB8(mode->slot, 0) != 0) {
                if (++mode->retries == 5) {
                    result = func_001FAFC0();
                }
            } else {
                mode->retries = 0;
                mode->step++;
            }
        }
        break;
    case 1:
        if (func_0021B0C0() <= 0) {
            if (func_0021B468() != 0) {
                return func_001FAFC0();
            }
            if (slot->unk20 == 0x200 && slot->unk00 == 0 && slot->unk01 != 0 && slot->unk02 != 0) {
                if (slot->unk03 == 0) {
                    sprintf(D_00428AC0, D_00446D10, D_00440980, D_00446D18);
                    mode->step++;
                } else {
                    if (slot->unk04 == 0 || slot->unk05 == 1) {
                        mode->step = 4;
                    } else {
                        mode->step = 7;
                    }
                }
                SLOT_RECORD_BYTES(slot)->first = 1;
            } else {
                result = func_001FAFC0();
            }
        }
        break;
    case 2:
        if (func_0021B020(mode->slot, 0, (int)D_00428AC0) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 3:
        status = func_0021B0C0();
        if (status > 0) {
            break;
        }
        if (func_0021B468() == 0x64 || status < 0) {
            slot->unk03 = 0;
            result = func_001FAFC0();
        } else {
            slot->unk03 = 1;
            mode->step++;
        }
        break;
    case 4:
        sprintf(D_00428AC0, D_00446D20, D_00440980, D_00446D18, D_00440940);
        mode->step++;
    case 5:
        if (func_0021AE38(mode->slot, 0, (int)D_00428AC0, (int)slot->unk0C, 0xDBF0) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 6:
        status = func_0021B0C0();
        if (status > 0) {
            break;
        }
        if (func_0021B468() != 0x3E || status < 0) {
            result = func_001FAFC0();
        } else {
            mode->step++;
        }
        break;
    case 7:
        sprintf(D_00428AC0, D_004409B0, D_00440980, D_00446D18, D_00440980, D_00446D28, file);
        mode->step++;
    case 8:
        if (func_0021AD68(mode->slot, 0, (int)D_00428AC0, (int)slot->unk08, *slot->unk08) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 9:
        status = func_0021B0C0();
        if (status > 0) {
            break;
        }
        if (func_0021B468() != 0x3E || status < 0) {
            flag = &D_0044733C[file].flag1;
            *flag = 1;
            result = func_001FAFC0();
        } else {
            flag = &D_0044733C[file].flag1;
            *flag = 0;
            mode->step++;
        }
        break;
    case 10:
        func_001FB8A0((void *)mode->slot, file);
        sprintf(D_00428AC0, D_00440970, D_00440980, D_00446D18, D_00440980, D_00446D18);
        mode->step++;
    case 11:
        if (func_0021AD68(mode->slot, 0, (int)D_00428AC0, (int)D_0044733C, 0x480) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 12:
        status = func_0021B0C0();
        if (status > 0) {
            break;
        }
        if (func_0021B468() != 0x3E || status < 0) {
            result = func_001FAFC0();
        } else {
            slot->unk04 = 1;
            func_001FB2A0(mode->slot);
            sprintf(D_00428AC0, D_004409C0, D_00440980, D_00446D18);
            mode->step++;
        }
        break;
    case 13:
        if (func_0021AE38(mode->slot, 0, (int)D_00428AC0, (int)D_00447338, 0x3C4) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 14:
        status = func_0021B0C0();
        if (status > 0) {
            break;
        }
        if (func_0021B468() != 0x3E || status < 0) {
            return func_001FAFC0();
        }
        result = 1;
        break;
    }
    return result;
}
