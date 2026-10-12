#include "kf_link.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_state.h"

extern u8 D_00369830[380];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern PlayerState D_00413F80;
extern u8 D_00413FBC[];
extern LevelDef D_004147F8[0x17C];
extern PlayerItemDef D_0041B9B8[];
extern LevelRecordDef D_0041E118[];
extern f32 D_004210A0[4];
extern f32 D_00425510[0x100];       /* one alpha per menu part; [200] on the second list's, [253] the cursor bar's, [255] the text's */
extern f32 D_00425910[0x100];       /* the same for the detail panel; [255] its text's */
extern f32 D_0042B7D0[16];
extern const TradeGroupBases D_0043FF10;
extern u32 D_00446A4C;
extern u32 D_00446A58;
extern u32 D_00446A84;
extern u32 D_00446A88;
extern u32 D_00446A8C;
extern u8 D_00446AD4;               /* nonzero while the detail panel is open */
extern void *D_00446B20;
extern f32 D_00446BD0;              /* the turning model's angle */
extern MenuColor D_00446C78;
extern MenuColor D_00446C80;
extern LinkNode *D_00446C90;
extern u16 D_00446DE8;
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
extern s32 D_00446F38;
extern MenuItem *D_00447210;
extern MenuItem *D_00447214;
extern MenuItem *D_00447218;
extern MenuItem *D_0044721C;
extern MenuItem *D_00447220;
extern SpritePacket *D_00447248;
extern SpritePacket *D_00447254;
extern SpritePacket *D_00447270;
extern SpritePacket *D_00447274;
extern SpritePacket *D_00447278;
extern s8 D_004472CB;               /* 0: fading in, 1: open, 2: waiting for the loads, 3: fading out */
extern s8 D_004472CC;               /* the detail panel's: 0: fading in, 1: open, 2: fading out */
extern s8 D_004472DA;               /* nonzero while the screen takes input */
extern MenuItem *D_005CE850[];
extern MenuItem *D_005CE860[];
extern MenuItem *D_005CE870[];

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
s32 func_0019CDC0(s32);
void func_001BA310(void);
s32 func_001CFC58(s32);
void func_001D2680(void);
void func_001D45D0(void);
int func_001D6E60(f32 *);
void func_001D7C30(void);
f32 func_001D8300(f32, s32, s32);
s32 func_001E0088(void);
int func_001E4FE0(s32, u32);
int func_001E5210(u8);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
int func_001FE9A8(LevelDef *, s32);
s32 func_00200068(u16 *, s32);
void func_00203F28(int, int, int);
void func_00207478(int, int, int, int);

static inline s32 ScaleValue(s32 value, s32 percent, s32 divisor) {
    return value * percent / divisor;
}

/* Starts every packet of a list at its full alpha. match: the alpha is read as an unsigned bit-field */
#define FILL_FADE(levels, packets, count)                               \
    for (i = 0; i < (count); i++) {                                     \
        (levels)[i] = SPRITE_PACKET_COLOR(&(packets)[i])->alpha;        \
    }

/* Draws a list of packets at their current fade levels. */
#define DRAW_PACKETS(packets, count, param)                                                             \
    for (i = 0; i < (count); i++) {                                                                     \
        func_001E5210((packets)[i].unk42);                                                              \
        func_00207478((int)&(packets)[i], (packets)[i].unk3E + 1000, (param), (int)D_00425910[i]);      \
        ((void (*)(u8))func_001E5210)(0);                                                               \
    }

/* The item screen: the list of seven under a cursor bar, the turning model, the chosen item's text and, when opened,
   the panel with its values; the confirm button puts the item on or takes it off. */
