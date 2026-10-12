#include "kf_player.h"

extern PlayerState D_00413F80;

void func_001B9478(void) {
    D_00413F80.unk1C4 = 0.0f;
    D_00413F80.unk1B0 = 0.0f;
    D_00413F80.unk1B4 = 0.0f;
    D_00413F80.unk1B8 = 0.0f;
    D_00413F80.unk1BC = 0.0f;
    D_00413F80.unk1C0 = 0;
    D_00413F80.unk1C8 &= ~0x100000000L;
}
