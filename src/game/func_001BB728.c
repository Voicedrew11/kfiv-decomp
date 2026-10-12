#include "kf_player.h"

extern LevelRecordDef D_0041E118[];
extern LevelRecord D_003682F0[];

#define ADD_SCALED(n) totals[n] += (def->unk00[n] + record->unk00[n]) * record->level / 100

/* Adds the item's nine values (base plus what was gained), scaled by its level in percent, to the totals. */
void func_001BB728(u16 *totals, u32 index) {
    LevelRecordDef *def;
    LevelRecord *record;

    if (index < 160) {
        def = &D_0041E118[index];
        record = &D_003682F0[index];
        ADD_SCALED(0);
        ADD_SCALED(1);
        ADD_SCALED(2);
        ADD_SCALED(3);
        ADD_SCALED(4);
        ADD_SCALED(5);
        ADD_SCALED(6);
        ADD_SCALED(7);
        ADD_SCALED(8);
    }
}
