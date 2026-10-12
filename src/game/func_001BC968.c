#include "kf_player.h"

extern PlayerState D_00413F80;
extern u32 D_00446ED4;

/* Kind 0x12 starts the first timed flag: 1800 frames from now. */
void func_001BC968(int kind) {
    if (kind == 0x12) {
        D_00413F80.timed_flags[0] = 1800;
        D_00413F80.unk2C4 = D_00446ED4;
    }
}
