#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* Turns auto-repeat off for the given buttons. */
void func_00217880(int port, int slot, u16 buttons) {
    D_01E0DB00.slots[port * 4 + slot].repeat_mask &= ~buttons;
}
