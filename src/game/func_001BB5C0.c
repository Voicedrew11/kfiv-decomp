#include "kf_player.h"

extern PlayerState D_00413F80;

void func_001B9610(void);

void func_001BB5C0(void) {
    if (D_00413F80.unk1A8 == 0) {
        D_00413F80.unkB4 = 0;
        D_00413F80.unk1A4 = 0;
        func_001B9610();
    }
}
