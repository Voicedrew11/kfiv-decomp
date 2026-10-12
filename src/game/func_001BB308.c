#include "kf_game.h"
#include "kf_player.h"

extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;

/* Takes amount from the counter, clamped at zero; fails for a non-positive amount or while the block is locked. */
int func_001BB308(int amount) {
    if (amount > 0 && D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (amount < D_00413F80.unkB4) {
            D_00413F80.unkB4 -= amount;
        } else {
            D_00413F80.unkB4 = 0;
        }
        return 1;
    }
    return 0;
}
