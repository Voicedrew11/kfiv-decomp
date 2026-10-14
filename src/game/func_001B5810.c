#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerState D_00414280;
extern PlayerScreenTint D_00414580;
extern u8 spell_levels[6][5];

void *memset(void *, s32, u32);
void func_001BA310(void);
void func_001B96C8(void);

/* Sets the player up for a new game. */
void func_001B5810(void) {
    int i;
    int j;

    memset(&D_00413F80, 0, sizeof(PlayerState));
    memset(&D_00414280, 0, sizeof(PlayerState));
    memset(&D_00414580, 0, sizeof(PlayerScreenTint));
    D_00413F80.magic_base = D_00413F80.power_base = 20;
    D_00413F80.gold = 50000;
    D_00413F80.level = 1;
    D_00413F80.hp_base = 50;
    D_00413F80.mp_base = 15;
    D_00413F80.experience = 0;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            spell_levels[i][j] = 0;
        }
    }
    D_00413F80.equipment[EQUIP_HELM] = ITEM_WORN_INDEX(ITEM_HELM_NONE);
    D_00413F80.equipment[EQUIP_ARMOR] = ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
    D_00413F80.equipment[EQUIP_GAUNTLET] = ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
    D_00413F80.equipment[EQUIP_BOOT] = ITEM_WORN_INDEX(ITEM_BOOT_NONE);
    D_00413F80.equipment[EQUIP_RING] = ITEM_WORN_INDEX(ITEM_RING_NONE);
    D_00413F80.equipment[EQUIP_BRACELET] = ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
    D_00413F80.equipment[EQUIP_AMULET] = ITEM_WORN_INDEX(ITEM_AMULET_NONE);
    D_00413F80.position.x = D_00413F80.position.y = D_00413F80.position.z = D_00413F80.position.w = 0.0f;
    D_00413F80.hp_now = D_00413F80.hp_base;
    D_00413F80.mp_now = D_00413F80.mp_base;
    D_00413F80.rotation.y = D_00413F80.rotation.z = D_00413F80.rotation.w = 0.0f;
    D_00413F80.timed_flags[0] = 0;
    D_00413F80.timed_flags[1] = 0;
    D_00413F80.timed_flags[2] = 0;
    D_00413F80.timed_flags[3] = 0;
    D_00413F80.timed_flags[4] = 0;
    D_00413F80.timed_flags[5] = 0;
    D_00413F80.weapon = 0;
    D_00413F80.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
    D_00413F80.shortcut_type = 0;
    D_00413F80.shortcut = 0;
    D_00413F80.unkBC = 0;
    D_00413F80.unkC0 = 0;
    D_00413F80.unk1A4 = 0;
    D_00413F80.unk1A6 = 0;
    D_00413F80.unk264 = 0;
    D_00413F80.rotation.x = 0.0f;
    D_00413F80.unk250 = 0xFF;
    D_00413F80.map_index = 0xFFFF;
    D_00413F80.unk1C4 = 0.0f;
    D_00413F80.unk1A8 = 0;
    D_00413F80.unk1C0 = 0;
    func_001BA310();
    func_001B96C8();
    D_00414280 = D_00413F80;
}
