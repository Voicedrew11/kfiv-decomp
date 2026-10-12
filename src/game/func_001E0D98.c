#include "kf_game.h"
#include "kf_menu.h"
#include "kf_packet.h"

extern MenuScreenState D_005CF2F0;
extern GameStatus D_0038CC50;
extern f32 D_00425510[0x100];
extern u8 D_0036A130[];
extern u32 D_00446A64;
extern u32 D_00446AAC;
extern SpritePacket *D_00446BF8;
extern SpritePacket *D_00446C58;
extern s8 D_00446C70;
extern s8 D_00446BDD;
extern MenuColor D_00446C78;
extern u16 D_00446DE8;
extern int D_004472D0;
extern float D_00446C04;
extern char D_00446C08;
extern void *D_00446B20;
extern int D_00446AC8;
extern s32 D_00446F38;
/* the numbers the text printer puts into the three save lines, twelve for each */
extern s32 D_00446DF4;
extern s32 D_00446DF8;
extern s32 D_00446DFC;
extern s32 D_00446E00;
extern s32 D_00446E04;
extern s32 D_00446E08;
extern s32 D_00446E0C;
extern s32 D_00446E10;
extern s32 D_00446E14;
extern s32 D_00446E18;
extern s32 D_00446E1C;
extern s32 D_00446E20;
extern s32 D_00446E24;
extern s32 D_00446E28;
extern s32 D_00446E2C;
extern s32 D_00446E30;
extern s32 D_00446E34;
extern s32 D_00446E38;
extern s32 D_00446E3C;
extern s32 D_00446E40;
extern s32 D_00446E44;
extern s32 D_00446E48;
extern s32 D_00446E4C;
extern s32 D_00446E50;
extern s32 D_00446E54;
extern s32 D_00446E58;
extern s32 D_00446E5C;
extern s32 D_00446E60;
extern s32 D_00446E64;
extern s32 D_00446E68;
extern s32 D_00446E6C;
extern s32 D_00446E70;
extern s32 D_00446E74;
extern s32 D_00446E78;
extern s32 D_00446E7C;
extern s32 D_00446E80;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
s32 func_001CFC58(s32);
f32 func_001D8300(f32, s32, s32);
void func_001E02F8(void);
int func_001E5210(u8);
s32 func_001F9438(void);
s32 func_001F9648(void); // match: the result is used here
s32 func_001F9740(s32);
int func_001FA4F0(int, int, int, int);
s32 func_001FB6F8(void);
s32 func_001FB948(s32);
s32 func_001FB960(s32);
s32 func_001FB980(s32);
s32 func_001FBA18(s32, s32, s32);
void func_001FBB50(s32);
void func_001FBB68(s32);
s32 func_001FBB88(s32);
void func_001FBDC8(void);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
void func_001FDD28(TradeValue *, s32);
void func_00203F28(int, int, int);
void func_00207478(int, int, int, int);
void *func_00217760(int, int);

// match: called as a void function, so the result register is free at once
#define SET_PART_MODE(mode) ((void (*)(u8))func_001E5210)(mode)

/* The save list of the memory card screen: shows the three saves, moves between them, asks before
   overwriting and starts the save or the load. */
