#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* Asks for digital mode. */
void func_00217948(int port, int slot, int lock) {
    PadSlot *pad = &D_01E0DB00.slots[port * 4 + slot];

    pad->mode_lock = lock;
    pad->mode_request = 0;
}
