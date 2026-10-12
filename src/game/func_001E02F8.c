#include "kf_game.h"
#include "kf_menu.h"
#include "kf_packet.h"

extern MenuScreenState D_005CF2F0;
extern GameStatus D_0038CC50;
extern f32 D_00425510[0x100];
extern u32 D_00446A60;
extern SpritePacket *D_00446BF4;
extern s8 D_00446C70;
extern s8 D_00446C94;
extern s8 D_00446BDC;
extern u8 D_00447224;
extern u16 D_00446DE8;
extern int D_004472D0;
extern float D_00446C04;
extern char D_00446C08;
extern void *D_00446B20;
extern void *D_00446B24;
extern int D_00446AC8;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001DF308(MenuScreenContext *);
void func_001E0D98(void);
int func_001E5210(u8);

// match: called as a void function, so the result register is free at once
#define SET_PART_MODE(mode) ((void (*)(u8))func_001E5210)(mode)
s32 func_001F9648(void); // match: the result is used here
s32 func_001F9740(s32);
s32 func_001F9BA0(s32, s32); // match: the result is used here
void func_001FAFB0(void);
s32 func_001FB6F8(void);
s32 func_001FB9B0(void);
s32 func_001FBA78(void);
void func_001FBB68(s32);
s32 func_001FBBB8(void);
char func_001FBBE0(int);
void func_00203F28(int, int, int);
void func_00207478(int, int, int, int);
void *func_00217760(int, int);