void func_001E0D98(void) {
    CardSaveInfo *info[3];
    MenuCursorBar bar;
    s32 i;
    s32 first_arg;
    s32 second_arg;
    u8 kind;
    s32 slot;
    s32 group;
    s32 table;
    s32 entry;
    GsPackedWord word;
    PadReport *report;

    info[0] = (CardSaveInfo *)func_001FB948(0);
    D_00446DF4 = info[0]->unk0C;
    D_00446DF8 = info[0]->unk08;
    D_00446DFC = info[0]->unk28;
    D_00446E00 = info[0]->unk10;
    D_00446E04 = info[0]->unk14;
    D_00446E08 = info[0]->unk18;
    D_00446E0C = info[0]->unk1C;
    D_00446E10 = info[0]->unk20;
    D_00446E14 = info[0]->unk24;
    D_00446E18 = info[0]->unk17B;
    D_00446E1C = info[0]->unk17A;
    D_00446E20 = info[0]->unk179;
    info[1] = (CardSaveInfo *)func_001FB948(1);
    D_00446E24 = info[1]->unk0C;
    D_00446E28 = info[1]->unk08;
    D_00446E2C = info[1]->unk28;
    D_00446E30 = info[1]->unk10;
    D_00446E34 = info[1]->unk14;
    D_00446E38 = info[1]->unk18;
    D_00446E3C = info[1]->unk1C;
    D_00446E40 = info[1]->unk20;
    D_00446E44 = info[1]->unk24;
    D_00446E48 = info[1]->unk17B;
    D_00446E4C = info[1]->unk17A;
    D_00446E50 = info[1]->unk179;
    info[2] = (CardSaveInfo *)func_001FB948(2);
    D_00446E54 = info[2]->unk0C;
    D_00446E58 = info[2]->unk08;
    D_00446E5C = info[2]->unk28;
    D_00446E60 = info[2]->unk10;
    D_00446E64 = info[2]->unk14;
    D_00446E68 = info[2]->unk18;
    D_00446E6C = info[2]->unk1C;
    D_00446E70 = info[2]->unk20;
    D_00446E74 = info[2]->unk24;
    D_00446E78 = info[2]->unk17B;
    D_00446E7C = info[2]->unk17A;
    D_00446E80 = info[2]->unk179;

    if (D_005CF2F0.unk13 == -1) {
        for (i = 0; i < 3; i++) {
            MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] = func_001FB980(i);
        }
        for (i = 0; i < 3; i++) {
            if (MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] != 1) {
                switch (i) {
                case 0:
                    if (D_005CF2F0.unk1F != 0 || D_005CF2F0.unk20 != 0) {
                        first_arg = 1;
                        second_arg = 0;
                    } else {
                        first_arg = 0;
                        second_arg = 1;
                    }
                    break;
                case 1:
                    if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk20 != 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk20 == 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk20 != 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk20 == 0) {
                        second_arg = 1;
                        first_arg = 1;
                    }
                    break;
                case 2:
                    if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk1F != 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk1F == 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk1F != 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk1F == 0) { // match: a last test, not an else: retail keeps a path that sets neither
                        second_arg = 1;
                        first_arg = 2;
                    }
                    break;
                }
                if (func_001FBA18(D_005CF2F0.unk0C, first_arg, second_arg) > 0) {
                    MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] = 0;
                } else {
                    MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] = -1;
                }
            }
        }
    }

    for (i = 0; i < D_00446A64; i++) {
        kind = D_00446BF8[i].unk3E;
        slot = D_00446BF8[i].slot;
        func_001E5210(D_00446BF8[i].unk42);
        switch (kind) {
        case 2:
            func_00207478((int)&D_00446BF8[i], 0x3EA, 1, D_00425510[i]);
            break;
        case 3:
            group = slot / 3;
            if (MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[group] == 1) {
                func_00207478((int)&D_00446BF8[i], 0x3EB, group * 3, D_00425510[i]);
            }
            break;
        case 25:
            if (MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[slot] == 1) {
                func_00207478((int)&D_00446BF8[i], 0x401, D_005CF2F0.unk0D, D_00425510[i]);
            }
            break;
        default:
            func_00207478((int)&D_00446BF8[i], kind + 1000, D_005CF2F0.unk0D, D_00425510[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    table = func_001CFC58(D_00446F38);
    word.raw = 0x000A87507C107120;
    for (i = 0; i < 3; i++) {
        entry = ((s32 *)table)[info[i]->unk04 + 1];
        func_001FDB88(word);
        func_001FDCD0(D_00446C78);
        if (entry != 0) {
            func_001FDD28((TradeValue *)(table + entry), 0x18);
        }
        word.f.mid16 += 0x340;
    }

    for (i = 0; i < D_00446AAC; i++) {
        kind = D_00446C58[i].unk3E;
        SET_PART_MODE(D_00446C58[i].unk42);
        if (kind == 3) {
            switch (D_005CF2F0.unk13) {
            case 8:
                func_00203F28((int)&D_00446C58[i], kind, 8);
                break;
            case 1:
                func_00203F28((int)&D_00446C58[i], kind, 1);
                break;
            case 3:
                func_00203F28((int)&D_00446C58[i], kind, 3);
                break;
            case 10:
                func_00203F28((int)&D_00446C58[i], kind, 10);
                break;
            case 23:
                func_00203F28((int)&D_00446C58[i], kind, 23);
                break;
            }
        } else {
            switch (D_005CF2F0.unk13) {
            case 1:
            case 3:
            case 8:
            case 10:
            case 23:
                func_00207478((int)&D_00446C58[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
        }
        SET_PART_MODE(0);
    }

    if (D_005CF2F0.unk13 == 8) {
        bar.first.f.lo16 = 0x7490;
        bar.first.f.mid16 = D_005CF2F0.unk0E * 0xC0 + 0x80A0;
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1720;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFD] = func_001D8300(D_00425510[0xFD], 0x10, 0x20);
        bar.alpha = D_00425510[0xFD];
        func_001FC5E8(&bar); // match: the float at bar+0x24 that func_001FC5E8 copies is never set
    }

    D_005CF2F0.unk00 = func_001F9648();
    if (func_001FB960(D_005CF2F0.unk0C) == 0) {
        D_005CF2F0.unk00 = -1;
    }
    if (D_005CF2F0.unk13 == 1 && func_001FB6F8() == 0) {
        D_005CF2F0.unk13 = 3;
    }
    if (D_005CF2F0.unk00 < 0) {
        if (func_001FBB88(D_005CF2F0.unk0C) != 0 && !D_0038CC50.bit9) {
            D_005CF2F0.unk13 = 23;
            func_001FBB68(D_005CF2F0.unk0C);
            func_001F9740(0);
        } else if (D_005CF2F0.unk13 != 3) {
            if (!D_0038CC50.bit9) {
                D_005CF2F0.unk13 = 10;
            }
        }
    }
    if (D_0038CC50.bit9) {
        if (D_005CF2F0.unk13 == -1 && D_00446BDD == 0) {
            func_001F9438();
            D_005CF2F0.unk18 = 1;
            D_00446C70 = 2;
            D_00446BDD = 1;
        }
    }

    report = func_00217760(0, 0);
    if ((report->pressed & 0x4000) && D_004472D0 != 0xFF) {
        if (D_005CF2F0.unk13 != 1) {
            report = func_00217760(0, 0);
            if (report->held & 0x50) {
                D_005CF2F0.unk1D = 1;
            } else {
                D_005CF2F0.unk1D = 3;
            }
        }
    }
    if (D_005CF2F0.unk1D == 2) {
        D_005CF2F0.unk1D = 3;
    }
    if (D_005CF2F0.unk1D == 1) {
        report = func_00217760(0, 0);
        if (report->held & 0x50) {
            if (D_00446DE8 & 0x50) {
                D_005CF2F0.unk1D = 2;
            }
        } else {
            D_005CF2F0.unk1D = 3;
        }
    }

    if (D_005CF2F0.unk13 != 1 && D_005CF2F0.unk18 == 0) {
        if (D_005CF2F0.unk1D == 3) {
            D_005CF2F0.unk1D = 0;
            switch (D_005CF2F0.unk13) {
            case -1:
                func_0019CDC0(0);
                D_005CF2F0.unk13 = 8;
                break;
            case 8:
                if (D_005CF2F0.unk0E == 0) {
                    func_0019CDC0(0);
                    D_005CF2F0.unk13 = 1;
                    memset(D_0036A130, 0, 0x20000);
                    func_001FA4F0(D_005CF2F0.unk0C, D_005CF2F0.unk0D, (int)D_0036A130, 0x20000);
                } else {
                    func_0019CDC0(2);
                    D_005CF2F0.unk13 = -1;
                    D_005CF2F0.unk0E = 1;
                }
                break;
            case 3:
                func_0019CDC0(0);
                D_005CF2F0.unk13 = -1;
                break;
            case 10:
                func_0019CDC0(0);
                D_005CF2F0.unk18 = 1;
                if (!D_0038CC50.bit9) {
                    D_00446C70 = 3;
                    func_001FBB50(D_005CF2F0.unk0C);
                } else {
                    D_005CF2F0.unk13 = -1;
                    D_00446C70 = 2;
                    D_00446BDD = 1;
                }
                break;
            case 23:
                func_0019CDC0(0);
                D_005CF2F0.unk13 = -1;
                D_005CF2F0.unk0E = 1;
                break;
            }
        } else {
            report = func_00217760(0, 0);
            if ((report->pressed & 0x2000) && D_004472D0 != 0xFF && D_005CF2F0.unk1D == 0) {
                func_0019CDC0(2);
                switch (D_005CF2F0.unk13) {
                case 10:
                    D_005CF2F0.unk18 = 1;
                    if (!D_0038CC50.bit9) {
                        D_00446C70 = 3;
                        func_001FBB50(D_005CF2F0.unk0C);
                    } else {
                        D_005CF2F0.unk13 = -1;
                        D_00446C70 = 2;
                        D_00446BDD = 1;
                    }
                    break;
                case -1:
                    D_005CF2F0.unk18 = 1;
                    D_00446C70 = 3;
                    break;
                case 3:
                    D_005CF2F0.unk13 = -1;
                    break;
                case 8:
                case 23:
                    D_005CF2F0.unk13 = -1;
                    D_005CF2F0.unk0E = 1;
                    break;
                }
            } else if ((D_00446DE8 & 0x40) && D_005CF2F0.unk18 == 0) {
                switch (D_005CF2F0.unk13) {
                case -1:
                    D_005CF2F0.unk0D++;
                    for (i = 0; i < 3; i++) {
                        if (func_001FB980((D_005CF2F0.unk0D + i) % 3) != 0) {
                            break;
                        }
                    }
                    if (i != 2) {
                        func_0019CDC0(1);
                    }
                    D_005CF2F0.unk0D = (D_005CF2F0.unk0D + i) % 3;
                    break;
                case 8:
                    func_0019CDC0(1);
                    D_005CF2F0.unk0E = (D_005CF2F0.unk0E + 1) & 1;
                    break;
                }
            } else if ((D_00446DE8 & 0x10) && D_005CF2F0.unk18 == 0) {
                switch (D_005CF2F0.unk13) {
                case -1:
                    D_005CF2F0.unk0D--;
                    for (i = 0; i < 3; i++) {
                        if (func_001FB980((D_005CF2F0.unk0D - i + 3) % 3) != 0) {
                            break;
                        }
                    }
                    if (i != 2) {
                        func_0019CDC0(1);
                    }
                    D_005CF2F0.unk0D = (D_005CF2F0.unk0D - i + 3) % 3;
                    break;
                case 8:
                    func_0019CDC0(1);
                    D_005CF2F0.unk0E = (D_005CF2F0.unk0E + 1) & 1;
                    break;
                }
            }
        }
    }

    switch (D_00446C70) {
    case 0:
        D_00425510[0xFF] += 12.8f;
        if (D_00425510[0xFF] > 128.0f) {
            D_00425510[0xFF] = 128.0f;
        }
        D_00446C78.alpha = D_00425510[0xFF];
        for (i = 0; i < D_00446A64; i++) {
            D_00425510[i] += SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A64; i++) {
                    D_00425510[i] = SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha;
                }
                D_00446C70 = 1;
                D_005CF2F0.unk18 = 0;
                break;
            }
        }
        break;
    case 2:
    case 3:
        D_00425510[0xFF] -= 12.8f;
        if (D_00425510[0xFF] < 0.0f) {
            D_00425510[0xFF] = 0.0f;
        }
        D_00446C78.alpha = D_00425510[0xFF];
        for (i = 0; i < D_00446A64; i++) {
            D_00425510[i] -= SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446BDD++;
                D_00446C08 = 0;
                D_00446C04 = 128.0f;
                if (D_00446C70 == 2) {
                    if (D_00446BDD != 2) {
                        D_005CF2F0.unk16 = 1;
                        D_00446C70 = 0;
                        D_00446B20 = (void *)func_001E02F8;
                        D_00446BDD = 0;
                        func_001FBDC8();
                        D_00446AC8++;
                    }
                } else if (D_00446C70 == 3) {
                    D_005CF2F0.unk0E = 1;
                    D_00446B20 = (void *)func_001E02F8;
                    D_00446C70 = 0;
                    D_00446BDD = 0;
                    func_001F9740(0);
                }
                break;
            }
        }
        break;
    }
}
