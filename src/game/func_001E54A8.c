#include "kf_menu.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern MenuItem **D_00420D60[11];
extern MenuItem D_004231F0[70];
extern MenuItem D_00423650[30];
extern MenuItem D_00423830[25];
extern MenuItem D_004239C0[25];
extern MenuItem D_00423B50[25];
extern MenuItem D_00423CE0[25];
extern MenuItem D_00423E70[10];
extern MenuItem D_00423F10[10];
extern MenuItem D_00423FB0[10];
extern MenuItem D_00424050[];
extern MenuItem D_00424600[];

/* Empties one equipment slot: its menu entry goes back to the first item of its list and the player's number to the "nothing" one. */
void func_001E54A8(s8 slot) {
    switch (slot) {
        case 0:
            *D_00420D60[0] = D_004231F0;
            D_00413F80.weapon = 0;
            break;
        case 1:
            *D_00420D60[1] = D_00424050;
            D_00413F80.equipped_magic = 0;
            break;
        case 2:
            *D_00420D60[2] = D_00423650;
            D_00413F80.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
            break;
        case 3:
            *D_00420D60[3] = D_00423830;
            D_00413F80.equipment[EQUIP_HELM] = ITEM_WORN_INDEX(ITEM_HELM_NONE);
            break;
        case 4:
            *D_00420D60[4] = D_004239C0;
            D_00413F80.equipment[EQUIP_ARMOR] = ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
            break;
        case 5:
            *D_00420D60[5] = D_00423B50;
            D_00413F80.equipment[EQUIP_GAUNTLET] = ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
            break;
        case 6:
            *D_00420D60[6] = D_00423CE0;
            D_00413F80.equipment[EQUIP_BOOT] = ITEM_WORN_INDEX(ITEM_BOOT_NONE);
            break;
        case 7:
            *D_00420D60[7] = D_00423E70;
            D_00413F80.equipment[EQUIP_RING] = ITEM_WORN_INDEX(ITEM_RING_NONE);
            break;
        case 8:
            *D_00420D60[8] = D_00423F10;
            D_00413F80.equipment[EQUIP_BRACELET] = ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
            break;
        case 9:
            *D_00420D60[9] = D_00423FB0;
            D_00413F80.equipment[EQUIP_AMULET] = ITEM_WORN_INDEX(ITEM_AMULET_NONE);
            break;
        case 11:
            *D_00420D60[10] = D_00424600;
            D_00413F80.shortcut_type = 0;
            D_00413F80.shortcut = 0;
            break;
    }
}
