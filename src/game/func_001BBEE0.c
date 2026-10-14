#include "common.h"
#include "kf_message.h"

extern char D_003679A0[];
extern char D_00413F80[];
typedef struct {
    char pad0[0x14];
    u8 unk14;
    u8 pad15;
    u16 unk16;
} E22;

/* Counts a use of the weapon in hand (its row of D_003679A0): at 180 and 480 uses its level goes up ("Weapon Level UP").
   Every 100th call also raises PlayerState.power_base ("Physical UP"). */
void Player_CountWeaponUse(int weapon) {
    E22 *record = (E22 *)(D_003679A0 + weapon * 0x22);
    u16 v;
    int n;
    int t;
    int *p;

    v = record->unk16;
    if (v < 0x1E0) {
        n = v + 1;
        record->unk16 = n;
        if ((u16)n == 0xB4 || (u16)n == 0x1E0) {
            if (record->unk14 < 2) {
                record->unk14++;
                Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_WEAPON_LEVEL_UP);
            }
        }
    }
    p = (int *)D_00413F80;
    t = (p[53] + 1) % 100;
    p[53] = t;
    if (t == 0) {
        if (p[10] < 0x3E7) {
            p[10]++;
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_PHYSICAL_UP);
        }
    }
}
