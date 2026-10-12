#include "kf_player.h"
#include "kf_effect.h"

extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern LevelRecord D_003679A0[];
extern u8 spell_levels[6][5];
extern FxTableEntry D_00402DA8[];

/* Picks PlayerState.unk264: unk3D, else unk46, else what the current PlayerItemDef names, else the first
   marked slot of spell_levels whose effect is ready. */
void func_001BB5E8(void) {
    PlayerItemDef *def;
    int i;
    int j;
    int effect_id;

    if (D_00413F80.unk3D != 0) {
        D_00413F80.unk264 = D_00413F80.unk3D;
        return;
    }
    if (D_00413F80.unk48 == 2 && D_00413F80.unk46 != 0) {
        D_00413F80.unk264 = D_00413F80.unk46;
        return;
    }
    def = &D_0041B9B8[D_00413F80.unk3C];
    if (def->unk63 != 2) {
        if (def->unk42 != 0xFFFF) {
            D_00413F80.unk264 = def->unk42;
            return;
        }
        // match: the pointer form puts the sum in the index register
        if (def->unk52 != 0xFFFF && (D_003679A0 + D_00413F80.unk3C)->unk14 >= 2) {
            D_00413F80.unk264 = def->unk52;
            return;
        }
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            effect_id = 15 * i + 3 * j + spell_levels[i][j];
            if (spell_levels[i][j] != 0 && D_00402DA8[effect_id].unk102 == 1) {
                D_00413F80.unk264 = effect_id;
                return;
            }
        }
    }
}
