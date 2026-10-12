#include "kf_player.h"

extern PlayerState D_00413F80;
extern s32 D_00447188;

/* picks the weapon trail kind: the strongest of six values, else by unk43 */
void func_001C06E8(void) {
    s32 best = 0;

    D_00447188 = 0;
    if (D_00413F80.unk122[0] != 0 && D_00413F80.unk18E[0] > best) {
        best = D_00413F80.unk18E[0];
        D_00447188 = 1;
    }
    if (D_00413F80.unk122[1] != 0 && D_00413F80.unk18E[1] > best) {
        best = D_00413F80.unk18E[1];
        D_00447188 = 2;
    }
    if (D_00413F80.unk122[2] != 0 && D_00413F80.unk18E[2] > best) {
        best = D_00413F80.unk18E[2];
        D_00447188 = 3;
    }
    if (D_00413F80.unk122[3] != 0 && D_00413F80.unk18E[3] > best) {
        best = D_00413F80.unk18E[3];
        D_00447188 = 4;
    }
    if (D_00413F80.unk122[4] != 0 && D_00413F80.unk18E[4] > best) {
        best = D_00413F80.unk18E[4];
        D_00447188 = 5;
    }
    if (D_00413F80.unk122[5] != 0 && D_00413F80.unk18E[5] > best) {
        best = D_00413F80.unk18E[5];
        D_00447188 = 6;
    }
    if (D_00447188 == 0) {
        if (D_00413F80.equipment[5] == 0x87) {
            D_00447188 = 1;
        }
        if (D_00413F80.equipment[5] == 0x88) {
            D_00447188 = 2;
        }
        if (D_00413F80.equipment[5] == 0x89) {
            D_00447188 = 3;
        }
        if (D_00413F80.equipment[5] == 0x8A) {
            D_00447188 = 4;
        }
    }
}
