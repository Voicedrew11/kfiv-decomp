#include "kf_game.h"
#include "kf_model.h"

extern GameStatus D_0038CC50;
extern u8 D_004141D8[];
extern int D_00447390;
extern ModelInstance *D_0044739C;

void func_00209448(void) {
    if (D_004141D8[0] == 0) {
        D_0038CC50.unk30 = 0xC5F;
        D_0038CC50.unk40 = D_0044739C;
        D_00447390++;
    }
}
