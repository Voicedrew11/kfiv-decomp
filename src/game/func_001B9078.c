#include "kf_player.h"

extern PlayerState D_00413F80;
extern u32 D_00446ED4;

int rand(void);
void func_001B9610(void);
void func_001BB2E8(int value);
void func_001B4B10(int, int);
void func_001B4DF8(int idx, int amount);

/* What the ground under the player does to him: death, poison (by chance or at once), damage, or experience. */
void func_001B9078(void) {
    if (D_00413F80.unk1EC != 0) {
        D_00413F80.hp_now = 0;
        D_00413F80.unk1A4 = 0;
        if (D_00413F80.unk1A8 == 0) {
            func_001B9610();
        }
    }
    if (D_00413F80.unk1E0 == 0) {
        return;
    }
    switch (D_00413F80.unk1E4) {
    case 1:
        if ((u16)D_00413F80.timed_flags[0] == 0 && D_00413F80.flag_timers[0] == 0) {
            if (D_00413F80.unk1E8 % 60 == 0) {
                // match: the chance (at least 5 in 100) is worked out after the roll, inside the comparison
                if ((int)(rand() * 100.0f * 4.656613e-10f) < (20 - D_00413F80.unkDC[0] > 4 ? 20 - D_00413F80.unkDC[0] : 5)) {
                    D_00413F80.timed_flags[0] = 1800;
                    D_00413F80.unk2C4 = D_00446ED4;
                }
            }
        }
        break;
    case 2:
        if ((u16)D_00413F80.timed_flags[0] == 0 && D_00413F80.flag_timers[0] == 0) {
            if (D_00413F80.unk1E8 % 60 == 0) {
                // match: the chance (at least 5 in 100) is worked out after the roll, inside the comparison
                if ((int)(rand() * 100.0f * 4.656613e-10f) < (50 - D_00413F80.unkDC[0] > 4 ? 50 - D_00413F80.unkDC[0] : 5)) {
                    D_00413F80.timed_flags[0] = 1800;
                    D_00413F80.unk2C4 = D_00446ED4;
                }
            }
        }
        break;
    case 3:
        if ((u16)D_00413F80.timed_flags[0] == 0) {
            D_00413F80.timed_flags[0] = 1800;
            D_00413F80.unk2C4 = D_00446ED4;
        }
        break;
    case 4:
        func_001BB2E8(10);
        break;
    case 5:
        func_001BB2E8(1);
        func_001B4B10(D_00413F80.weapon, 1);
        func_001B4DF8(D_00413F80.equipment[EQUIP_SHIELD], 1);
        func_001B4DF8(D_00413F80.equipment[EQUIP_HELM], 1);
        func_001B4DF8(D_00413F80.equipment[EQUIP_ARMOR], 1);
        func_001B4DF8(D_00413F80.equipment[EQUIP_GAUNTLET], 1);
        func_001B4DF8(D_00413F80.equipment[EQUIP_BOOT], 1);
        break;
    }
}
