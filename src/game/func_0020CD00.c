#include "kf_game.h"

extern GameClock D_00431CE8;
extern ClockControl D_00432820;
extern int D_00446ED4;
extern int D_00446EF0;

void func_0020CD00(void) {
    GameClock *clock;

    if (D_00432820.unk213 == 0) {
        if (D_00432820.flags & 0x200) {
            if (D_00432820.unk214 == 0 && (D_00446ED4 & 3) == 0) {
                clock = &D_00431CE8;
                clock->ticks += 1;
            }
        } else if (D_00432820.unk214 == 0) {
            clock = &D_00431CE8;
            clock->ticks += 1;
        }
    }
    if (D_00446EF0 & 0x400) {
        clock = &D_00431CE8;
        clock->ticks += 0x3B;
    }
}
