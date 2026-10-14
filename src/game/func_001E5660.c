#include "kf_player.h"

extern PlayerState D_00413F80;

/* Tells whether the item numbered value is the one the player has in the given equipment slot. */
int func_001E5660(u32 slot, s32 kind, s32 value) {
    switch (slot) {
        case 0:
            if (value == D_00413F80.weapon + ITEM_WEAPON_FIRST) {
                return TRUE;
            }
            break;
        case 1:
            if (value == D_00413F80.equipped_magic) {
                return TRUE;
            }
            break;
        case 2:
            if (value == D_00413F80.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 3:
            if (value == D_00413F80.equipment[EQUIP_HELM] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 4:
            if (value == D_00413F80.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 5:
            if (value == D_00413F80.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 6:
            if (value == D_00413F80.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 7:
            if (value == D_00413F80.equipment[EQUIP_RING] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 8:
            if (value == D_00413F80.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 9:
            if (value == D_00413F80.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 10:
            if ((kind == 11 && D_00413F80.shortcut_type < 2) || (kind == 1 && D_00413F80.shortcut_type == 2)) {
                if (value == D_00413F80.shortcut) {
                    return TRUE;
                }
            }
            break;
    }
    return FALSE;
}
