#include "kf_game.h"
#include "kf_link.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_state.h"

extern u8 D_00369830[380];          /* how many of each item the player has */
extern GameStatus D_0038CC50;
extern GameDefinition D_004147F8[380];
extern f32 D_00421000[4];
extern f32 D_00425510[0x100];       /* one alpha per menu part; [253] the cursor bar's, [255] the text's */
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern f32 D_0042B7D0[16];
extern u32 D_00446A34;              /* how many parts the item menu has */
extern s32 D_00446AC8;
extern void *D_00446B20;
extern f32 D_00446B3C;              /* the shown item model's turn about y */
extern s32 D_00446B40;
extern MenuColor D_00446C78;
extern LinkNode *D_00446C90;
extern u16 D_00446DE8;
extern s32 D_00446DF4;
extern s32 D_00446F38;
extern SpritePacket *D_00447230;
extern s8 D_004472C5;               /* 0: fading in, 1: open, 2 to 4: waiting for the models, 5 to 7: fading out */
extern s8 D_004472DA;

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
s32 func_0019CDC0(s32);
s32 func_001CFC58(s32);
void func_001D45D0(void);
void func_001D50C0(void);
int func_001D6E60(f32 *);
void func_001D7C30(void);
f32 func_001D8300(f32, s32, s32);
void func_001DC2A8(void);
s32 func_001DFC60(HandlerContext *, s32);
int func_001E5210(u8);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
void func_001FDD28(TradeValue *, s32);
void func_00200068(s32, s32);
void func_00207478(int, int, int, int);

/* an angle brought back into -pi..pi (for angles above -pi) */
static inline f32 wrap_angle(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* The item menu: draws its parts, the turning item model, the list of seven and the cursor bar, and fades it in and out. */
void func_001D8398(HandlerContext *context) {
    MenuCursorBar bar;
    f32 view[16];
    f32 local[16];
    GsPackedWord word;
    GsPackedWord corner;
    MenuItem *item;
    MenuItem *bar_item;
    u32 i;
    s32 row;
    s32 table;
    s32 entry;
    s32 slot;
    s32 text;
    u8 kind;
    u32 full_alpha;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 0.8f;
    view[15] = 1.0f;
    word.raw = 0x0A0AAE607C307080;
    corner.raw = 0x0A0F42407C306FE0;
    item = D_00426120[0]; // match: dead (the magic menu, func_001D8C90, takes its first row here); it keeps the address's high part out of a register
    if (D_00426110[0] != NULL) {
        func_001DFC60(context, D_004472DA == 0);
    }
    if (D_00426110[0] != NULL) {
        D_00446DF4 = D_00369830[D_00426110[0]->unk0C];
    } else {
        D_00446DF4 = 0;
    }
    for (i = 0; i < D_00446A34; i++) {
        kind = D_00447230[i].unk3E;
        ((void (*)(u8))func_001E5210)(D_00447230[i].unk42); // match: called as if it returned nothing
        if (D_00426110[0] != NULL) {
            func_00207478((int)&D_00447230[i], kind + 1000, 0, (int)D_00425510[i]);
        } else {
            func_00207478((int)&D_00447230[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        ((void (*)(u8))func_001E5210)(0); // match: called as if it returned nothing
    }
    D_00446B40++;
    if (D_00446B40 >= 0x80) {
        D_00446B40 = 0;
    }
    if (D_00426110[0] != NULL) {
        bar_item = D_00426120[0];
        for (row = 0; row < 7; row++, bar_item = bar_item->prev) {
            if (bar_item == D_00426110[0]) {
                bar.first.f.lo16 = corner.f.lo16;
                bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
                bar.first.f.mid24 = corner.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
                bar.second.f.mid24 = bar.first.f.mid24;
                if (D_004472C5 == 1) {
                    D_00425510[253] = func_001D8300(D_00425510[253], 0x10, 0x20);
                }
                bar.alpha = (s32)D_00425510[253];
                func_001FC5E8(&bar);
                break;
            }
        }
    }
    if (D_00426110[0] != NULL) {
        sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        D_00446B3C = wrap_angle(D_00446B3C);
        sceVu0RotMatrixY(local, view, D_00446B3C);
        sceVu0TransMatrix(local, local, D_00421000);
        func_001D6E60(local);
        item = D_00426120[0];
        ((s32 (*)(MenuColor))func_001FDCD0)(D_00446C78); // match: called as if it returned a number
        for (row = 0; row < 7; row++, item = item->prev) {
            text = (s32)&D_004147F8[item->unk0C];
            func_001FDB88(word);
            ((s32 (*)(s32, s32))func_001FDD28)(text, 0x1A); // match: called as if it took and returned a number
            word.f.mid16 += 0xB0;
            word.f.lo16 = 0x7080;
            if (D_00426130[0]->prev == item) {
                break;
            }
        }
        item = D_00426110[0];
        table = func_001CFC58(D_00446F38);
        slot = item->unk0C + 0x1F5;
        entry = ((s32 *)table)[slot];
        word.raw = 0x000A875083207080;
        func_001FDB88(word);
        if (entry != 0) {
            text = table + entry;
            func_001FDCD0(D_00446C78);
            func_00200068((u16 *)text, 0x33);
        }
    }
    if ((D_00446DE8 & 0x4000) && D_004472DA != 0 && D_00426110[0] != NULL) {
        if (D_00426110[0]->unk0C < 0x91 || D_00426110[0]->unk0C > 0x93) {
            func_0019CDC0(0);
            D_0038CC50.bit6 = TRUE;
            D_0038CC50.unk20 = D_00426110[0]->unk0C;
            D_004472C5 = 2;
            D_004472DA = 0;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else {
            func_0019CDC0(2);
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472C5 = 3;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    } else if ((D_00446DE8 & 0x8000) && D_004472DA != 0) {
        func_0019CDC0(0);
        D_004472C5 = 4;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472C5 == 2 && D_00446C90 == NULL) {
        D_004472C5 = 5;
    }
    if (D_004472C5 == 3 && D_00446C90 == NULL) {
        D_004472C5 = 6;
    }
    if (D_004472C5 == 4 && D_00446C90 == NULL) {
        D_004472C5 = 7;
    }
    switch (D_004472C5) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00425510[253] += 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446A34; i++) {
            D_00425510[i] += D_00447230[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447230[i].alpha) {
                for (i = 0; i < D_00446A34; i++) {
                    full_alpha = D_00447230[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472C5 = 1;
                D_004472DA = D_004472C5;
                return;
            }
        }
        break;
    case 5:
    case 6:
    case 7:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A34; i++) {
            D_00425510[i] -= D_00447230[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                switch (D_004472C5) {
                case 5:
                    D_00446B20 = NULL;
                    D_00446AC8++;
                    break;
                case 6:
                    ((s32 (*)(s32))func_001D45D0)(0); // match: called as if it took a number and returned one
                    D_00446B20 = func_001D7C30;
                    break;
                case 7:
                    ((s32 (*)(s32))func_001D45D0)(0); // match: called as if it took a number and returned one
                    ((void (*)(s32))func_001D50C0)(0); // match: the callee takes nothing
                    D_00446B20 = func_001DC2A8;
                    break;
                }
                D_004472C5 = 0;
                return;
            }
        }
        break;
    }
}
