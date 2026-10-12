#include "kf_player.h"

extern PlayerState D_00413F80;
extern u64 D_00414448[];
void func_001BB260(int);

void func_001B92C0(float speed) {
    int amount;

    if (speed < 66.666664f) {
        return;
    }
    amount = speed * speed / (1800.0f / (speed / 6.6666665f - 9.0f));
    if (D_00413F80.unk204 != 0) {
        amount = amount * 60 / 100;
    } else if ((D_00413F80.unk1C8 >> 38 & 1) == 1) {
        if (!(D_00414448[0] >> 38 & 1)) {
            amount = amount * 80 / 100;
        }
    }
    if (amount > 0) {
        func_001BB260(amount);
        if (D_00413F80.unk42 != 0x7D) {
            D_00413F80.unk200 = 60;
        }
    }
}
