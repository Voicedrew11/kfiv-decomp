#include "common.h"
#include "kf_game.h"

extern int D_0044733C;

/* Checks the three scratch slots: -1 if one has flag1 set, 1 if one has flag0 set, else 0. */
s32 func_001FBCF0(void) {
    s32 address = D_0044733C;
    s32 end = address + 0x480;
    ScratchSlot *slot;

    do {
        slot = (ScratchSlot *)address;
        if (slot->flag1 == 0) {
            if (slot->flag0 != 0) {
                return 1;
            }
        } else {
            return -1;
        }
        address += 0x180;
    } while (address < end);
    return 0;
}
