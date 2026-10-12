#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* Sets the rumble: the big motor runs at the given power and the small motor is on whenever that is not zero. */
void func_00217908(int port, int slot, int power) {
    PadSlot *pad = &D_01E0DB00.slots[port * 4 + slot];
    u8 big_motor = power;

    if (big_motor != 0) {
        pad->actuators[0] = 1;
    } else {
        pad->actuators[0] = 0;
    }
    pad->actuators[1] = big_motor;
}
