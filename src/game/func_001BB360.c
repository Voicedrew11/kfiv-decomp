#include "kf_game.h"

extern GameStatus D_0038CC50;
extern s32 D_00414580[];

void func_001BB360(void) {
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        D_00414580[0] = 10;
    }
}
