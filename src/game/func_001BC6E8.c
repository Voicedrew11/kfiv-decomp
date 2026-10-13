#include "kf_item_effect.h"
#include "kf_player.h"
#include "kf_message.h"

extern PlayerState D_00413F80;

void func_001BA310(void);
void func_001BB5E8(void);
int func_001BC990(int);

/* What using item `kind` does to the player: starts a timer, raises one of unk24/unk28/unk2C (999 at most),
   or marks a slot of spell_levels. */
void func_001BC6E8(int kind) {
    int marked;

    switch (kind) {
    case 0x15:
        D_00413F80.unk19C = 1800;
        break;
    case 0x16:
        D_00413F80.unk1A0 = 1800;
        break;
    case 0x1B:
        if (D_00413F80.unk2C < 999) {
            KF_Message_ShowNotice(0, 0, 0, 1, 0x2D, 0x2B);
            D_00413F80.unk2C++;
        } else {
            KF_Message_ShowNotice(0, 0, 0, 1, 0x2D, 0xA);
        }
        break;
    case 0x1C:
        if (D_00413F80.unk28 < 999) {
            D_00413F80.unk28++;
            KF_Message_ShowNotice(0, 0, 0, 1, 0x2D, 0x2A);
        } else {
            KF_Message_ShowNotice(0, 0, 0, 1, 0x2D, 0xA);
        }
        break;
    case 0x46:
        D_00413F80.unk24++;
        if (D_00413F80.unk24 > 999) {
            D_00413F80.unk24 = 999;
        }
        func_001BA310();
        KF_ItemEffect_Start(kind, 0);
        break;
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
    case 0x4C:
        marked = func_001BC990(kind - 0x47);
        D_00413F80.unk24++;
        if (D_00413F80.unk24 > 999) {
            D_00413F80.unk24 = 999;
        }
        func_001BA310();
        if (D_00413F80.unk264 == 0) {
            func_001BB5E8();
        }
        KF_ItemEffect_Start(kind, marked);
        break;
    case 0x4D:
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
    case 0x52:
        marked = func_001BC990(kind - 0x4D);
        D_00413F80.unk24 += 5;
        if (D_00413F80.unk24 > 999) {
            D_00413F80.unk24 = 999;
        }
        func_001BA310();
        if (D_00413F80.unk264 == 0) {
            func_001BB5E8();
        }
        KF_ItemEffect_Start(kind, marked);
        break;
    }
}
