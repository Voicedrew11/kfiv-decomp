#include "kf_game.h"
#include "kf_menu.h"
#include "kf_packet.h"

extern f32 D_00425510[0x100];
extern u32 D_00446ABC;
extern int D_00446AC8;
extern SpritePacket *D_00446C68;
extern s8 D_00446C70;
extern u16 D_00446DE8;
extern int D_004472D0;
extern MenuScreenState D_005CF2F0;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
f32 func_001D8300(f32, s32, s32);
int func_001E5210(u8);
s32 func_001F9648(void);
s32 func_001F9740(s32);
s32 func_001F9BA0(s32, s32);
void func_001FA988(void);
void func_001FAFB0(void);
s32 func_001FB6F8(void);
s32 func_001FBBA8(void);
char func_001FBBE0(int);
s32 func_001FBC00(s32);
int func_001FBC80(int);
s32 func_001FBCF0(void);
void func_001FBDC8(void);
void func_001FC5E8(MenuCursorBar *);
void func_00207478(int, int, int, int);
void *func_00217760(int, int);

/* The menu screen that first checks the two slots, then asks yes or no with a cursor bar, and fades its parts in and out. */
void func_001E3858(void) {
    u32 i;
    u8 kind;
    MenuCursorBar bar;
    PadReport *report;
    u32 full_alpha;
    s32 result = ((s32 (*)(void))func_001F9648)(); // match: its result is used here

    switch (D_005CF2F0.unk17) {
    case 0:
        if (func_001FBBA8() == 0) {
            return;
        }
        if (func_001FBBE0(0) != 0 && func_001FBC00(0) != 0) {
            func_001F9BA0(0, 0);
            D_005CF2F0.unk17 = 1;
        } else if (func_001FBBE0(1) != 0 && func_001FBC00(1) != 0) {
            D_005CF2F0.unk17 = 2;
            func_001F9BA0(1, 0);
        } else {
            D_005CF2F0.unk17 = 3;
            func_001FA988();
        }
        return;
    case 1:
        if (func_001FB6F8() == 0) {
            D_005CF2F0.unk19 = func_001FBCF0();
            if (func_001FBBE0(1) != 0 && func_001FBC00(1) != 0) {
                D_005CF2F0.unk17 = 2;
                func_001F9BA0(1, 0);
            } else {
                D_005CF2F0.unk17 = 3;
                func_001FA988();
            }
        } else if (result < 0) {
            D_005CF2F0.unk19 = 0;
            if (func_001FBBE0(1) != 0 && func_001FBC00(1) != 0) {
                D_005CF2F0.unk17 = 2;
                func_001F9BA0(1, 0);
            } else {
                D_005CF2F0.unk17 = 3;
                func_001FA988();
            }
        }
        return;
    case 2:
        if (func_001FB6F8() == 0) {
            D_005CF2F0.unk1A = func_001FBCF0();
            D_005CF2F0.unk17 = 3;
            func_001FA988();
        } else if (result < 0) {
            D_005CF2F0.unk1A = 0;
            D_005CF2F0.unk17 = 3;
            func_001FA988();
        }
        return;
    case 3:
        if (result > 0) {
            D_005CF2F0.unk17 = 4;
            for (i = 0; (s32)i < 2; i++) {
                switch (func_001FBC80(i)) {
                case 0:
                    D_005CF2F0.unk04 |= 1 << (i * 4);
                    break;
                case 8:
                    D_005CF2F0.unk04 |= 2 << (i * 4);
                    break;
                default:
                    D_005CF2F0.unk04 |= 4 << (i * 4);
                    break;
                }
            }
            if (D_005CF2F0.unk04 == 0x44) {
                D_005CF2F0.unk12 = 0x10;
            } else if (D_005CF2F0.unk04 == 0x22) {
                D_005CF2F0.unk12 = 0x13;
            } else if (D_005CF2F0.unk04 == 0x42) {
                D_005CF2F0.unk12 = 0x11;
            } else if (D_005CF2F0.unk04 == 0x24) {
                D_005CF2F0.unk12 = 0x12;
            } else {
                D_005CF2F0.unk1C = D_005CF2F0.unk11 = 1;
                func_001FAFB0();
            }
        }
        return;
    }

    if (D_005CF2F0.unk1C == 0) {
        for (i = 0; i < D_00446ABC; i++) {
            kind = D_00446C68[i].unk3E;
            ((void (*)(u8))func_001E5210)(D_00446C68[i].unk42); // match: called as returning nothing, which frees $2 for the switch
            switch (D_005CF2F0.unk12) {
            case 0x10:
                func_00207478((int)&D_00446C68[i], kind + 1000, 0x10, D_00425510[i]);
                break;
            case 0x11:
                func_00207478((int)&D_00446C68[i], kind + 1000, 0x11, D_00425510[i]);
                break;
            case 0x12:
                func_00207478((int)&D_00446C68[i], kind + 1000, 0x12, D_00425510[i]);
                break;
            case 0x13:
                func_00207478((int)&D_00446C68[i], kind + 1000, 0x13, D_00425510[i]);
                break;
            }
            func_001E5210(0);
        }
    }

    if (D_005CF2F0.unk1C == 0) {
        bar.first.f.lo16 = 0x75D0;
        bar.first.f.mid16 = D_005CF2F0.unk0C * 0xE0 + 0x8250;
        bar.first.f.mid24 = 0xDBBA0;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x14A0;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        if (D_00446C70 == 1) {
            D_00425510[0xFD] = func_001D8300(D_00425510[0xFD], 0x10, 0x20);
        } else {
            D_00425510[0xFD] = 0.0f;
        }
        bar.alpha = D_00425510[0xFD];
        func_001FC5E8(&bar);
    }

    if (D_005CF2F0.unk11 != 0) {
        if (result < 0) {
            if (D_00446C70 == 2) {
                D_00446C70 = 4;
            } else if (D_00446C70 == 3) {
                D_00446C70 = 5;
            } else if (D_00446C70 == 0) {
                func_001FBDC8();
                D_00446AC8++;
            }
        }
    } else if (result < 0) {
        D_005CF2F0.unk11 = 1;
        func_001F9740(1);
        func_001FAFB0();
    } else {
        report = func_00217760(0, 0);
        if ((report->pressed & 0x4000) && D_004472D0 != 0xFF) {
            func_0019CDC0(0);
            D_005CF2F0.unk11 = 1;
            func_001F9740(1);
            func_001FAFB0();
            if (D_005CF2F0.unk0C == 0) {
                D_00446C70 = 2;
            } else {
                D_00446C70 = 3;
            }
        } else if ((D_00446DE8 & 0x50) && D_004472D0 != 0xFF) {
            func_0019CDC0(1);
            D_005CF2F0.unk0C = (D_005CF2F0.unk0C + 1) & 1;
        }
    }

    if (D_005CF2F0.unk1C != 0) {
        return;
    }
    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446ABC; i++) {
            D_00425510[i] += (f32)D_00446C68[i].alpha / 10.0f;
            if ((f32)D_00446C68[i].alpha < D_00425510[i]) {
                for (i = 0; i < D_00446ABC; i++) {
                    full_alpha = D_00446C68[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00446C70 = 1;
                break;
            }
        }
        break;
    case 4:
    case 5:
        for (i = 0; i < D_00446ABC; i++) {
            D_00425510[i] -= (f32)D_00446C68[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_005CF2F0.unk1B++;
                if (D_005CF2F0.unk1B == 1) {
                    break;
                }
                if (D_00446C70 == 4) {
                    func_001FBDC8();
                    D_00446AC8++;
                } else {
                    D_00446AC8--;
                }
                D_00446C70 = 0;
                break;
            }
        }
        break;
    }
}
