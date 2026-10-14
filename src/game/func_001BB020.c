#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerItemDef weapon_defs[];
extern LevelRecordDef worn_defs[];

/* Adds what wearing item `index` gives (a weapon from ITEM_WEAPON_FIRST, the rest from ITEM_WORN_FIRST) to the player. */
void func_001BB020(int index) {
    u8 *kinds;
    u8 *amounts;
    int i;
    u16 *rates;

    if (index >= ITEM_WORN_FIRST) {
        kinds = worn_defs[ITEM_WORN_INDEX(index)].bonus_kinds;
        amounts = worn_defs[ITEM_WORN_INDEX(index)].bonus_amounts;
    } else {
        kinds = weapon_defs[ITEM_WEAPON_INDEX(index)].bonus_kinds;
        amounts = weapon_defs[ITEM_WEAPON_INDEX(index)].bonus_amounts;
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
            D_00413F80.power += amounts[i];
            break;
        case 10:
            D_00413F80.magic += amounts[i];
            break;
        case 0:
        case 11:
            break;
        }
    }
    if (index >= ITEM_WORN_FIRST) {
        rates = worn_defs[ITEM_WORN_INDEX(index)].unk12;
        D_00413F80.unkDC[0] += rates[0];
        D_00413F80.unkDC[1] += rates[1];
        D_00413F80.unkDC[2] += rates[2];
        D_00413F80.unkDC[3] += rates[3];
        D_00413F80.unkDC[4] += rates[4];
        D_00413F80.unkDC[5] += rates[5];
    }
    if (index == ITEM_DEMON_LEGS) {
        D_00413F80.unk108 = 1;
    }
    if (index == ITEM_RING_OF_SIGHT) {
        D_00413F80.unk10C = 1;
    }
}
