#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern LevelRecordDef D_0041E118[];

/* Adds what wearing definition `index` gives (150 and up: the item in hand, 220 and up: the rest) to the player. */
void func_001BB020(int index) {
    u8 *kinds;
    u8 *amounts;
    int i;
    u16 *rates;

    if (index >= 220) {
        kinds = D_0041E118[index - 220].bonus_kinds;
        amounts = D_0041E118[index - 220].bonus_amounts;
    } else {
        kinds = D_0041B9B8[index - 150].bonus_kinds;
        amounts = D_0041B9B8[index - 150].bonus_amounts;
    }
    for (i = 0; i < 2; i++) {
        switch (kinds[i]) {
        case 1:
            D_00413F80.unkE8 = 1;
            break;
        case 2:
            D_00413F80.unkEC = 1;
            break;
        case 3:
            D_00413F80.unkF0 += amounts[i];
            break;
        case 4:
            D_00413F80.unkF4 += amounts[i];
            break;
        case 5:
            D_00413F80.unkF8 += amounts[i];
            break;
        case 6:
            D_00413F80.unkFC += amounts[i];
            break;
        case 7:
            D_00413F80.unk100 = amounts[i];
            break;
        case 8:
            D_00413F80.unk104 = amounts[i];
            break;
        case 9:
            D_00413F80.unkAC += amounts[i];
            break;
        case 10:
            D_00413F80.unkB0 += amounts[i];
            break;
        case 0:
        case 11:
            break;
        }
    }
    if (index >= 220) {
        rates = D_0041E118[index - 220].unk12;
        D_00413F80.unkDC[0] += rates[0];
        D_00413F80.unkDC[1] += rates[1];
        D_00413F80.unkDC[2] += rates[2];
        D_00413F80.unkDC[3] += rates[3];
        D_00413F80.unkDC[4] += rates[4];
        D_00413F80.unkDC[5] += rates[5];
    }
    if (index == 0x15A) {
        D_00413F80.unk108 = 1;
    }
    if (index == 0x15F) {
        D_00413F80.unk10C = 1;
    }
}
