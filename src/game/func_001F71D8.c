#include "kf_game.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_pad.h"
#include "kf_state.h"

extern f32 D_00425510[0x100];
extern SaveListRange D_00426600;
extern SaveListRange D_00426610;
extern u32 D_00446A60;
extern SpritePacket *D_00446BF4;
extern s8 D_00446C08;
extern s8 D_00446C09;
extern s8 D_00446C70;
extern int (*D_00446CF8)();
extern s32 D_00446CFC;
extern u16 D_00446DE8;
extern s32 D_0044731C;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001E5210(s32); // match: defined as int (u8); retail passes unk42 sign-extended and uses no result
f32 func_001E6A20(s32, s32, f32);
int func_001F8058();
s32 func_001F9648(void);
s32 func_001F9740(s8);
s32 func_001F9BA0(s32, char);
s32 func_001FA780(s32);
void func_001FAFB0(void);
s32 func_001FB6F8(void);
s32 func_001FB960(s32);
s32 func_001FB9E8(s32);
s32 func_001FBA78(void);
s8 func_001FBBD0(void);
char func_001FBBE0(int);
s8 func_001FBC00(s32);
s8 func_001FBC20(s32);
s8 func_001FBC40(s32);
void func_001FBC60(s32);
s32 func_001FBD38(void);
void func_001FBDC8(void);
void func_001FC5E8(MenuCursorBar *);
void func_00203F28(int, int, int);
void func_00207478(int, int, int, int);

/* The save screen: draws its parts, picks a card, asks the format and overwrite questions, runs the card
   checks and hands over to the save list (func_001F8058). */
