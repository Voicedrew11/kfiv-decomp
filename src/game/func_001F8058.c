#include "kf_game.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_pad.h"
#include "kf_state.h"
#include "kf_message.h"

extern f32 D_00425510[0x100];
extern s32 D_00426600[3];               /* which save each of the three lines shows */
extern s32 D_00426610[3];               /* the line each save is shown on */
extern u8 D_0036A130[];
extern u32 D_00446A64;
extern u32 D_00446AAC;
extern SpritePacket *D_00446BF8;
extern SpritePacket *D_00446C58;
extern s8 D_00446C70;
extern MenuColor D_00446C78;
extern u16 D_00446DE8;
extern s32 D_0044731C;
extern s32 D_00447324;
extern float D_00446C04;
extern char D_00446C08;
extern int (*D_00446CF8)();
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
void func_00103C18(void);
s32 func_0019CDC0(s32);
void func_001D20E0(void);
int func_001E5210(u8);
f32 func_001E6A20(s32, s32, f32);
void func_001F71D8(void);
s32 func_001F9648(void); // match: the result is used here
s32 func_001F9740(s8);
int func_001F9FD0(int, int, int);
s32 func_001FB6F8(void);
s32 func_001FB948(s32);
s32 func_001FB960(s32);
char func_001FB980(int);
s32 func_001FBA18(s32, s32, s32);
s32 func_001FBA58(s32);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor); // match: the definition takes the same eight bytes as ColorBytes and returns s32
s32 func_001FDD28(u16 *, s32);
void func_00203F28(int, int, int);
void func_00207478(int, int, int, int);

// match: called as a void function, so the result register is free at once
#define SET_PART_MODE(mode) ((void (*)(u8))func_001E5210)(mode)
// match: this file hands func_001E5210 a signed byte
#define SET_PART_MODE_OF(packet) ((void (*)(s8))func_001E5210)((s8)(packet)->unk42)

/* The save list of the save screen: shows the three saves newest first, moves between them, asks before
   overwriting, starts the save and reorders the lines once it is written. */
