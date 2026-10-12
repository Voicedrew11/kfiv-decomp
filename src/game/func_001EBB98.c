#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_link.h"
#include "kf_state.h"

extern const TradeValueTables D_00440300;
extern const TradeStockTables D_00440318;
extern const TradeGroupBases D_00440330;
extern void (*const D_004402F8[])(HandlerContext *);
extern u8 D_00369830[380];
extern LevelDef D_004147F8[0x17C];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern LevelRecordDef D_0041E118[];
extern PlayerItemDef D_0041B9B8[];
extern PlayerState D_00413F80;
extern u8 D_00413FBC[];
extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern f32 D_00425D10[0x100];
extern f32 D_00428A20[4];
extern f32 D_0042B7D0[4][4];
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern u32 D_00446A78;
extern u32 D_00446A94;
extern u32 D_00446A98;
extern u32 D_00446A9C;
extern u32 D_00446AB4;
extern SpritePacket *D_00446C00;
extern SpritePacket *D_00446C0C;
extern SpritePacket *D_00446C10;
extern SpritePacket *D_00446C14;
extern SpritePacket *D_00446C60;
extern MenuColor D_00446C78;
extern MenuColor D_00446C88;
extern LinkNode *D_00446C90;
extern u8 D_00446C9C;
extern void (*D_00446CA0)(HandlerContext *);
extern f32 D_00446CC0;
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
extern s32 D_00446E1C;
extern s32 D_00446E20;
extern s32 D_00446E24;
extern s32 D_00446E28;
extern s32 D_00446E2C;
extern s32 D_00446E3C;
extern s32 D_00446E44;
extern s32 D_00446E48;
extern s32 D_00446E4C;
extern s32 D_00446E50;
extern s32 D_00446F38;
extern s8 D_00447311;
extern s8 D_00447312;
extern s8 D_00447318;

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
s32 func_0019CDC0(s32);
s32 func_001CFC58(s32);
void func_001D45D0(void);
int func_001D6E60(f32 *); // match: an int result keeps $2 for the load after the call
int func_001E4FE0(s32, u32);
int func_001E5210(u8);
void func_001E54A8(s8);
void func_001E57A0(s32);
f32 func_001E6A20(s32, s32, f32);
void func_001E6AB8(HandlerContext *);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
int func_001FE9A8(LevelDef *, s32);
void func_001FF428(LevelDef *, s32);
void func_00200068(s32, s32);
void func_00207478(int, int, int, int);

/* match: the part number is passed sign-extended and no result is kept (the definition takes a u8 and returns int) */
#define SELECT_PART(part) ((void (*)(s32))func_001E5210)(part)

static inline s32 ScaleValue(s32 value, s32 percent, s32 divisor) {
    return value * percent / divisor;
}

/* Starts every packet of a list at its full alpha. match: the alpha goes through an unsigned local */
#define FILL_FADE(levels, packets, count)                   \
    for (i = 0; i < (count); i++) {                         \
        full_alpha = (packets)[i].alpha;                    \
        (levels)[i] = full_alpha;                           \
    }

/* Draws a list of packets at their current fade levels. */
#define DRAW_PACKETS(packets, count, param)                                                             \
    for (i = 0; i < (count); i++) {                                                                     \
        SELECT_PART((s8)(packets)[i].unk42);                                                            \
        func_00207478((int)&(packets)[i], (packets)[i].unk3E + 1000, (param), (int)D_00425910[i]);      \
        SELECT_PART(0);                                                                                 \
    }

