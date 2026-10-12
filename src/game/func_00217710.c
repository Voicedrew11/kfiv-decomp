#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* The controller's scePadGetState value as of the last func_00217008. */
int func_00217710(int port, int slot) {
    PadSlot *pad = &D_01E0DB00.slots[port * 4 + slot];

    return pad->state;
}
