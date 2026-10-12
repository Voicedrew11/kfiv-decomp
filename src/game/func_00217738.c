#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* The controller's scePadGetReqState value as of the last func_00217008. */
int func_00217738(int port, int slot) {
    PadSlot *pad = &D_01E0DB00.slots[port * 4 + slot];

    return pad->request_state;
}
