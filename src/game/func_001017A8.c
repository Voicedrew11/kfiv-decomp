#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_sound.h"

extern u8 D_00349680[0x10];
extern LevelRecord D_003679A0[];
extern SoundLoadQueue D_0038A130;
extern GameStatus D_0038CC50;
extern MapLoadState D_0038D610;
extern PlayerState D_00413F80;
extern PlayerState D_00414280;
extern TriggerTypeDef D_0041A6F8[];
extern PlayerItemDef D_0041B9B8[];
extern int D_00446F78;

int func_0019F6B0(void);
void func_00118788(void);
int func_001187D8(void);
int func_00102230(void);
void func_00102228(void);
void func_001B5778(void);
void func_001BF898(void);
void func_00101CE0(void);
void func_001E6180(s32);
void func_001EE730(void);
void func_001F6A38(void);
void func_001095E0(void);
int func_001D62C8(s8);
int func_001BC9E0(int);
void func_0011A438(int);
void func_00101D48(int);
void func_00101C88(void);
void func_0011A500(void);
void func_00102218(void);
void func_001187B0(void);
void func_00101EF0(void);

/* Opens the menu or screen GameStatus asks for (bits 2 to 5), once the sound loads and the map load allow it,
   then puts back what the menu changed and fades the game in again. */
void func_001017A8(void) {
    PlayerState *player = &D_00413F80;
    int menu_ran = FALSE;
    int faded_out = FALSE;
    u8 brightness = D_00349680[10];
    int load_held;
    PlayerItemDef *item_def;

    if (!D_0038CC50.bit2 && !D_0038CC50.bit3 && !D_0038CC50.bit4 && !D_0038CC50.bit5 && !D_0038CC50.bit8 &&
        !D_0038CC50.bit14) {
        return;
    }
    if (D_00446F78 == 0) {
        return;
    }
    if (D_0038CC50.unk30 & 1) {
        return;
    }
    if (player->unk1A8 != 0) {
        return;
    }
    if (D_0038A130.unk0C != 0) {
        return;
    }
    if (D_0038A130.unk18 != 0) {
        return;
    }
    if (D_0038A130.unk04 != 0 && func_0019F6B0() == 0) {
        return;
    }
    load_held = FALSE;
    if (D_0038D610.step >= 2 && D_0038D610.step <= 4) {
        func_00118788();
        if (func_001187D8() == 0) {
            return;
        }
        load_held = func_00102230();
        if (load_held) {
            func_00102228();
        }
    } else if (func_00102230() != 0) {
        return;
    }
    func_001B5778();
    if (D_0038CC50.bit14 && D_00413F80.unk254 >= 2) {
        func_001BF898();
        D_0038CC50.bit14 = FALSE;
    }
    if (D_0038CC50.bit3) {
        if (!faded_out) { // match: always true here, and so in the three branches below; without the test 39 instructions differ
            faded_out = TRUE;
            func_00101CE0();
        }
        D_0038CC50.bit3 = FALSE;
        menu_ran = TRUE;
        func_001E6180(D_0038CC50.unk2C);
        func_001095E0();
        func_001095E0();
    } else if (D_0038CC50.bit4) {
        if (!faded_out) {
            faded_out = TRUE;
            func_00101CE0();
        }
        D_0038CC50.bit4 = FALSE;
        menu_ran = TRUE;
        func_001EE730();
        func_001095E0();
        func_001095E0();
    } else if (D_0038CC50.bit5 && D_0038CC50.unk00 != 0) {
        if (!faded_out) {
            faded_out = TRUE;
            func_00101CE0();
        }
        D_0038CC50.bit5 = FALSE;
        func_001F6A38();
        func_001095E0();
        func_001095E0();
    } else if (D_0038CC50.bit2 && D_0038CC50.unk00 != 0) {
        if (!faded_out) {
            faded_out = TRUE;
            func_00101CE0();
        }
        D_0038CC50.bit2 = FALSE;
        D_0038CC50.bit6 = FALSE;
        D_0038CC50.unk28 = 0;
        menu_ran = TRUE;
        func_001D62C8(0);
        func_001095E0();
        func_001095E0();
    }
    if (menu_ran) {
        func_001BF898();
        if (D_0038CC50.unk28 != 0) {
            func_001BC9E0(D_0038CC50.unk28);
            D_0038CC50.unk28 = 0;
        }
        if (D_00413F80.unk3C != D_00414280.unk3C) {
            D_00413F80.unkBC = 0;
        }
        if (D_00413F80.unk3D != 0 && D_00414280.unk3D != D_00413F80.unk3D) {
            D_00413F80.unk264 = D_00413F80.unk3D;
            D_00413F80.unkC0 = 0;
        } else if (D_00413F80.unk48 == 2 &&
                   (D_00414280.unk48 != D_00413F80.unk48 || D_00413F80.unk46 != D_00414280.unk46)) {
            D_00413F80.unkC0 = 0;
            D_00413F80.unk264 = D_00413F80.unk46;
        } else if (D_00413F80.unk3C != D_00414280.unk3C) {
            item_def = &D_0041B9B8[D_00413F80.unk3C];
            if (item_def->unk63 != 2) {
                if (item_def->unk42 != 0xFFFF) {
                    D_00413F80.unkC0 = 0;
                    D_00413F80.unk264 = item_def->unk42;
                } else if (item_def->unk52 != 0xFFFF && D_003679A0[D_00413F80.unk3C].unk14 + 1 == 3) {
                    D_00413F80.unkC0 = 0;
                    D_00413F80.unk264 = item_def->unk52;
                }
            }
        }
        if (D_0038CC50.bit6 && D_0041A6F8[D_0038CC50.unk20].type == 6) {
            func_0011A438(D_0038CC50.unk20);
            D_0038CC50.unk20 = 0;
            D_0038CC50.bit6 = FALSE;
        }
    }
    if (D_0038CC50.bit8 && !D_0038CC50.bit9) {
        if (faded_out) {
            func_00101D48(brightness);
            faded_out = FALSE;
        } else {
            func_00101C88();
        }
        func_0011A500();
    }
    if (D_0038D610.step != 0) {
        if (load_held) {
            func_00102218();
        }
        func_001187B0();
    }
    if (faded_out) {
        if (!D_0038CC50.bit9) {
            func_00101D48(brightness);
        } else {
            func_00101EF0();
        }
    }
}
