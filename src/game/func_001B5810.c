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
    D_00413F80.unk2C = D_00413F80.unk28 = 20;
    D_00413F80.unk4C = 50000;
    D_00413F80.unk54 = 1;
    D_00413F80.unk20 = 50;
    D_00413F80.unk24 = 15;
    D_00413F80.unk50 = 0;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            spell_levels[i][j] = 0;
        }
    }
    D_00413F80.equipment[1] = 0x1E;
    D_00413F80.equipment[2] = 0x37;
    D_00413F80.equipment[3] = 0x50;
    D_00413F80.equipment[4] = 0x69;
    D_00413F80.equipment[5] = 0x82;
    D_00413F80.equipment[6] = 0x8C;
    D_00413F80.equipment[7] = 0x96;
    D_00413F80.position.x = D_00413F80.position.y = D_00413F80.position.z = D_00413F80.position.w = 0.0f;
    D_00413F80.unkB4 = D_00413F80.unk20;
    D_00413F80.unkB8 = D_00413F80.unk24;
    D_00413F80.rotation.y = D_00413F80.rotation.z = D_00413F80.rotation.w = 0.0f;
    D_00413F80.timed_flags[0] = 0;
    D_00413F80.timed_flags[1] = 0;
    D_00413F80.timed_flags[2] = 0;
    D_00413F80.timed_flags[3] = 0;
    D_00413F80.timed_flags[4] = 0;
    D_00413F80.timed_flags[5] = 0;
    D_00413F80.unk3C = 0;
    D_00413F80.equipment[0] = 0;
    D_00413F80.unk48 = 0;
    D_00413F80.unk46 = 0;
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
