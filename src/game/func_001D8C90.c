#include "kf_effect.h"
#include "kf_game.h"
#include "kf_link.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include <eeregs.h>

extern GameStatus D_0038CC50;
extern FxTableEntry D_00402DA8[];
extern PlayerState D_00413F80;
extern f32 D_00421010[4];
extern f32 D_00425510[0x100];       /* one alpha per menu part; [253] the cursor bar's, [255] the text's */
extern LinkNode D_00426470;
extern f32 D_0042B7D0[16];
extern u32 D_00446A38;              /* how many parts the magic menu has */
extern s32 D_00446AC8;
extern void *D_00446B20;
extern MenuColor D_00446C78;
extern LinkNode *D_00446C90;
extern u16 D_00446DE8;
extern s32 D_00446DF4;
extern s32 D_00446DF8;
extern s32 D_00446F38;
extern SpritePacket *D_00447234;
extern s8 D_004472C8;               /* 0: fading in, 1: open, 2: waiting for the models, 3: fading out */
extern s8 D_004472DA;
extern s32 D_005CE830[];
extern MenuItem *D_005CE880[];
extern MenuItem *D_005CE890[];
extern MenuItem *D_005CE8A0[];

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
s32 func_0019CDC0(s32);
s32 func_001CFC58(s32);
void func_001D45D0(void);
void func_001D7C30(void);
f32 func_001D8300(f32, s32, s32);
int func_001DFEF8(void);
int func_001E5210(u8);
void func_001FC5E8(MenuCursorBar *);
int func_001FDC28(GsPackedWord);
void func_001FDCD0(MenuColor);
s32 func_001FE9A8(FxTableEntry *, s32);
void func_00200068(s32, s32);
void func_00201E10(LinkNode *, f32 *, MenuColor); // match: the colour is loaded unaligned, so not the definition's types
void func_00207478(int, int, int, int);

/* The magic menu: draws its parts, the list of seven and the cursor bar, and fades it in and out. */
void func_001D8C90(s32 context) {
    f32 view[16];
    f32 local[16];
    MenuCursorBar bar;
    GsPackedWord word;
    GsPackedWord corner;
    GsPackedWord pitch;
    MenuItem *item;
    MenuItem *selected;
    u32 *packet;
    u32 i;
    s32 row;
    s32 page;
    s32 number;
    s32 table;
    s32 entry;
    s32 slot;
    u8 kind;
    u32 full_alpha;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 0.8f;
    view[15] = 1.0f;
    if (D_005CE830[0] == 0) {
        D_00446B20 = func_001D7C30;
        return;
    }
    if (D_004472C8 < 2) {
        ((void (*)(s32))func_001DFEF8)(context); // match: the callee takes nothing
    }
    item = D_005CE890[0];
    word.raw = 0x0C0AAE607C307080;
    func_001FDB88(word);
    corner.raw = 0x0A0F42407C307000;
    pitch.raw = 0xB00140;
    func_001FDC28(pitch);
    i = 0;
    number = D_005CE880[0]->unk0C; /* the selected magic's number */
    page = (number - 1) / 15;
    D_00446DF4 = D_00402DA8[number].unkFC;
    D_00446DF8 = (number - 1) % 3 + 1;
    for (; i < D_00446A38; i++) {
        kind = D_00447234[i].unk3E;
        func_001E5210(D_00447234[i].unk42);
        func_00207478((int)&D_00447234[i], kind + 1000, page, (int)D_00425510[i]);
        ((void (*)(u8))func_001E5210)(0); // match: called as if it returned nothing
    }
    for (row = 0; row < 7; row++, item = item->prev) {
        if (D_005CE880[0] == item) {
            bar.first.f.lo16 = corner.f.lo16;
            bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
            bar.first.f.mid24 = corner.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1220;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_004472C8 == 1) {
                D_00425510[253] = func_001D8300(D_00425510[253], 0x10, 0x20);
            }
            bar.alpha = (s32)D_00425510[253];
            func_001FC5E8(&bar);
            break;
        }
    }
    item = D_005CE890[0];
    func_001FDCD0(D_00446C78);
    for (row = 0; row < 7; row++, item = item->prev) {
        func_001FE9A8(&D_00402DA8[item->unk0C], 0x1A);
        word.f.mid16 += 0xB0; // match: dead, left over from the item menu (func_001D8398), which sets the row's place here
        word.f.lo16 = 0x7080; // match: dead as well; without the two lines the loop is compiled differently
        if (D_005CE8A0[0]->prev == item) {
            break;
        }
    }
    selected = D_005CE880[0];
    table = func_001CFC58(D_00446F38);
    slot = selected->unk0C + 0x385;
    entry = ((s32 *)table)[slot];
    word.raw = 0x000A875082E07000;
    func_001FDB88(word);
    if (entry != 0) {
        func_00200068(table + entry, 0x35);
    }
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    sceVu0TransMatrix(local, view, D_00421010);
    packet = func_001FBE70();
    packet[0] = 0x8002;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0x60F0;
    packet[5] = 0x7990;
    packet[6] = 0x19;
    packet[7] = 0;
    packet[8] = 0x44;
    packet[9] = 0;
    packet[10] = 0x43;
    packet[11] = 0;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 3;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    func_00201E10(&D_00426470, local, D_00446C78);
    if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        if (D_00413F80.unkB8 >= ((FxTableEntry *)D_005CE880[0]->unk00)->unkFC && D_00413F80.unkC0 >= 5000) {
            func_0019CDC0(0);
            D_0038CC50.unk28 = D_005CE880[0]->unk0C;
            D_004472C8 = 2;
            D_004472DA = 0;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else {
            func_0019CDC0(3);
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472C8 = 2;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472C8 == 2 && D_00446C90 == NULL) {
        D_004472C8 = 3;
    }
    switch (D_004472C8) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00425510[253] += 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] > 16.0f) {
            D_00425510[255] = 16.0f; // match: retail clamps the text alpha here, not the bar's ([253])
        }
        for (i = 0; i < D_00446A38; i++) {
            D_00425510[i] += D_00447234[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447234[i].alpha) {
                D_004472DA = D_004472C8 = 1;
                for (i = 0; i < D_00446A38; i++) {
                    full_alpha = D_00447234[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                return;
            }
        }
        break;
    case 3:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A38; i++) {
            D_00425510[i] -= D_00447234[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                D_004472C8 = 0;
                if (D_0038CC50.unk28 != 0) {
                    D_00446B20 = NULL;
                    D_00446AC8++;
                } else {
                    ((s32 (*)(s32))func_001D45D0)(0); // match: called as if it took a number and returned one
                    D_00446B20 = func_001D7C30;
                }
                return;
            }
        }
        break;
    }
}
