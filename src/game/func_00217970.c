#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* Asks for analog mode; FALSE when the controller cannot switch modes. */
int func_00217970(int port, int slot, int lock) {
    PadSlot *pad = &D_01E0DB00.slots[port * 4 + slot];

    if (!(pad->features & 1)) {
        return FALSE;
    }
    pad->mode_lock = lock;
    pad->mode_request = 1;
    return TRUE;
}