void func_001DDB10(HandlerContext *context) {
    f32 view[16];
    f32 local[16];
    TradeGroupBases bases;
    MenuCursorBar bar;
    s32 worn_mask;
    s32 worn_bit;
    SpritePacket *packets;
    u32 count;
    MenuItem *selected;
    MenuItem *item;
    MenuItem *node;
    MenuItem *cursor;
    LevelDef *def;
    GsPackedWord corner;
    GsPackedWord position;
    s32 record;
    s32 number;
    s32 row;
    s32 i;
    s32 kind;
    s32 texts;
    s32 text_index;
    s32 text_offset;
    s32 text;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 0.8f;
    view[15] = 1.0f;
    worn_mask = 0;
    worn_bit = 0;
    bases = D_0043FF10;
    if (D_004472CB < 2) {
        ((void (*)(HandlerContext *))func_001E0088)(context); // match: the callee takes nothing
    }
    selected = D_005CE850[0];
    item = D_005CE860[0];
    corner.raw = 0x0A09C4007C306FE0;
    if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        {
            // match: the value read first is used only to set the bit; the test and the clearing read the field again
            u8 state = D_004147F8[selected->unk0C].state;
            D_004147F8[selected->unk0C].state =
                (D_004147F8[selected->unk0C].state & 1) ? D_004147F8[selected->unk0C].state & ~1 : state | 1;
        }
        func_0019CDC0(0);
        number = selected->unk0C;
        if (number == D_00413F80.unk3C + 150) {
            D_00413F80.unk3C = 0;
        } else if (number == D_00413F80.equipment[0] + 220) {
            D_00413F80.equipment[0] = 0;
        } else if (number == D_00413F80.equipment[2] + 220) {
            D_00413F80.equipment[2] = 55;
        } else if (number == D_00413F80.equipment[1] + 220) {
            D_00413F80.equipment[1] = 30;
        } else if (number == D_00413F80.equipment[3] + 220) {
            D_00413F80.equipment[3] = 80;
        } else if (number == D_00413F80.equipment[4] + 220) {
            D_00413F80.equipment[4] = 105;
        } else if (number == D_00413F80.equipment[5] + 220) {
            D_00413F80.equipment[5] = 130;
        } else if (number == D_00413F80.equipment[6] + 220) {
            D_00413F80.equipment[6] = 140;
        } else if (number == D_00413F80.equipment[7] + 220) {
            D_00413F80.equipment[7] = 150;
        } else if (number == D_00413F80.unk46) {
            D_00413F80.unk48 = 0;
            D_00413F80.unk46 = 0;
        }
        func_001BA310();
        if (D_00413F80.unk58 < D_00413F80.unkB4) {
            D_00413F80.unkB4 = D_00413F80.unk58;
        }
        if (D_00413F80.unkB8 > D_00413F80.unk5C) {
            D_00413F80.unkB8 = D_00413F80.unk5C;
        }
    }
    if (D_004472CB == 2) {
        item = D_00447210;
    } else {
        D_00447214 = selected;
        D_00447210 = item;
    }

    record = D_005CE850[0]->unk0C - bases.first_item[D_005CE850[0]->group];
    D_00446DF4 = D_00369830[D_005CE850[0]->unk0C];
    if (D_005CE850[0]->group == 0) {
        D_00446E30 = D_003679A0[record].unk14 + 1;
    }
    for (i = 0; i < D_00446A58; i++) {
        kind = D_00447254[i].unk3E;
        func_001E5210(D_00447254[i].unk42);
        if (D_005CE850[0]->group != 12) {
            func_00207478((int)&D_00447254[i], kind + 1000, 0, (int)D_00425510[i]);
        } else {
            func_00207478((int)&D_00447254[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    for (i = 0; i < 7; i++, item = item->prev) {
        if (D_004147F8[item->unk0C].state & 1) {
            worn_mask |= 1 << worn_bit;
        }
        worn_bit++;
        if (item == D_00447214) {
            bar.first.f.lo16 = corner.f.lo16;
            bar.first.f.mid16 = corner.f.mid16 + i * 0xB0;
            bar.first.f.mid24 = corner.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_004472CB == 1) {
                D_00425510[253] = func_001D8300(D_00425510[253], 0x10, 0x20);
            }
            bar.alpha = (s32)D_00425510[253];
            func_001FC5E8(&bar);
        }
        if (D_005CE870[0]->prev == item) {
            break;
        }
    }

    node = D_005CE860[0];
    cursor = D_005CE850[0];
    position.raw = 0x0A09EB107C3071C0;
    if (D_004472CB == 2) {
        node = D_00447218;
    } else {
        D_0044721C = cursor;
        D_00447218 = node;
    }
    for (row = 0; row < 7; row++, node = node->prev) {
        def = &D_004147F8[node->unk0C];
        func_001FDB88(position);
        func_001FDCD0(D_00446C78);
        func_001FE9A8(def, 0x1A);
        position.f.mid16 += 0xB0;
        if (D_005CE870[0]->prev == node) {
            break;
        }
    }

    if (D_004472CB != 2) {
        node = D_005CE850[0];
        D_00447220 = node;
    } else {
        node = D_00447220;
    }
    texts = func_001CFC58(D_00446F38);
    position.raw = 0x000A875083207080;
    text_index = node->unk0C + 0x1F5;
    text_offset = ((s32 *)texts)[text_index];
    func_001FDB88(position);
    if (text_offset != 0) {
        text = texts + text_offset;
        func_001FDCD0(D_00446C78);
        func_00200068(text, 0x33);
    }

    for (i = 0; i < D_00446A4C; i++) {
        kind = D_00447248[i].unk3E;
        if (kind != 2) {
            ((void (*)(u8))func_001E5210)(D_00447248[i].unk42); // match: called as returning nothing
            if (D_004472CB != 1 && ((worn_mask >> D_00447248[i].slot) & 1)) {
                func_00207478((int)&D_00447248[i], kind + 1000, worn_mask, (int)D_00425510[i + 200]);
            } else {
                func_00203F28((int)&D_00447248[i], kind, worn_mask);
            }
            func_001E5210(0);
        }
    }

    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    D_00446BD0 += 3.1415927f;
    D_00446BD0 -= (s32)(D_00446BD0 / 6.2831855f) * 6.2831855f;
    D_00446BD0 -= 3.1415927f;
    sceVu0RotMatrixY(local, view, D_00446BD0);
    sceVu0TransMatrix(local, local, D_004210A0);
    func_001D6E60(local);

    if ((D_00446DE8 & 0x1000) && D_004472DA != 0 && func_001E4FE0(record, D_005CE850[0]->group)) {
        func_0019CDC0(1);
        if (D_00446AD4 == 0) {
            D_00446AD4 = 1;
        } else if (D_004472CC != 2) {
            D_004472CC = 2;
        } else {
            D_004472CC = 0;
        }
    }

    if (D_00446AD4 != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
        switch (D_005CE850[0]->group) {
        case 11:
        case 12:
            packets = D_00447274;
            count = D_00446A88;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[0] + D_0041E118[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[1] + D_0041E118[record].unk00[1], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(D_003682F0[record].unk00[2] + D_0041E118[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(D_0041E118[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(D_0041E118[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(D_0041E118[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(D_0041E118[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(D_0041E118[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E18 = ScaleValue(D_0041E118[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E24 = D_003682F0[record].level;
            D_00446E28 = 100;
            D_00446E1C = D_004147F8[D_005CE850[0]->unk0C].unk3E;
            if (D_004472CC == 1) {
                FILL_FADE(D_00425910, D_00447274, D_00446A88)
            }
            DRAW_PACKETS(D_00447274, D_00446A88, D_00413FBC[D_005CE850[0]->group])
            packets = D_00447274;
            count = D_00446A88;
            break;
        case 7:
        case 8:
        case 9:
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[0] + D_0041E118[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[1] + D_0041E118[record].unk00[1], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(D_003682F0[record].unk00[2] + D_0041E118[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(D_0041E118[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(D_0041E118[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(D_0041E118[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(D_0041E118[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(D_0041E118[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E18 = ScaleValue(D_0041E118[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E1C = D_004147F8[D_005CE850[0]->unk0C].unk3E;
            if (D_004472CC == 1) {
                FILL_FADE(D_00425910, D_00447278, D_00446A8C)
            }
            DRAW_PACKETS(D_00447278, D_00446A8C, D_00413FBC[D_005CE850[0]->group])
            packets = D_00447278;
            count = D_00446A8C;
            break;
        case 0:
            D_00446DF8 = ScaleValue(D_003679A0[record].unk00[0] + D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[0], D_003679A0[record].level, 100);
            D_00446DFC = ScaleValue(D_003679A0[record].unk00[1] + D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[1], D_003679A0[record].level, 100);
            D_00446E00 = ScaleValue(D_003679A0[record].unk00[2] + D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[2], D_003679A0[record].level, 100);
            D_00446E04 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[3], D_003679A0[record].level, 100);
            D_00446E08 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[4], D_003679A0[record].level, 100);
            D_00446E0C = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[5], D_003679A0[record].level, 100);
            D_00446E10 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[6], D_003679A0[record].level, 100);
            D_00446E14 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[7], D_003679A0[record].level, 100);
            D_00446E18 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[8], D_003679A0[record].level, 100);
            D_00446E20 = (s32)(D_0041B9B8[record].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[record].level;
            D_00446E1C = D_004147F8[D_005CE850[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_005CE850[0]->group];
            D_00446E2C = 100;
            if (D_004472CC == 1) {
                FILL_FADE(D_00425910, D_00447270, D_00446A84)
            }
            DRAW_PACKETS(D_00447270, D_00446A84, 0)
            packets = D_00447270;
            count = D_00446A84;
            break;
        }

        switch (D_004472CC) {
        case 0:
            D_00425910[255] += 12.8f;
            if (D_00425910[255] > 128.0f) {
                D_00425910[255] = 128.0f;
            }
            D_00446C80.alpha = (s32)D_00425910[255];
            for (i = 0; i < count; i++) {
                D_00425910[i] += (f32)packets[i].alpha / 10.0f;
                if ((f32)packets[i].alpha < D_00425910[i]) {
                    FILL_FADE(D_00425910, packets, count)
                    D_004472CC = 1;
                    break;
                }
            }
            break;
        case 2:
            D_00425910[255] -= 12.8f;
            if (D_00425910[255] < 0.0f) {
                D_00425910[255] = 0.0f;
            }
            D_00446C80.alpha = (s32)D_00425910[255];
            for (i = 0; i < count; i++) {
                D_00425910[i] -= (f32)packets[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400); // match: the panel's state is left at 2 and D_00446AD4 set
                    break;
                }
            }
            break;
        }
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472CB = 2;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472CB == 2 && D_00446C90 == NULL) {
        D_004472CB = 3;
    }

    switch (D_004472CB) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[253] += 1.6f;
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446A4C; i++) {
            D_00425510[i + 200] += (f32)D_00447248[i].alpha / 10.0f;
        }
        for (i = 0; i < D_00446A58; i++) {
            D_00425510[i] += (f32)D_00447254[i].alpha / 10.0f;
            if ((f32)D_00447254[i].alpha < D_00425510[i]) {
                FILL_FADE(D_00425510, D_00447254, D_00446A58)
                for (i = 0; i < D_00446A4C; i++) {
                    D_00425510[i + 200] = SPRITE_PACKET_COLOR(&D_00447248[i])->alpha;
                }
                D_004472DA = D_004472CB = 1;
                break;
            }
        }
        break;
    case 3:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[253] -= 1.6f;
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A4C; i++) {
            D_00425510[i + 200] -= (f32)D_00447248[i].alpha / 10.0f;
        }
        for (i = 0; i < D_00446A58; i++) {
            D_00425510[i] -= (f32)D_00447254[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                func_001D2680();
                D_00446B20 = (void *)func_001D7C30;
                ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                D_004472CB = 0;
                D_004472CC = 0;
                D_00446AD4 = 0;
                break;
            }
        }
        break;
    }
}
