#include "common.h"
#include "kf_state.h"

extern ModeState D_005CF410[];

s32 func_0021B0C0(void);
int func_0021ACB8(u32, u32);
int func_0021AF70(u32, u32);
s32 func_0021B468(void);
s32 func_001FAFC0(void);

/* Steps the memory card check: open the slot, read its status, then confirm it. */
s32 func_001FA7C0(void) {
    ModeState *mode = D_005CF410;
    SlotRecord *slot = &D_00428B80[mode->slot];
    s32 result = 0;
    s32 status;

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
            status = func_0021B468();
            if (status != 3) {
                result = func_001FAFC0();
            } else if (++mode->retries == 5) {
                slot->unk24 = status;
                slot->unk01 = 1;
                slot->unk03 = 0;
                slot->unk04 = 0;
                slot->unk18 = 0;
                slot->unk02 = 0;
                mode->retries = 0;
                mode->step++;
            }
        }
        break;
    case 2:
        if (func_0021AF70(mode->slot, 0) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 3:
        if (func_0021B0C0() <= 0) {
            if (func_0021B468() == 0x34) {
                slot->unk02 = 1;
                slot->unk24 = 0;
                SLOT_RECORD_BYTES(slot)->first = 0;
                result = 1;
            } else {
                result = func_001FAFC0();
            }
        }
        break;
    }
    return result;
}