void func_001F8058(void) {
    CardSaveInfo *info[3];
    MenuCursorBar bar;
    s32 i;
    s32 first_arg;
    s32 second_arg;
    s32 kind;
    s32 slot;
    s32 group;
    s32 *table;
    char *text;
    GsPackedWord word;
    PadReport *report;
    s32 same_count;
    s32 save_state;
    s32 line;
    s32 swapped;

    info[0] = (CardSaveInfo *)func_001FB948(D_00426600[0]);
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
    info[1] = (CardSaveInfo *)func_001FB948(D_00426600[1]);
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
    info[2] = (CardSaveInfo *)func_001FB948(D_00426600[2]);
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

    if ((D_005CF3C0.unk13 == -1 && (D_005CF3C0.unk08 == 0 || D_005CF3C0.unk08 != func_001FBA58(D_005CF3C0.unk0C))) ||
        (D_005CF3C0.unk13 == 2 && D_005CF3C0.unk08 != func_001FBA58(D_005CF3C0.unk0C))) {
        D_005CF3C0.unk08 = 0;
        for (i = 0; i < 3; i++) {
            MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] = func_001FB980(i);
        }
        for (i = 0; i < 3; i++) {
            if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] != 1) {
                switch (i) {
                case 0:
                    if (D_005CF3C0.unk1F > 0 || D_005CF3C0.unk20 > 0) {
                        first_arg = 1;
                        second_arg = 0;
                    } else {
                        first_arg = 0;
                        second_arg = 1;
                    }
                    break;
                case 1:
                    if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk20 > 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk20 == 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk20 > 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk20 == 0) { // match: no else: a negative byte sets neither argument
                        second_arg = 1;
                        first_arg = 1;
                    }
                    break;
                case 2:
                    if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk1F > 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk1F == 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk1F > 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk1F == 0) { // match: a last test, not an else: retail keeps a path that sets neither
                        second_arg = 1;
                        first_arg = 2;
                    }
                    break;
                }
                if (func_001FBA18(D_005CF3C0.unk0C, first_arg, second_arg) > 0) {
                    MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] = 0;
                } else {
                    MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] = -1;
                }
            }
        }
    }

    for (i = 0; i < D_00446A64; i++) {
        kind = D_00446BF8[i].unk3E;
        slot = D_00446BF8[i].slot;
        ((int (*)(s8))func_001E5210)((s8)D_00446BF8[i].unk42); // match: a signed byte, see SET_PART_MODE_OF
        switch (kind) {
        case 2:
            func_00207478((int)&D_00446BF8[i], 0x3EA, 0, D_00425510[i]);
            break;
        case 3:
            group = slot / 3;
            if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[group]] == 1) {
                func_00207478((int)&D_00446BF8[i], 0x3EB, group * 3, D_00425510[i]);
            } else if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[group]] == 0) {
                func_00207478((int)&D_00446BF8[i], 0x3EB, group * 3 + 1, D_00425510[i]);
            }
            break;
        case 25:
            if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[slot]] != -1) {
                func_00207478((int)&D_00446BF8[i], 0x401, D_005CF3C0.unk0D, D_00425510[i]);
            }
            break;
        default:
            func_00207478((int)&D_00446BF8[i], kind + 1000, D_005CF3C0.unk0D, D_00425510[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    table = (s32 *)Message_GetFontText(D_00446F38);
    word.raw = 0x000A87507C107120;
    for (i = 0; i < 3; i++) {
        if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[i]] == 1) {
            kind = table[info[i]->unk04 + 1]; // match: the text's offset in the kind variable: retail keeps both in one register
            if (kind != 0) {
                text = (char *)table + kind;
                func_001FDB88(word);
                func_001FDCD0(D_00446C78);
                func_001FDD28((TradeValue *)text, 0x18);
            }
        }
        word.f.mid16 += 0x340;
    }

    for (i = 0; i < D_00446AAC; i++) {
        kind = D_00446C58[i].unk3E;
        SET_PART_MODE_OF(&D_00446C58[i]);
        if (kind == 3) {
            switch (D_005CF3C0.unk13) {
            case 6:
                func_00203F28((int)&D_00446C58[i], kind, 6);
                break;
            case 7:
                func_00203F28((int)&D_00446C58[i], kind, 7);
                break;
            case 0:
                func_00203F28((int)&D_00446C58[i], kind, 0);
                break;
            case 2:
                func_00203F28((int)&D_00446C58[i], kind, 2);
                break;
            case 20:
                func_00203F28((int)&D_00446C58[i], kind, 20);
                break;
            case 10:
                func_00203F28((int)&D_00446C58[i], kind, 10);
                break;
            case 11:
                func_00203F28((int)&D_00446C58[i], kind, 11);
                break;
            }
        } else {
            switch (D_005CF3C0.unk13) {
            case 0:
            case 2:
            case 6:
            case 7:
            case 10:
            case 11:
            case 20:
                func_00207478((int)&D_00446C58[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
        }
        SET_PART_MODE(0);
    }

    if (D_005CF3C0.unk13 == 6 || D_005CF3C0.unk13 == 7) {
        bar.first.f.lo16 = 0x7490;
        bar.first.f.mid16 = D_005CF3C0.unk0E * 0xC0 + 0x80A0;
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1720;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFF] = func_001E6A20(0x10, 0x20, D_00425510[0xFF]);
        bar.alpha = D_00425510[0xFF];
        func_001FC5E8(&bar); // match: the float at bar+0x24 that func_001FC5E8 copies is never set
    }

    D_005CF3C0.unk00 = func_001F9648();
    if (func_001FB960(D_005CF3C0.unk0C) == 0) {
        D_005CF3C0.unk00 = -1;
    }
    if (D_005CF3C0.unk00 < 0) {
        if (D_005CF3C0.unk13 == 0 || D_005CF3C0.unk13 == 11) {
            D_005CF3C0.unk13 = 11;
        } else {
            D_005CF3C0.unk13 = 10;
        }
    }
    if (D_005CF3C0.unk13 == 0 && func_001FB6F8() == 0) {
        D_005CF3C0.unk13 = 2;
        if (D_00447324 != 0) {
            D_00447324 = 0;
            same_count = 0;
            if (D_00426610[0] == D_00426600[0]) {
                same_count++;
            }
            if (D_00426610[1] == D_00426600[1]) {
                same_count++;
            }
            if (D_00426610[2] == D_00426600[2]) {
                same_count++;
            }
            for (i = 0; i < 3; i++) {
                if (i != D_005CF3C0.unk0D && D_00426610[i] >= D_00426610[D_005CF3C0.unk0D]) {
                    D_00426610[i]++;
                }
            }
            if (D_00426600[D_005CF3C0.unk0D] != 0) {
                if (same_count == 1) {
                    swapped = D_00426600[D_00426610[D_005CF3C0.unk0D]];
                    D_00426600[D_00426610[D_005CF3C0.unk0D]] = D_00426600[D_005CF3C0.unk0D];
                    D_00426600[D_005CF3C0.unk0D] = swapped;
                } else if (same_count == 2) {
                    if (D_00426600[D_005CF3C0.unk0D] != 1) {
                        for (i = 0; i < 3; i++) {
                            if (D_00426600[i] == 1) {
                                break;
                            }
                        }
                        line = i;
                        // match: the group variable as the second counter: retail keeps both in one register
                        for (group = 0; group < 3; group++) {
                            if (D_00426600[group] == 2) {
                                break;
                            }
                        }
                        D_00426600[group] = 1;
                        D_00426600[line] = 2;
                    }
                }
            }
        }
    }

    report = Pad_GetReport(0, 0);
    if ((report->pressed & 0x4000) && D_0044731C != 0xFF) {
        if (D_005CF3C0.unk13 != 0) {
            report = Pad_GetReport(0, 0);
            if (report->held & 0x50) {
                D_005CF3C0.unk1D = 1;
            } else {
                D_005CF3C0.unk1D = 3;
            }
        }
    }
    if (D_005CF3C0.unk1D == 2) {
        D_005CF3C0.unk1D = 3;
    }
    if (D_005CF3C0.unk1D == 1) {
        report = Pad_GetReport(0, 0);
        if (report->held & 0x50) {
            if (D_00446DE8 & 0x50) {
                D_005CF3C0.unk1D = 2;
            }
        } else {
            D_005CF3C0.unk1D = 3;
        }
    }

    if (D_005CF3C0.unk13 != 0 && D_005CF3C0.unk18 == 0) {
        if (D_005CF3C0.unk1D == 3) {
            D_005CF3C0.unk1D = 0;
            switch (D_005CF3C0.unk13) {
            case -1:
                func_0019CDC0(0);
                save_state = func_001FB980(D_00426610[D_005CF3C0.unk0D]);
                if (save_state == 0) {
                    if (func_001FBA18(D_005CF3C0.unk0C, 1, 0) > 0 &&
                        MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_005CF3C0.unk0D] != -1) {
                        D_00447324 = 1;
                        D_005CF3C0.unk13 = 6;
                        D_005CF3C0.unk08 = func_001FBA58(D_005CF3C0.unk0C);
                    } else {
                        D_005CF3C0.unk13 = 20;
                        D_00447324 = 0;
                    }
                } else if (save_state == 1) {
                    D_005CF3C0.unk13 = 7;
                }
                break;
            case 6:
            case 7:
                if (D_005CF3C0.unk0E == 0) {
                    func_0019CDC0(0);
                    D_005CF3C0.unk13 = 0;
                    func_001D20E0();
                    func_00103C18();
                    func_001F9FD0(D_005CF3C0.unk0C, D_00426610[D_005CF3C0.unk0D], (int)D_0036A130);
                } else {
                    func_0019CDC0(2);
                    D_005CF3C0.unk13 = -1;
                }
                D_005CF3C0.unk0E = 1;
                break;
            case 2:
            case 20:
                func_0019CDC0(0);
                D_005CF3C0.unk13 = -1;
                break;
            case 10:
            case 11:
                func_0019CDC0(0);
                D_00446C70 = 3;
                D_005CF3C0.unk18 = 1;
                break;
            }
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & 0x2000) && D_0044731C != 0xFF && D_005CF3C0.unk1D == 0) {
                func_0019CDC0(2);
                switch (D_005CF3C0.unk13) {
                case -1:
                    D_00446C70 = 2;
                    D_005CF3C0.unk18 = 1;
                    break;
                case 10:
                case 11:
                    D_00446C70 = 3;
                    D_005CF3C0.unk18 = 1;
                    break;
                case 6:
                case 7:
                    D_005CF3C0.unk0E = 1;
                case 2:
                case 20:
                    D_005CF3C0.unk13 = -1;
                    break;
                }
            } else if (D_00446DE8 & 0x40) {
                switch (D_005CF3C0.unk13) {
                case -1:
                    for (i = 0; i < 3; i++) {
                        D_005CF3C0.unk0D = ((s8)(D_005CF3C0.unk0D + 1) + 3) % 3;
                        if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_005CF3C0.unk0D] >= 0) {
                            if (i != 2) {
                                func_0019CDC0(1);
                            }
                            break;
                        }
                    }
                    break;
                case 6:
                case 7:
                    func_0019CDC0(1);
                    D_005CF3C0.unk0E = (D_005CF3C0.unk0E + 1) & 1;
                    break;
                }
            } else if (D_00446DE8 & 0x10) {
                switch (D_005CF3C0.unk13) {
                case -1:
                    for (i = 0; i < 3; i++) {
                        D_005CF3C0.unk0D = ((s8)(D_005CF3C0.unk0D - 1) + 3) % 3;
                        if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_005CF3C0.unk0D] >= 0) {
                            if (i != 2) {
                                func_0019CDC0(1);
                            }
                            break;
                        }
                    }
                    break;
                case 6:
                case 7:
                    func_0019CDC0(1);
                    D_005CF3C0.unk0E = (D_005CF3C0.unk0E + 1) & 1;
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
                D_005CF3C0.unk18 = 0;
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
                D_00446C04 = 128.0f;
                D_00446CF8 = (int (*)())func_001F71D8;
                D_00446C08 = 0;
                D_005CF3C0.unk0D = 1;
                D_005CF3C0.unk0E = 1;
                if (D_00446C70 == 2) {
                    func_001F9740(0);
                } else {
                    func_001F9740(1);
                }
                D_00446C70 = 0;
                break;
            }
        }
        break;
    }
}
