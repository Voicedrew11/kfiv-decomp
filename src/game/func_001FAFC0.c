#include "kf_state.h"

extern ModeState D_005CF410[];

s32 func_0021B0C0(void);
int func_0021ACB8(u32, u32);
s32 func_0021B468(void);
void func_001FB0E8(void);

s32 func_001FAFC0(void) {
    ModeState *mode = D_005CF410;
    SlotRecord *slot;
    s32 result;

    slot = &D_00428B80[mode->slot];
    mode->state = 0x63;
    result = 0;
    SLOT_RECORD_BYTES(slot)->first = 2;
    switch (mode->phase) {
    case 0:
        if (func_0021B0C0() <= 0) {
            func_0021ACB8(mode->slot, 0);
            if (++mode->count == 5) {
                slot->unk24 = 1;
                slot->unk00 = 1;
                result = -1;
            } else {
                mode->count = 0;
                mode->phase++;
            }
        }
        break;
    case 1:
        if (func_0021B0C0() <= 0) {
            result = -1;
            func_0021B468();
            func_001FB0E8();
            mode->phase++;
        }
        break;
    case 2:
        result = -1;
        break;
    default:
        result = -1;
        break;
    }
    return result;
}
