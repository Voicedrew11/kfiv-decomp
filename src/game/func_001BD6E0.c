#include "kf_player.h"

extern PlayerState D_00413F80;
extern int D_00447128;

void func_001BD6E0(void) {
    D_00447128 = 0;
    D_00413F80.unk1C8 &= ~(1L << 36);
}
