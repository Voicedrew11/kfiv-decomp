#include "kf_player.h"

extern PlayerState D_00413F80;

void func_001BB020(int);

/* Works out what the worn items add: clears unkDC to unk10C, lets every worn item add its part, then
   turns the four rates into frame counts. */
void func_001BAE88(void) {
    D_00413F80.unkE8 = 0;
    D_00413F80.unkEC = 0;
    D_00413F80.unkF0 = 0;
    D_00413F80.unkF4 = 0;
    D_00413F80.unkF8 = 0;
    D_00413F80.unkFC = 0;
    D_00413F80.unk100 = 0;
    D_00413F80.unk104 = 0;
    D_00413F80.unk108 = 0;
    D_00413F80.unk10C = 0;
    D_00413F80.unkDC[0] = 0;
    D_00413F80.unkDC[1] = 0;
    D_00413F80.unkDC[2] = 0;
    D_00413F80.unkDC[3] = 0;
    D_00413F80.unkDC[4] = 0;
    D_00413F80.unkDC[5] = 0;
    func_001BB020(D_00413F80.weapon + ITEM_WEAPON_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_HELM] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_RING] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST);
    if (D_00413F80.unkF0 != 0) {
        D_00413F80.unkF0 = 150 / (s32)D_00413F80.unkF0;
        if (D_00413F80.unkF0 == 0) {
            D_00413F80.unkF0 = 1;
        }
    }
    if (D_00413F80.unkF4 != 0) {
        D_00413F80.unkF4 = 150 / (s32)D_00413F80.unkF4;
        if (D_00413F80.unkF4 == 0) {
            D_00413F80.unkF4 = 1;
        }
    }
    if (D_00413F80.unkF8 != 0) {
        D_00413F80.unkF8 = 150 / (s32)D_00413F80.unkF8;
        if (D_00413F80.unkF8 == 0) {
            D_00413F80.unkF8 = 1;
        }
    }
    if (D_00413F80.unkFC != 0) {
        D_00413F80.unkFC = 150 / (s32)D_00413F80.unkFC;
        if (D_00413F80.unkFC == 0) {
            D_00413F80.unkFC = 1;
        }
    }
}
