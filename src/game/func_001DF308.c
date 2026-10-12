#include "kf_game.h"
#include "kf_menu.h"
#include "kf_packet.h"

extern s8 D_00446C70;
extern u16 D_00446DE8;
extern char D_004472DA;
extern s32 D_004472E8;
extern s32 D_00446BD8;
extern u32 D_00446A5C;
extern SpritePacket *D_00447258;
extern f32 D_00425510[0x100];
extern GameStatus D_0038CC50;
extern MenuScreenState D_005CF2F0;
extern u16 D_005CF318[13];
extern void *D_0043FC58[];
extern void *D_00446B20;
extern int D_004472D0;
extern float D_00446C04;
extern char D_00446C08;
extern s8 D_00446C09;
extern u8 D_004472CF;
extern u8 D_0034968A[];
extern int D_00446AC8;
extern u16 D_0044692C;
extern u16 D_0044692E;
extern u16 D_00446930;
extern u16 D_00446932;
extern u16 D_00446934;
extern u16 D_00446936;
extern u16 D_00446938;
extern u16 D_0044693A;
extern u16 D_0044693C;
extern u16 D_0044693E;
extern u16 D_00446940;
extern u16 D_00446942;
extern u16 D_00446944;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001D7C30(void);
f32 func_001D8300(f32, s32, s32);
void func_001E2558(void);
int func_001E5210(u8);

// match: both are called as void functions, so the result register is free at once
#define PLAY_MENU_SOUND(sound) ((void (*)(s32))func_0019CDC0)(sound)
#define SET_PART_MODE(mode) ((void (*)(u8))func_001E5210)(mode)

s32 func_001F94C0(void);
s32 func_001F9740(s32);
void func_001FC5E8(MenuCursorBar *);
void func_00207478(int, int, int, int);
void *func_00217760(int, int);

