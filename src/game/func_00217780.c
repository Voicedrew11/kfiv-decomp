#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* The buttons held on a controller. */
int func_00217780(int port, int slot) {
    return D_01E0DB00.slots[port * 4 + slot].buttons;
}
