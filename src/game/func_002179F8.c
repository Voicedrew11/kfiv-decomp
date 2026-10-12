#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* The controller's mode: 0 digital, 1 analog, 2 analog with pressure, 0xFF not known yet. */
int func_002179F8(int port, int slot) {
    return D_01E0DB00.slots[port * 4 + slot].mode;
}
