#include "kf_player.h"

extern LevelRecord D_003682F0[];
extern LevelDef D_00417EF8[];

/* Adds experience to a record, rolling over into levels (levels below 0x33 reset to 0x32). */
void func_001B4DF8(int idx, int amount) {
    LevelRecord *rec = &D_003682F0[idx];
    LevelDef *def = &D_00417EF8[idx];

    if (rec->level >= 0x33 && idx != ITEM_WORN_INDEX(ITEM_SHIELD_NONE) && idx != ITEM_WORN_INDEX(ITEM_HELM_NONE)
        && idx != ITEM_WORN_INDEX(ITEM_ARMOR_NONE) && idx != ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE)
        && idx != ITEM_WORN_INDEX(ITEM_BOOT_NONE) && idx != ITEM_WORN_INDEX(ITEM_RING_NONE)
        && idx != ITEM_WORN_INDEX(ITEM_BRACELET_NONE) && idx != ITEM_WORN_INDEX(ITEM_AMULET_NONE)) {
        rec->exp += amount;
        while (rec->exp >= def->exp_per_level) {
            rec->exp -= def->exp_per_level;
            rec->level--;
            if (rec->level < 0x33) {
                rec->level = 0x32;
                rec->exp = 0;
                break;
            }
        }
    }
}
