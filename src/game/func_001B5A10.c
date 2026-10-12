#include "kf_player.h"

extern PlayerState D_00413F80;

void func_001BB5E8(void);

void func_001B5A10(void) {
    D_00413F80.unkBC = 0;
    D_00413F80.unkC0 = 0;
    D_00413F80.unk1A4 = 0;
    D_00413F80.unk1A6 = 0;
    D_00413F80.unk264 = 0;
    ((int (*)(void))func_001BB5E8)(); // match: called as if it returned something, which keeps the call from being a jump
}
