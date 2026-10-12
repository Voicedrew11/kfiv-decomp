#include "kf_player.h"
#include "kf_effect.h"

extern PlayerState D_00413F80;
extern u16 D_00367940[6][5];
extern u8 D_00367980[6][5];
extern FxTableEntry D_00402DA8[];

void func_001CFA70(int, int, int, int, int, int, ...);

/* Counts a use of the effect: its slot's use count rises (by 3 for some), and at 50 and 150 uses the slot's
   level goes up. Every 100th call also raises PlayerState.unk2C. */
void func_001BBC88(int effect_id) {
    int slot;
    int row;
    int column;
    u32 uses;
    u16 clamped_uses;

    slot = (effect_id - 1) / 3;
    row = slot / 5;
    column = slot % 5;
    if ((u32)row < 6 && column >= 0 && column < 5 && D_00367940[row][column] < 150) {
        if (D_00402DA8[effect_id].unk102 == 0) {
            D_00367940[row][column] += 1;
        } else if ((u32)(effect_id - 0x10) < 3) {
            D_00367940[row][column] += 1;
        } else if ((u32)(effect_id - 0x37) < 3) {
            D_00367940[row][column] += 1;
        } else {
            D_00367940[row][column] += 3;
        }
        uses = D_00367940[row][column];
        if (uses > 150) {
            D_00367940[row][column] = 150;
            uses = 150;
        }
        clamped_uses = uses;
        if ((clamped_uses >= 50 && D_00367980[row][column] < 2)
            || (clamped_uses >= 150 && D_00367980[row][column] < 3)) {
            D_00367980[row][column]++;
            func_001CFA70(0, 0, 0, 0, 0x2D, 0x2D);
            if (D_00413F80.unk3D == effect_id) {
                D_00413F80.unk3D = effect_id + 1;
            } else if (D_00413F80.unk48 == 2 && D_00413F80.unk46 == effect_id) {
                D_00413F80.unk46++;
            }
        }
    }
    D_00413F80.unkD8 = (D_00413F80.unkD8 + 1) % 100;
    if (D_00413F80.unkD8 == 0 && D_00413F80.unk2C < 999) {
        D_00413F80.unk2C++;
        func_001CFA70(0, 0, 0, 0, 0x2D, 0x2B);
    }
}