/* The trade screen while the amount to give is chosen: draws the lists and the chosen item's values, and gives or cancels. */
void func_001EBB98(HandlerContext *context) {
    TradeStockTables stock = D_00440318;
    TradeValueTables values = D_00440300;
    TradeGroupBases bases = D_00440330;
    f32 local_matrix[4][4];
    f32 view_matrix[4][4];
    MenuCursorBar bar;
    s32 record;
    u32 i;
    u32 full_alpha;
    s32 row;
    s32 number;
    s32 kind;
    s32 glow;
    s32 group;
    s32 info_pressed;
    MenuItem *node;
    GsPackedWord position;
    LevelDef *chosen;
    s32 bar_drawn;
    SpritePacket *packets;
    u32 count;
    MenuItem *current;
    s32 texts;
    s32 text_offset;
    f32 angle;

    info_pressed = 0;
    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x20) {
            func_0019CDC0(1);
            context->unk04++;
        } else if (D_00446DE8 & 0x80) {
            func_0019CDC0(1);
            context->unk04--;
        }
    }
    if (D_00369830[D_00426110[0]->unk0C] < context->unk04) {
        context->unk04 = 1;
    } else if (context->unk04 <= 0) {
        context->unk04 = D_00369830[D_00426110[0]->unk0C];
    }
    D_00446E44 = D_00369830[D_00426110[0]->unk0C];
    D_00446E48 = stock.counts[context->arg][D_00426110[0]->unk0C];
    D_00446E4C = context->unk04;
    D_00446E50 = context->unk04 * values.values[context->arg][D_00426110[0]->unk0C].give_value;
    if (D_00446E48 >= 100) {
        D_00446E48 = 99;
    }
    record = D_00426110[0]->unk0C - bases.first_item[D_00426110[0]->group];
    if (D_00426110[0]->group == 0) {
        D_00446E3C = D_003679A0[record].unk14 + 1;
    }
    for (i = 0; i < D_00446A78; i++) {
        kind = D_00446C00[i].unk3E;
        SELECT_PART((s8)D_00446C00[i].unk42);
        if (kind == 3) {
            switch (D_00446C00[i].slot) {
            case 0:
                func_00207478((int)&D_00446C00[i], kind + 1000, 0, (int)D_00425510[i]);
                break;
            case 1:
                func_00207478((int)&D_00446C00[i], kind + 1000, 1, (int)D_00425510[i]);
                break;
            case 2:
                break;
            case 3:
                if (D_00426110[0]->group != 11) {
                    func_00207478((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
                }
                break;
            }
        } else {
            // match: 10000 where the other screens add 1000 (the callee takes it modulo 1000, so it draws the same)
            func_00207478((int)&D_00446C00[i], kind + 10000, 4, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }

    func_001FDCD0(D_00446C78);
    memset(view_matrix, 0, 0x40);
    view_matrix[2][2] = view_matrix[1][1] = view_matrix[0][0] = 0.8f;
    view_matrix[3][3] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CC0 + 3.1415927f;
    D_00446CC0 = angle - (s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)view_matrix, D_00446CC0);
    sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, D_00428A20);
    func_001D6E60((f32 *)local_matrix);

    if (D_00447318 == 0) {
        group = D_00426110[0]->group;
        if (group == 11) {
            group = 12;
        }
        if ((D_00446DE8 & 0x1000) && func_001E4FE0(record, group)) {
            info_pressed = 1;
            if (D_00446C9C == 0) {
                D_00446C9C = 1;
            } else {
                D_00447312 = 2;
            }
        }
    }
    if (D_00446C9C != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
        switch (D_00426110[0]->group) {
        case 11:
        case 12:
            packets = D_00446C14;
            count = D_00446A9C;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (info_pressed != 0) {
                func_0019CDC0(1);
            }
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + D_0041E118[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + D_0041E118[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + D_0041E118[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(D_0041E118[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(D_0041E118[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(D_0041E118[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(D_0041E118[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(D_0041E118[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(D_0041E118[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E24 = D_003682F0[record].level;
            D_00446E28 = 100;
            D_00446E1C = D_004147F8[D_00426110[0]->unk0C].unk3E;
            glow = D_00413FBC[D_00426110[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C14, D_00446A9C)
            }
            DRAW_PACKETS(D_00446C14, D_00446A9C, glow)
            packets = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (info_pressed != 0) {
                func_0019CDC0(1);
            }
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + D_0041E118[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + D_0041E118[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + D_0041E118[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(D_0041E118[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(D_0041E118[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(D_0041E118[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(D_0041E118[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(D_0041E118[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(D_0041E118[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E1C = D_004147F8[D_00426110[0]->unk0C].unk3E;
            glow = D_00413FBC[D_00426110[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C0C, D_00446A94)
            }
            DRAW_PACKETS(D_00446C0C, D_00446A94, glow)
            packets = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (info_pressed != 0) {
                func_0019CDC0(1);
            }
            D_00446DF4 = ScaleValue(D_003679A0[record].unk00[0] + D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[0], D_003679A0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003679A0[record].unk00[1] + D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[1], D_003679A0[record].level, 100);
            D_00446DFC = ScaleValue(D_003679A0[record].unk00[2] + D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[2], D_003679A0[record].level, 100);
            D_00446E00 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[3], D_003679A0[record].level, 100);
            D_00446E04 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[4], D_003679A0[record].level, 100);
            D_00446E08 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[5], D_003679A0[record].level, 100);
            D_00446E0C = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[6], D_003679A0[record].level, 100);
            D_00446E10 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[7], D_003679A0[record].level, 100);
            D_00446E14 = ScaleValue(D_0041B9B8[record].unk00[D_003679A0[record].unk14].values[8], D_003679A0[record].level, 100);
            D_00446E20 = (s32)(D_0041B9B8[record].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[record].level;
            D_00446E1C = D_004147F8[D_00426110[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_00426110[0]->group];
            D_00446E2C = 100;
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C10, D_00446A98)
            }
            DRAW_PACKETS(D_00446C10, D_00446A98, 0)
            packets = D_00446C10;
            count = D_00446A98;
            break;
        }

        switch (D_00447312) {
        case 0:
            for (i = 0; i < count; i++) {
                D_00425910[i] += (f32)packets[i].alpha / 10.0f;
                if ((f32)packets[i].alpha < D_00425910[i]) {
                    FILL_FADE(D_00425910, packets, count)
                    D_00447312 = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= (f32)packets[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    D_00447312 = 0;
                    D_00446C9C = 0;
                    break;
                }
            }
            break;
        }
    }

    node = D_00426120[0];
    position.raw = 0x0C0975E07C307080L;
    func_001FDB88(position);
    for (row = 0; row < 7; row++) {
        func_001FE9A8(&D_004147F8[node->unk0C], 0x1A);
        if (D_00426130[0]->prev == node) {
            break;
        }
        node = node->prev;
    }

    bar_drawn = 0;
    for (i = 0; i < D_00446AB4; i++) {
        if (bar_drawn == 0 && D_00446C60[i].unk43 == 1) {
            bar.first.f.lo16 = 0x7850;
            bar.first.f.mid16 = 0x80B0;
            bar.first.f.mid24 = 250000;
            bar.second.f.lo16 = bar.first.f.lo16 + 0xFF0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar_drawn = 1;
            if (D_00447311 == 1) {
                D_00425510[0xFD] = func_001E6A20(0x10, 0x20, D_00425510[0xFD]);
            }
            bar.alpha = (s32)D_00425510[0xFD];
            func_001FC5E8(&bar);
        }
        kind = D_00446C60[i].unk3E;
        SELECT_PART((s8)D_00446C60[i].unk42);
        func_00207478((int)&D_00446C60[i], kind + 1000, 4, (int)D_00425D10[i]);
        SELECT_PART(0);
    }

    chosen = &D_004147F8[D_00426110[0]->unk0C];
    func_001FDCD0(D_00446C78);
    position.raw = 0x0A2DC6C07DB078C0L;
    func_001FDB88(position);
    func_001FF428(chosen, 0x18);

    current = D_00426110[0];
    texts = func_001CFC58(D_00446F38);
    number = current->unk0C + 0x1F5; // match: its own local, or the offset is folded into the load
    text_offset = ((s32 *)texts)[number];
    position.raw = 0x000A875083207080L;
    func_001FDB88(position);
    if (text_offset != 0) {
        func_00200068(texts + text_offset, 0x33);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            func_0019CDC0(0);
            D_00413F80.unk4C += D_00446E50;
            if (D_00413F80.unk4C > 999999) {
                D_00413F80.unk4C = 999999;
            }
            D_00369830[D_00426110[0]->unk0C] -= context->unk04;
            if (D_00426110[0]->unk0C == D_00413F80.unk3C + 150 || D_00426110[0]->unk0C == D_00413F80.equipment[0] + 220 ||
                D_00426110[0]->unk0C == D_00413F80.equipment[2] + 220 || D_00426110[0]->unk0C == D_00413F80.equipment[1] + 220 ||
                D_00426110[0]->unk0C == D_00413F80.equipment[3] + 220 || D_00426110[0]->unk0C == D_00413F80.equipment[4] + 220 ||
                D_00426110[0]->unk0C == D_00413F80.equipment[5] + 220 || D_00426110[0]->unk0C == D_00413F80.equipment[6] + 220 ||
                D_00426110[0]->unk0C == D_00413F80.equipment[7] + 220 || D_00426110[0]->unk0C == D_00413F80.unk46) {
                if (D_00369830[D_00426110[0]->unk0C] == 0) {
                    func_001E54A8(D_00426110[0]->group);
                }
            }
            if (stock.counts[context->arg][D_00426110[0]->unk0C] != 0xFF) {
                stock.counts[context->arg][D_00426110[0]->unk0C] += context->unk04;
                if (stock.counts[context->arg][D_00426110[0]->unk0C] >= 100) {
                    stock.counts[context->arg][D_00426110[0]->unk0C] = 99;
                }
            }
            D_00447311 = 3;
            D_00447312 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else if (D_00446DE8 & 0x2000) {
            func_0019CDC0(2);
            D_00447311 = 4;
            D_00447312 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        }
    }
    if (D_00447311 == 3 && D_00446C90 == NULL) {
        D_00447311 = 5;
    }
    if (D_00447311 == 4 && D_00446C90 == NULL) {
        D_00447311 = 6;
    }

    switch (D_00447311) {
    case 0:
        D_00425510[0xFF] += 12.8f;
        if (D_00425510[0xFF] > 128.0f) {
            D_00425510[0xFF] = 128.0f;
        }
        D_00425510[0xFD] += 1.6f;
        D_00446C78.alpha = (s32)D_00425510[0xFF];
        if (D_00425510[0xFD] > 16.0f) {
            D_00425510[0xFD] = 16.0f;
        }
        for (i = 0; i < D_00446A78; i++) {
            D_00425510[i] += (f32)D_00446C00[i].alpha / 10.0f;
            if ((f32)D_00446C00[i].alpha < D_00425510[i]) {
                D_00447318 = 0;
                D_00447311 = 1;
                FILL_FADE(D_00425510, D_00446C00, D_00446A78)
                break;
            }
        }
        for (i = 0; i < D_00446AB4; i++) {
            D_00425D10[i] += (f32)D_00446C60[i].alpha / 10.0f;
            if ((f32)D_00446C60[i].alpha < D_00425D10[i]) {
                FILL_FADE(D_00425D10, D_00446C60, D_00446AB4)
                break;
            }
        }
        break;
    case 5:
    case 6:
        D_00425510[0xFF] -= 12.8f;
        if (D_00425510[0xFF] < 0.0f) {
            D_00425510[0xFF] = 0.0f;
        }
        D_00425510[0xFE] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[0xFF];
        if (D_00425510[0xFE] < 0.0f) {
            D_00425510[0xFE] = 0.0f;
        }
        D_00425510[0xFD] -= 1.6f;
        D_00446C88.alpha = (s32)D_00425510[0xFE];
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A78; i++) {
            D_00425510[i] -= (f32)D_00446C00[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447311 == 5) {
                    context->unk04 = 1;
                    D_00446CA0 = func_001E6AB8;
                    func_001E57A0(context->arg);
                    ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                } else if (D_00447311 == 6) {
                    D_00446CA0 = D_004402F8[1];
                }
                D_00447311 = 0;
                D_00447312 = 0;
                memset(D_00425510, 0, 0x400);
                break;
            }
        }
        for (i = 0; i < D_00446AB4; i++) {
            D_00425D10[i] -= (f32)D_00446C60[i].alpha / 10.0f;
            if (D_00425D10[i] < 0.0f) {
                memset(D_00425D10, 0, 0x400);
                break;
            }
        }
        break;
    }
}
