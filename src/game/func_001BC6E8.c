#include "kf_item_effect.h"
#include "kf_player.h"
#include "kf_message.h"

extern PlayerState D_00413F80;

void func_001BA310(void);
void func_001BB5E8(void);
int func_001BC990(int);

/* What using item `kind` does to the player itself: the Power Seed and the Widda Needle start a timer, the
   fruits raise unk2C ("Magic Power UP") or unk28 ("Physical UP"), the crystals raise unk24 and teach a magic. */
void Player_ApplyItem(int kind) {
    int marked;

    switch (kind) {
    case ITEM_POWER_SEED:
        D_00413F80.power_seed_timer = 1800;
        break;
    case ITEM_WIDDA_NEEDLE:
        D_00413F80.widda_needle_timer = 1800;
        break;
    case ITEM_WISE_MANS_FRUIT:
        if (D_00413F80.magic_base < 999) {
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_MAGIC_POWER_UP);
            D_00413F80.magic_base++;
        } else {
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
        }
        break;
    case ITEM_FOOLS_FRUIT:
        if (D_00413F80.power_base < 999) {
            D_00413F80.power_base++;
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_PHYSICAL_UP);
        } else {
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
        }
        break;
    case ITEM_MAGIC_CRYSTAL:
        D_00413F80.mp_base++;
        if (D_00413F80.mp_base > 999) {
            D_00413F80.mp_base = 999;
        }
        func_001BA310();
        ItemEffect_Start(kind, 0);
        break;
    case ITEM_FIRE_CRYSTAL:
    case ITEM_EARTH_CRYSTAL:
    case ITEM_WIND_CRYSTAL:
    case ITEM_WATER_CRYSTAL:
    case ITEM_LIGHT_CRYSTAL:
    case ITEM_DARK_CRYSTAL:
        marked = func_001BC990(kind - ITEM_FIRE_CRYSTAL);
        D_00413F80.mp_base++;
        if (D_00413F80.mp_base > 999) {
            D_00413F80.mp_base = 999;
        }
        func_001BA310();
        if (D_00413F80.unk264 == 0) {
            func_001BB5E8();
        }
        ItemEffect_Start(kind, marked);
        break;
    case ITEM_FIRE_CRYSTAL_2:
    case ITEM_EARTH_CRYSTAL_2:
    case ITEM_WIND_CRYSTAL_2:
    case ITEM_WATER_CRYSTAL_2:
    case ITEM_LIGHT_CRYSTAL_2:
    case ITEM_DARK_CRYSTAL_2:
        marked = func_001BC990(kind - ITEM_FIRE_CRYSTAL_2);
        D_00413F80.mp_base += 5;
        if (D_00413F80.mp_base > 999) {
            D_00413F80.mp_base = 999;
        }
        func_001BA310();
        if (D_00413F80.unk264 == 0) {
            func_001BB5E8();
        }
        ItemEffect_Start(kind, marked);
        break;
    }
}
