#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* The buttons that went down this frame. */
int func_002177A8(int port, int slot) {
    return D_01E0DB00.slots[port * 4 + slot].pressed;
}
