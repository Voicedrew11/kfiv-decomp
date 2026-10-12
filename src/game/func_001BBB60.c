#include "kf_player.h"

extern PlayerState D_00413F80;
extern const PlayerLevelStep D_0043E130[];

void func_001CFA70(int, int, int, int, int, int, ...);

/* Adds experience and takes every level it now reaches. */
void func_001BBB60(int amount) {
    int level;

    if (D_00413F80.equipment[6] == 0x94) {
        amount *= 2;
    }
    D_00413F80.unk50 += amount;
    if (D_00413F80.unk50 > 999998) {
        D_00413F80.unk50 = 999999;
    }
    for (level = D_00413F80.unk54 + 1; level < 100; level++) {
        if (D_00413F80.unk50 >= D_0043E130[level].experience) {
            D_00413F80.unk54++;
            D_00413F80.unk20 += D_0043E130[level].unk04;
            D_00413F80.unk24 += D_0043E130[level].unk05;
            D_00413F80.unk28 += D_0043E130[level].unk06;
            D_00413F80.unk2C += D_0043E130[level].unk07;
            func_001CFA70(0, 0, 0, 0, 0x2D, 0x28);
        }
    }
}
