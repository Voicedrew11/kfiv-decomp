#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* The buttons that fired this frame, auto-repeat included. */
int func_002177D0(int port, int slot) {
    return D_01E0DB00.slots[port * 4 + slot].repeated;
}
