#include "kf_effect.h"
#include "kf_game.h"
#include "kf_model.h"

extern GameStatus D_0038CC50;
extern int D_00447390;
extern ModelInstance *D_0044739C;

void func_001A3918(int);

void func_00209408(void) {
    if (D_0038CC50.unk30 == 0) {
        D_0038CC50.unk30 = 0xC5F;
        D_0038CC50.unk40 = D_0044739C;
        D_00447390 += 2;
        func_001A3918(0x5D);
    }
}
