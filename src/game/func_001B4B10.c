#include "kf_player.h"

extern PlayerItemDef weapon_defs[];
extern LevelDef D_00416D78[];
extern LevelRecord D_003679A0[];

/* Adds experience to an item's record, rolling over into levels (levels below 0x33 reset to 0x32). */
void func_001B4B10(int idx, int amount) {
    PlayerItemDef *item;
    LevelRecord *rec;
    LevelDef *def;

    if (idx != 0) {
        item = &weapon_defs[idx];
        def = &D_00416D78[idx];
        rec = &D_003679A0[idx];
        if (item->unk63 < 2 && rec->level >= 0x33) {
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
}
