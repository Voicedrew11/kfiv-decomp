#include "common.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Adds amount to the player's running total at 0x2A0. */
void func_001B4EC8(int amount) {
    D_00413F80.unk2A0 += amount;
}
