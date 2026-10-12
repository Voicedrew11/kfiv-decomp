#include "kf_player.h"

extern PlayerItemDef D_0041B9B8[];
extern LevelDef D_00416D78[];
extern LevelRecord D_003679A0[];

/* Adds experience to an item's record, rolling over into levels (levels below 0x33 reset to 0x32). */
void func_001B4B10(int idx, int amount) {
    PlayerItemDef *item;
    LevelRecord *rec;
    LevelDef *def;

    if (idx != 0) {
        item = &D_0041B9B8[idx];
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