/* The memory card screen: draws its parts, picks a card slot and starts the load or save screen (func_001E0D98). */
void func_001E02F8(void) {
    u32 i;
    s32 card_result;
    u8 kind;
    PadReport *report;

    card_result = func_001F9648();
    if (D_005CF2F0.unk1B == 0 && func_001FBBB8() == 0) {
        D_005CF2F0.unk18 = 0;
        D_005CF2F0.unk1B = 1;
        if (func_001FBBE0(0) == 0 && func_001FBBE0(1) != 0) {
            D_005CF2F0.unk0C = 1;
        }
    }

    for (i = 0; i < D_00446A60; i++) {
        kind = D_00446BF4[i].unk3E;
        func_001E5210(D_00446BF4[i].unk42);
        switch (kind) {
        case 2:
            func_00207478((int)&D_00446BF4[i], kind + 1000, 1, D_00425510[i]);
            break;
        case 29:
            if ((u8)D_00446BF4[i].slot < 2) {
                func_00207478((int)&D_00446BF4[i], kind + 1000, D_00446BF4[i].slot, D_00425510[i]);
            }
            break;
        case 3:
            switch (D_005CF2F0.unk12) {
            case 4:
            case 5:
            case 9:
            case 10:
                func_00207478((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
            break;
        case 31:
            if (D_005CF2F0.unk1B != 0) {
                if (D_00446C70 != 1) {
                    func_00207478((int)&D_00446BF4[i], kind + 1000, D_005CF2F0.unk0C, D_00425510[i]);
                } else {
                    func_00207478((int)&D_00446BF4[i], kind, D_005CF2F0.unk0C, D_00425510[i]);
                }
            }
            break;
        case 26:
            if (func_001FBBE0(0) == 0) {
                if (D_00446C70 == 0) {
                    func_00207478((int)&D_00446BF4[i], kind + 1000, 0x20, D_00425510[i]);
                } else if (D_00446C70 < 2) {
                    if (D_00446C94 != 1 || D_00446BDC == 0) {
                        func_00203F28((int)&D_00446BF4[i], kind, 0x20);
                    }
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
                    if (D_00446C94 != 1 || D_00446BDC == 0) {
                        func_00203F28((int)&D_00446BF4[i], kind, 0x20);
                    }
                }
            } else if (D_00446C70 != 1) {
                func_00207478((int)&D_00446BF4[i], kind + 1000, 0x80, D_00425510[i]);
            } else {
                func_00203F28((int)&D_00446BF4[i], kind, 0x80);
            }
            break;
        case 28:
            switch (D_005CF2F0.unk12) {
            case 4:
                func_00203F28((int)&D_00446BF4[i], kind, 4);
                break;
            case 5:
                func_00203F28((int)&D_00446BF4[i], kind, 5);
                break;
            case 9:
                func_00203F28((int)&D_00446BF4[i], kind, 9);
                break;
            case 10:
                func_00203F28((int)&D_00446BF4[i], kind, 10);
                break;
            }
            /* fallthrough */
        default:
            func_00207478((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    if (D_005CF2F0.unk14 != 0) {
        if (func_001FB6F8() == 0) {
            D_00447224 = func_001FBA78();
            if (D_00447224 != 0) {
                D_005CF2F0.unk18 = 1;
                D_00446C70 = 2;
                D_005CF2F0.unk10 = 1;
            } else {
                D_005CF2F0.unk12 = 9;
            }
            D_005CF2F0.unk14 = 0;
        } else if (card_result < 0) {
            if (func_001FBBE0(D_005CF2F0.unk0C) != 0) {
                D_005CF2F0.unk12 = 9;
            } else {
                D_005CF2F0.unk12 = 10;
            }
            D_005CF2F0.unk14 = 0;
            card_result = 0;
            func_001F9740(1);
        }
    }

    if (D_005CF2F0.unk11 != 0) {
        if (card_result < 0) {
            D_005CF2F0.unk18 = 1;
            D_00446C70 = 3;
        }
    } else if (card_result < 0) {
        D_005CF2F0.unk14 = 0;
        D_005CF2F0.unk12 = 10;
        func_001F9740(1);
    } else if (D_005CF2F0.unk14 == 0 && D_005CF2F0.unk18 == 0) {
        if ((D_00446DE8 & 0xA0) && D_004472D0 != 0xFF && D_005CF2F0.unk12 == -1) {
            func_0019CDC0(1);
            D_005CF2F0.unk0C = (D_005CF2F0.unk0C + 1) & 1;
        } else {
            report = func_00217760(0, 0);
            if ((report->pressed & 0x4000) && D_004472D0 != 0xFF) {
                if (D_005CF2F0.unk12 != -1) {
                    func_0019CDC0(0);
                    D_005CF2F0.unk12 = -1;
                } else if (func_001FBBE0(D_005CF2F0.unk0C) != 0) {
                    if (func_001FB9B0() > 0) {
                        if (func_001F9BA0(D_005CF2F0.unk0C, 1) == 0) {
                            func_0019CDC0(0);
                            D_005CF2F0.unk14 = 1;
                        }
                    } else {
                        D_005CF2F0.unk12 = 9;
                    }
                } else if (D_005CF2F0.unk0C == 0) {
                    func_0019CDC0(0);
                    D_005CF2F0.unk12 = 4;
                } else if (D_005CF2F0.unk0C == 1) {
                    func_0019CDC0(0);
                    D_005CF2F0.unk12 = 5;
                }
            } else {
                report = func_00217760(0, 0);
                if (((report->pressed & 0x2000) && D_004472D0 != 0xFF) || D_0038CC50.bit9) {
                    func_0019CDC0(2);
                    if (D_005CF2F0.unk12 != -1) {
                        D_005CF2F0.unk12 = -1;
                    } else {
                        D_005CF2F0.unk11 = 1;
                        func_001FAFB0();
                    }
                }
            }
        }
    }

    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446A60; i++) {
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
                if (D_005CF2F0.unk1B != 0) {
                    D_005CF2F0.unk18 = 0;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446A60; i++) {
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
                if (D_00446C94 == 1) {
                    D_00446BDC++;
                } else {
                    D_00446BDC = 2;
                }
                if (D_00446BDC == 2) {
                    D_00446C04 = 128.0f;
                    D_00446BDC = 0;
                    D_00446C08 = 0;
                    if (D_00446C70 == 2) {
                        D_005CF2F0.unk13 = -1;
                        D_00446B20 = (void *)func_001E0D98;
                        D_00446B24 = (void *)func_001E02F8;
                        D_005CF2F0.unk0D = 0;
                        D_005CF2F0.unk1D = 0;
                        func_001FBB68(0);
                        func_001FBB68(1);
                    } else if (D_00446C70 == 3) {
                        if (D_00446C94 != 1) {
                            D_00446B20 = (void *)func_001DF308;
                        } else {
                            D_00446AC8++;
                        }
                    }
                    D_005CF2F0.unk12 = -1;
                    D_00446C70 = 0;
                }
                break;
            }
        }
        break;
    }
}