void func_001F71D8(void) {
    MenuCursorBar bar;
    GsPackedWord unused_corner;
    u32 i;
    s32 card_result;
    u8 kind;
    s32 slot;
    s32 card_flags;
    PadReport *report;

    card_result = func_001F9648();
    if (func_001FB960(D_005CF3C0.unk0C) == 0 && D_005CF3C0.unk12 == 14) {
        card_result = -1;
    }
    if (D_005CF3C0.unk1B == 0 && func_001FBBD0() == 0) {
        D_005CF3C0.unk18 = 0;
        D_005CF3C0.unk1B = 1;
        if (func_001FBBE0(0) == 0 && func_001FBBE0(1) != 0) {
            D_005CF3C0.unk0C = 1;
        }
    }

    for (i = 0; i < D_00446A60; i++) {
        kind = D_00446BF4[i].unk3E;
        func_001E5210((s8)D_00446BF4[i].unk42);
        switch (kind) {
        case 2:
            func_00207478((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
            break;
        case 29:
            slot = D_00446BF4[i].slot;
            if (slot == 0 || slot == 2) {
                func_00207478((int)&D_00446BF4[i], kind + 1000, slot, D_00425510[i]);
            }
            break;
        case 31:
            if (D_005CF3C0.unk1B != 0) {
                if (D_00446C70 != 1) {
                    func_00207478((int)&D_00446BF4[i], kind + 1000, D_005CF3C0.unk0C, D_00425510[i]);
                } else {
                    func_00207478((int)&D_00446BF4[i], kind, D_005CF3C0.unk0C, D_00425510[i]);
                }
            }
            break;
        case 3:
            switch (D_005CF3C0.unk12) {
            case 4:
            case 5:
            case 10:
            case 12:
            case 13:
            case 14:
            case 15:
            case 21:
                func_00207478((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
            break;
        case 26:
            if (func_001FBBE0(0) == 0) {
                if (D_00446C70 == 0) {
                    func_00207478((int)&D_00446BF4[i], kind + 1000, 0x20, D_00425510[i]);
                } else if (D_00446C70 < 2) {
                    func_00203F28((int)&D_00446BF4[i], kind, 0x20);
                }
            } else if (D_00446C70 != 1) {
                func_00207478((int)&D_00446BF4[i], kind + 1000, 0x80, D_00425510[i]);
            } else {
                func_00203F28((int)&D_00446BF4[i], kind, 0x80);
            }
            break;
        case 27:
            if (func_001FBBE0(1) == 0) {
                if (D_00446C70 == 0) {
                    func_00207478((int)&D_00446BF4[i], kind + 1000, 0x20, D_00425510[i]);
                } else if (D_00446C70 < 2) {
                    func_00203F28((int)&D_00446BF4[i], kind, 0x20);
                }
            } else if (D_00446C70 != 1) {
                func_00207478((int)&D_00446BF4[i], kind + 1000, 0x80, D_00425510[i]);
            } else {
                func_00203F28((int)&D_00446BF4[i], kind, 0x80);
            }
            break;
        case 28:
            switch (D_005CF3C0.unk12) {
            case 4:
                func_00203F28((int)&D_00446BF4[i], kind, 4);
                break;
            case 5:
                func_00203F28((int)&D_00446BF4[i], kind, 5);
                break;
            case 12:
                func_00203F28((int)&D_00446BF4[i], kind, 12);
                break;
            case 13:
                func_00203F28((int)&D_00446BF4[i], kind, 13);
                break;
            case 14:
                func_00203F28((int)&D_00446BF4[i], kind, 14);
                break;
            case 15:
                func_00203F28((int)&D_00446BF4[i], kind, 15);
                break;
            case 21:
                func_00203F28((int)&D_00446BF4[i], kind, 21);
                break;
            case 10:
                func_00203F28((int)&D_00446BF4[i], kind, 10);
                break;
            case 22:
                func_00203F28((int)&D_00446BF4[i], kind, 22);
                break;
            }
            break;
        default:
            func_00207478((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
            break;
        }
        func_001E5210(0);
    }

    if (D_005CF3C0.unk12 == 12) {
        // match: a depth nothing reads; without it the 64-bit mask of the bar's depth field is loaded after the branch below
        unused_corner.f.mid24 = 0x5F370;
        bar.first.f.lo16 = 0x7580;
        bar.first.f.mid16 = D_005CF3C0.unk0D * 0xB0 + 0x80E0;
        if (D_005CF3C0.unk0D != 0) {
            bar.first.f.mid16 = D_005CF3C0.unk0D * 0xB0 + 0x80F0;
        }
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1540;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFF] = func_001E6A20(0x10, 0x20, D_00425510[0xFF]);
        bar.alpha = D_00425510[0xFF];
        func_001FC5E8(&bar); // match: the float at bar+0x24 that func_001FC5E8 copies is never set
    }

    if (D_005CF3C0.unk21 != 0) {
        if (func_001FBBD0() == 0) {
            D_005CF3C0.unk21 = 0;
            D_005CF3C0.unk18 = 0;
            if (D_005CF3C0.unk12 == 12) {
                if (D_005CF3C0.unk0D == 0) {
                    func_001FA780(D_005CF3C0.unk0C);
                    D_005CF3C0.unk12 = 13;
                    D_005CF3C0.unk14 = 1;
                } else {
                    D_005CF3C0.unk12 = -1;
                }
            } else if (D_005CF3C0.unk12 == 14) {
                if (func_001F9BA0(D_005CF3C0.unk0C, 0) == 0) {
                    D_005CF3C0.unk14 = 1;
                }
            } else if (D_005CF3C0.unk12 == 21) {
                D_005CF3C0.unk12 = -1;
            } else if (D_005CF3C0.unk12 != -1) {
                D_005CF3C0.unk12 = -1;
            } else if (func_001FBBE0(D_005CF3C0.unk0C) != 0) {
                if (func_001FBC00(D_005CF3C0.unk0C) == 0) {
                    D_005CF3C0.unk12 = 12;
                } else if (func_001FBC20(D_005CF3C0.unk0C) == 0 && func_001FB9E8(D_005CF3C0.unk0C) < 0) {
                    D_005CF3C0.unk12 = 21;
                    func_001F9740(1);
                } else if (func_001F9BA0(D_005CF3C0.unk0C, 1) == 0) {
                    D_005CF3C0.unk14 = 1;
                }
            } else if (D_005CF3C0.unk0C == 0) {
                D_005CF3C0.unk12 = 4;
            } else if (D_005CF3C0.unk0C == 1) {
                D_005CF3C0.unk12 = 5;
            }
        }
    } else if (D_005CF3C0.unk14 != 0) {
        if (func_001FB6F8() == 0) {
            if (D_005CF3C0.unk12 == 13) {
                D_005CF3C0.unk12 = 14;
                D_005CF3C0.unk0D = 1;
                D_005CF3C0.unk13 = -1;
            } else if (func_001FBD38() != 0 || func_001FB9E8(D_005CF3C0.unk0C) > 0) {
                D_005CF3C0.unk18 = 1;
                D_005CF3C0.unk13 = -1;
                D_00446C70 = 2;
                D_005CF3C0.unk0D = 0;
            } else {
                D_005CF3C0.unk12 = 21;
                func_001F9740(1);
            }
            D_005CF3C0.unk14 = 0;
        } else if (card_result < 0) {
            if (D_005CF3C0.unk12 == 13) {
                D_005CF3C0.unk12 = 15;
                D_005CF3C0.unk14 = 0;
                D_005CF3C0.unk0D = 1;
                func_001F9740(1);
                card_result = 0;
            } else if (D_005CF3C0.unk12 == -1) {
                if (func_001FBC40(D_005CF3C0.unk0C) != 0) {
                    if (func_001FBD38() != 0 || func_001FB9E8(D_005CF3C0.unk0C) > 0) {
                        D_005CF3C0.unk18 = 1;
                        D_00446C70 = 2;
                        D_005CF3C0.unk13 = -1;
                        D_005CF3C0.unk0D = 0;
                        func_001FBC60(D_005CF3C0.unk0C);
                    } else {
                        D_005CF3C0.unk12 = 21;
                        func_001F9740(1);
                    }
                } else {
                    D_005CF3C0.unk12 = 10;
                }
                card_result = 0;
                D_005CF3C0.unk14 = 0;
            }
        }
    }

    if (D_005CF3C0.unk11 != 0) {
        if (card_result < 0) {
            D_005CF3C0.unk18 = 1;
            D_00446C70 = 3;
        }
    } else if (D_005CF3C0.unk12 == 12 && func_001FBBE0(D_005CF3C0.unk0C) == 0) {
        D_005CF3C0.unk12 = 10;
        D_005CF3C0.unk0D = 1;
    } else if (card_result < 0) {
        D_005CF3C0.unk12 = 10;
        D_005CF3C0.unk0D = 1;
        D_005CF3C0.unk14 = 0;
        func_001F9740(1);
    } else if (D_005CF3C0.unk14 == 0 && D_005CF3C0.unk18 == 0) {
        if ((D_00446DE8 & 0xA0) && D_0044731C != 0xFF && D_005CF3C0.unk12 == -1) {
            func_0019CDC0(1);
            D_005CF3C0.unk0C = (D_005CF3C0.unk0C + 1) & 1;
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & 0x4000) && D_0044731C != 0xFF) {
                func_0019CDC0(0);
                D_005CF3C0.unk21 = 1;
                D_005CF3C0.unk18 = 1;
            } else {
                report = Pad_GetReport(0, 0);
                if ((report->pressed & 0x2000) && D_0044731C != 0xFF) {
                    func_0019CDC0(2);
                    if (D_005CF3C0.unk12 == 14) {
                        if (func_001F9BA0(D_005CF3C0.unk0C, 0) == 0) {
                            D_005CF3C0.unk14 = 1;
                        }
                    } else if (D_005CF3C0.unk12 != -1) {
                        D_005CF3C0.unk12 = -1;
                        D_005CF3C0.unk0D = 1;
                    } else {
                        D_005CF3C0.unk11 = 1;
                        func_001FAFB0();
                    }
                } else if ((D_00446DE8 & 0x10) && D_0044731C != 0xFF && D_005CF3C0.unk12 == 12) {
                    func_0019CDC0(1);
                    D_005CF3C0.unk0D = (D_005CF3C0.unk0D + 1) & 1;
                } else if ((D_00446DE8 & 0x40) && D_0044731C != 0xFF && D_005CF3C0.unk12 == 12) {
                    func_0019CDC0(1);
                    D_005CF3C0.unk0D = (D_005CF3C0.unk0D - 1) & 1;
                }
            }
        }
    }

    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446A60; i++) {
            // match: the cursor bar's level is stepped once for every part, inside the loop
            D_00425510[0xFF] += 1.6f;
            if (D_00425510[0xFF] > 16.0f) {
                D_00425510[0xFF] = 16.0f;
            }
            if (D_00446BF4[i].unk3E == 26) {
                if (func_001FBBE0(0) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x20;
                }
            } else if (D_00446BF4[i].unk3E == 27) {
                if (func_001FBBE0(1) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x20;
                }
            }
            D_00425510[i] += SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A60; i++) {
                    D_00425510[i] = SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha;
                }
                D_00446C70 = 1;
                if (D_005CF3C0.unk1B != 0) {
                    D_005CF3C0.unk18 = 0;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446A60; i++) {
            D_00425510[0xFF] -= 1.6f; // match: stepped once for every part here too
            if (D_00425510[0xFF] < 0.0f) {
                D_00425510[0xFF] = 0.0f;
            }
            if (D_00446BF4[i].unk3E == 26) {
                if (func_001FBBE0(0) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x20;
                }
            } else if (D_00446BF4[i].unk3E == 27) {
                if (func_001FBBE0(1) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x20;
                }
            }
            D_00425510[i] -= SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_00446C70 == 2) {
                    D_005CF3C0.unk1D = 0;
                    D_00446CF8 = func_001F8058;
                    D_005CF3C0.unk08 = 0;
                    D_005CF3C0.unk0D = 0;
                    D_00446C09 = 0;
                    D_00446C08 = 0;
                    card_flags = func_001FBA78();
                    if (card_flags != 0 && (card_flags & 1)) {
                        D_00426610.unk00 = 0;
                        if (card_flags & 2) {
                            D_00426610.unk04 = 1;
                            D_00426610.unk08 = 2;
                        } else {
                            D_00426610.unk04 = 1;
                            D_00426610.unk08 = 1;
                        }
                    } else {
                        D_00426610.unk00 = 0;
                        D_00426610.unk04 = 0;
                        D_00426610.unk08 = 0;
                    }
                    D_00426600.unk00 = 0;
                    D_00426600.unk04 = 1;
                    D_00426600.unk08 = 2;
                } else if (D_00446C70 == 3) {
                    D_005CF3C0.unk0D = 0;
                    D_00446CFC++;
                    func_001FBDC8();
                }
                D_005CF3C0.unk12 = -1;
                D_00446C70 = 0;
                break;
            }
        }
        break;
    }
}