/* The title menu screen: moves the cursor over its five lines, asks yes or no on the last, and fades the parts in and out. */
void func_001DF308(MenuScreenContext *context) {
    u32 i;
    s32 line_count = 5;
    u8 kind;
    MenuCursorBar bar;
    PadReport *report;

    if (D_00446C70 != 4) {
        if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_004472E8--;
        } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_004472E8++;
        }
    } else {
        if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_00446BD8--;
        } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_00446BD8++;
        }
        if (D_00446BD8 < 0) {
            D_00446BD8 = 1;
        } else if (D_00446BD8 >= 2) {
            D_00446BD8 = 0;
        }
    }
    D_004472E8 = (D_004472E8 + 5) % line_count;

    for (i = 0; i < D_00446A5C; i++) {
        kind = D_00447258[i].unk3E;
        SET_PART_MODE(D_00447258[i].unk42);
        if (D_00446C70 == 4 && kind == 2) {
            func_00207478((int)&D_00447258[i], 0x3EA, 0, D_00425510[i]);
        } else if (kind != 2) {
            if (D_00446C70 != 1) {
                func_00207478((int)&D_00447258[i], kind + 1000, D_004472E8, D_00425510[i]);
            } else {
                func_00207478((int)&D_00447258[i], kind, D_004472E8, D_00425510[i]);
            }
        }
        SET_PART_MODE(0);
    }

    if (D_00446C70 == 4) {
        bar.first.f.lo16 = 0x7710;
        bar.first.f.mid16 = D_00446BD8 * 0xC0 + 0x8010;
        bar.first.f.mid24 = 0x86470;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1250;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xC0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFD] = func_001D8300(D_00425510[0xFD], 0x10, 0x20);
        bar.alpha = D_00425510[0xFD];
        func_001FC5E8(&bar); // match: the float at bar+0x24 that func_001FC5E8 copies is never set
    }

    report = func_00217760(0, 0);
    if ((report->pressed & 0x4000) && D_004472D0 != 0xFF && D_004472DA != 0) {
        if (D_00446C70 != 4) {
            PLAY_MENU_SOUND(0);
            switch (D_004472E8) {
            case 0:
                D_005CF2F0.unk16 = 0;
                D_005CF2F0.unk10 = 0;
                D_005CF2F0.unk11 = 0;
                D_005CF2F0.unk14 = 0;
                D_005CF2F0.unk0C = 0;
                D_005CF2F0.unk0D = 0;
                D_005CF2F0.unk18 = 1;
                D_005CF2F0.unk1B = 0;
                D_005CF2F0.unk1E = 0;
                D_005CF2F0.unk1F = 0;
                D_005CF2F0.unk20 = 0;
                D_005CF2F0.unk00 = 0;
                D_005CF2F0.unk0E = 1;
                D_005CF2F0.unk12 = -1;
                func_001F94C0();
                func_001F9740(1);
                D_00446C70 = 2;
                D_004472DA = 0;
                for (i = 0; i < D_00446A5C; i++) {
                    if (D_00447258[i].unk3E == 1 && D_004472E8 == D_00447258[i].slot) {
                        D_00425510[i] = D_00446C04;
                    }
                }
                break;
            case 1:
            case 2:
            case 3:
                for (i = 0; i < D_00446A5C; i++) {
                    if (D_00447258[i].unk3E == 1 && D_004472E8 == D_00447258[i].slot) {
                        D_00425510[i] = D_00446C04;
                    }
                }
                D_004472DA = 0;
                D_00446C70 = 2;
                break;
            case 4:
                for (i = 0; i < D_00446A5C; i++) {
                    if (D_00447258[i].unk3E == 1 && D_004472E8 == D_00447258[i].slot) {
                        D_00425510[i] = D_00446C04;
                    }
                }
                D_00446C70 = 4;
                break;
            }
        } else if (D_00446BD8 == 0) {
            PLAY_MENU_SOUND(0);
            D_004472DA = 0;
            D_00446C70 = 5;
        } else {
            PLAY_MENU_SOUND(2);
            D_00446C70 = 1;
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        PLAY_MENU_SOUND(2);
        if (D_00446C70 != 4) {
            D_004472DA = 0;
            D_00446C70 = 3;
        } else {
            D_00446C70 = 1;
            D_00446BD8 = 1;
        }
    }

    if (D_0038CC50.bit9) {
        context->unk04 = 0;
        D_004472E8 = 0;
        D_00446B20 = (void *)func_001D7C30;
    }

    switch (D_00446C70) {
    case 0:
        D_00425510[0xFD] += 1.6f;
        if (D_00425510[0xFD] > 16.0f) {
            D_00425510[0xFD] = 16.0f;
        }
        for (i = 0; i < D_00446A5C; i++) {
            D_00425510[i] += SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A5C; i++) {
                    D_00425510[i] = SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha;
                }
                D_00446C70 = 1; // match: before D_004472DA, or every register of the step stores moves
                D_004472DA = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 5:
    case 6:
        D_00425510[0xFD] -= 1.6f;
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A5C; i++) {
            D_00425510[i] -= SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                D_00425510[i] = 0.0f;
                if (D_00447258[i].unk3E != 1 || D_004472E8 != D_00447258[i].slot) {
                    memset(D_00425510, 0, 0x400);
                    if (D_00446C70 == 2) {
                        D_004472CF = D_0034968A[0];
                        D_00446C04 = 128.0f;
                        D_00446B20 = D_0043FC58[D_004472E8];
                        D_00446C09 = 0;
                        D_00446C08 = 0;
                        if (D_004472E8 == 2) {
                            D_005CF318[0] = D_0044692C;
                            D_005CF318[1] = D_0044692E;
                            D_005CF318[2] = D_00446932;
                            D_005CF318[3] = D_00446930;
                            D_005CF318[4] = D_00446936;
                            D_005CF318[5] = D_00446934;
                            D_005CF318[6] = D_0044693C;
                            D_005CF318[7] = D_0044693E;
                            D_005CF318[8] = D_00446940;
                            D_005CF318[9] = D_00446944;
                            D_005CF318[10] = D_00446938;
                            D_005CF318[11] = D_0044693A;
                            D_005CF318[12] = D_00446942;
                            func_001E2558();
                        }
                    } else if (D_00446C70 == 3) {
                        context->unk04 = 0;
                        D_004472E8 = 0;
                        D_00446B20 = (void *)func_001D7C30;
                    } else if (D_00446C70 == 5) {
                        D_00446AC8++;
                        context->unk04 = 0;
                        D_004472E8 = 0;
                        D_0038CC50.bit9 = 1;
                    }
                    D_00446C70 = 0;
                    D_00446BD8 = 1;
                    break;
                }
            }
        }
        break;
    }
}
