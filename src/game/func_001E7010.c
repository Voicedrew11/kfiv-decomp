#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_link.h"
#include "kf_state.h"

extern const TradeValueTables D_00440300;
extern const TradeStockTables D_00440318;
extern const TradeGroupBases D_00440330;
extern const TradeValueSlots D_00440368;
extern u8 D_00369830[380];
extern LevelDef D_004147F8[0x17C];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern LevelRecordDef D_0041E118[];
extern PlayerItemDef D_0041B9B8[];
extern u8 D_00413FBC[];
extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern TradeValue D_00426620[4];
extern TradeValue D_00426630[4];
extern TradeValue D_00426640[4];
extern f32 D_004289F0[4];
extern f32 D_0042B7D0[4][4];
extern MenuItem *D_005CF360[4];
extern MenuItem *D_005CF370[4];
extern MenuItem *D_005CF380[4];
extern u32 D_00446A78;
extern u32 D_00446A94;
extern u32 D_00446A98;
extern u32 D_00446A9C;
extern SpritePacket *D_00446C00;
extern SpritePacket *D_00446C0C;
extern SpritePacket *D_00446C10;
extern SpritePacket *D_00446C14;
extern MenuColor D_00446C78;
extern MenuColor D_00446C88;
extern LinkNode *D_00446C90;
extern u8 D_00446C9C;
extern void (*D_00446CA0)(HandlerContext *);
extern int D_00446CA4;
extern s32 D_00446CAC;
extern f32 D_00446CB0;
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
extern s32 D_00446E80;
extern s32 D_00446F38;
extern s8 D_00447310;
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
f32 func_001E6A20(s32, s32, f32);
void func_001E6AB8(HandlerContext *);
void func_001E88A0(HandlerContext *);
s32 func_001ED738(HandlerContext *);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
void func_001FDD28(TradeValue *, s32);
int func_001FE9A8(LevelDef *, s32); // match: as func_001D6E60
void func_00200068(s32, s32);
void func_00207478(int, int, int, int);

/* match: the part number is passed sign-extended and no result is kept (the definition takes a u8 and returns int) */
#define SELECT_PART(part) ((void (*)(s32))func_001E5210)(part)

/* value * percent / divisor. match: an inline function, so every use loads its own divisor (the dead `li 100`s) */
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

/* The trade screen while the stock list is shown: draws it and the chosen item's values, and handles the buttons. */
void func_001E7010(HandlerContext *context) {
    TradeStockTables stock = D_00440318; // match: copied but never read
    TradeValueTables values = D_00440300;
    TradeGroupBases bases = D_00440330;
    MenuCursorBar bar;
    s32 record;
    s32 i;
    s32 kind;
    MenuItem *node;
    MenuItem *bar_node;
    MenuItem *current;
    MenuItem *value_node;
    s32 bar_row;
    s32 bar_depth;
    s32 row;
    s32 text_number;
    GsPackedWord position;
    GsPackedWord row_position;
    TradeValue *label;
    LevelDef *item;
    s32 glow;
    SpritePacket *packets;
    u32 full_alpha;
    u32 count;
    s32 texts;
    s32 text_offset;

    if (D_005CF380[0] == NULL) {
        D_00446CA0 = func_001E6AB8;
        return;
    }
    if (D_00447311 < 2) {
        func_001ED738(context);
    }
    record = D_005CF380[0]->unk0C - bases.first_item[D_005CF380[0]->group];
    if (context->arg == 4) {
        switch (D_004147F8[D_005CF380[0]->unk0C].unk30_20 & 3) {
        case 1:
            D_00446E80 = D_00369830[0x1F];
            break;
        case 2:
            D_00446E80 = D_00369830[0x20];
            break;
        case 3:
            D_00446E80 = D_00369830[0x21];
            break;
        }
    }
    if (D_005CF380[0]->group == 0) {
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
                break;
            case 2:
                if (context->arg != 4) {
                    func_00207478((int)&D_00446C00[i], kind + 1000, 2, (int)D_00425510[i]);
                }
                break;
            case 3:
                if (D_005CF380[0]->group != 12) {
                    func_00207478((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
                }
                break;
            case 4:
                if (context->arg == 4) {
                    func_00207478((int)&D_00446C00[i], kind + 1000, 4, (int)D_00425510[i]);
                }
                break;
            }
        } else if (context->arg != 4) {
            func_00207478((int)&D_00446C00[i], kind + 1000, 1, (int)D_00425510[i]);
        } else {
            func_00207478((int)&D_00446C00[i], kind + 1000, 5, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }

    bar_node = D_005CF360[0];
    for (bar_row = 0; bar_row < 7; bar_row++, bar_node = bar_node->prev) {
        if (bar_node == D_005CF380[0]) {
            bar.first.f.lo16 = 0x6FE0;
            bar.first.f.mid16 = bar_row * 0xB0 + 0x7C30;
            bar_depth = 0x86470; // match: the depth goes through a variable (its mask is loaded before the field's)
            bar.first.f.mid24 = bar_depth;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            if (D_00447311 == 1) {
                D_00425510[0xFD] = func_001E6A20(0x10, 0x20, D_00425510[0xFD]);
            }
            bar.alpha = (s32)D_00425510[0xFD];
            func_001FC5E8(&bar);
            break;
        }
    }

    if (context->arg == 4) {
        position.raw = 0x000DBBA079E08A30L;
        switch (D_004147F8[D_005CF380[0]->unk0C].unk30_20 & 3) {
        case 1:
            label = D_00426620;
            break;
        case 2:
            position.raw = 0x000DBBA079E089C0L;
            label = D_00426630;
            break;
        case 3:
            label = D_00426640;
            break;
        default:
            position.f.lo16 = 0x89C0;
            label = D_00426630;
            break;
        }
        func_001FDB88(position);
        func_001FDD28(label, 10);
    }

    func_001FDCD0(D_00446C78);
    row_position.raw = 0x0C7A12007C307080L;
    for (row = 0, node = D_005CF360[0]; row < 7; node = node->prev, row++) {
        item = &D_004147F8[node->unk0C];
        func_001FDB88(row_position);
        func_001FE9A8(item, 0x1A);
        row_position.f.mid16 += 0xB0;
        if (D_005CF370[0]->prev == node) {
            break;
        }
    }

    /* the values of the (up to seven) listed items; -1 for the rows left empty */
    {
        // match: the list head is read before the table is copied, and the loop reads it again
        MenuItem *first_node = D_005CF360[0];
        TradeValueSlots slots = D_00440368;
        MenuItem *next_node;

        i = 0;
        *slots.shown_value[i] = values.values[context->arg][first_node->unk0C].take_value;
        value_node = first_node->prev;
        if (first_node != value_node) {
            do {
                i++;
                if (i >= 7) {
                    break;
                }
                *slots.shown_value[i] = values.values[context->arg][value_node->unk0C].take_value;
                next_node = value_node->prev;
                value_node = next_node;
            } while (D_005CF360[0] != next_node);
        }
        for (i++; i < 7; i++) {
            *slots.shown_value[i] = -1;
        }
    }

    /* the chosen item's model: scaled to 0.8, turned by D_00446CB0 (kept within -pi..pi) and moved to D_004289F0 */
    {
        // match: declared in this block, after the one above, so they come after `slots` in the frame
        f32 local_matrix[4][4];
        f32 view_matrix[4][4];

        memset(view_matrix, 0, 0x40);
        view_matrix[2][2] = view_matrix[1][1] = view_matrix[0][0] = 0.8f;
        view_matrix[3][3] = 1.0f;
        sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        D_00446CB0 += 3.1415927f;
        D_00446CB0 -= (s32)(D_00446CB0 / 6.2831855f) * 6.2831855f;
        D_00446CB0 -= 3.1415927f;
        sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)view_matrix, D_00446CB0);
        sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, D_004289F0);
        func_001D6E60((f32 *)local_matrix);
    }

    if (D_00447318 == 0) {
        if ((D_00446DE8 & 0x1000) && func_001E4FE0(record, D_005CF380[0]->group)) {
            D_00446CAC = 1;
            if (D_00446C9C == 0) {
                D_00446C9C = 1;
            } else {
                D_00447312 = 2;
            }
        }
    }

    if (D_00446C9C != 0) {
        // match: groups 1 and 10 have no case, so packets and count are read unset by the fade below
        switch (D_005CF380[0]->group) {
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
            if (D_00446CAC != 0) {
                func_0019CDC0(1);
            }
            D_00446CAC = 0;
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
            D_00446E1C = D_004147F8[D_005CF380[0]->unk0C].unk3E;
            glow = D_00413FBC[D_005CF380[0]->group];
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
            if (D_00446CAC != 0) {
                func_0019CDC0(1);
            }
            D_00446CAC = 0;
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + D_0041E118[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + D_0041E118[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + D_0041E118[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(D_0041E118[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(D_0041E118[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(D_0041E118[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(D_0041E118[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(D_0041E118[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(D_0041E118[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E1C = D_004147F8[D_005CF380[0]->unk0C].unk3E;
            glow = D_00413FBC[D_005CF380[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C0C, D_00446A94)
            }
            DRAW_PACKETS(D_00446C0C, D_00446A94, glow)
            packets = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (D_00446CAC != 0) {
                func_0019CDC0(1);
            }
            D_00446CAC = 0;
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
            D_00446E1C = D_004147F8[D_005CF380[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_005CF380[0]->group];
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
                D_00425910[i] += packets[i].alpha / 10.0f;
                if (packets[i].alpha < D_00425910[i]) {
                    FILL_FADE(D_00425910, packets, count)
                    D_00447312 = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= packets[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    if (D_00447312 == 2) {
                        D_00446C9C = 0;
                    }
                    D_00447312 = 0;
                    break;
                }
            }
            break;
        }
    }

    current = D_005CF380[0];
    texts = func_001CFC58(D_00446F38);
    text_number = current->unk0C + 0x1F5; // match: a variable, so the 0x1F5 is added before the index is scaled
    text_offset = ((s32 *)texts)[text_number];
    func_001FDCD0(D_00446C78);
    position.raw = 0x000A875083207080L;
    func_001FDB88(position);
    if (text_offset != 0) {
        func_00200068(texts + text_offset, 0x33);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            func_0019CDC0(0);
            D_00447311 = 2;
            D_00447312 = 3;
            D_00447318 = 1;
        } else if (D_00446DE8 & 0x2000) {
            func_0019CDC0(2);
            if (context->arg != 4) {
                D_00447311 = 3;
            } else {
                D_00447311 = 4;
            }
            D_00447312 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        }
    }
    if (D_00447311 == 2 && D_00446C90 == NULL) {
        D_00447311 = 5;
    }
    if (D_00447311 == 3 && D_00446C90 == NULL) {
        D_00447311 = 6;
    }
    if (D_00447311 == 4 && D_00446C90 == NULL) {
        D_00447311 = 7;
    }

    switch (D_00447311) {
    case 0:
        D_00425510[0xFF] += 12.8f;
        if (D_00425510[0xFF] > 128.0f) {
            D_00425510[0xFF] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[0xFF];
        D_00425510[0xFD] += 1.6f;
        if (D_00425510[0xFD] > 16.0f) {
            D_00425510[0xFD] = 16.0f;
        }
        for (i = 0; i < D_00446A78; i++) {
            D_00425510[i] += D_00446C00[i].alpha / 10.0f;
            if (D_00446C00[i].alpha < D_00425510[i]) {
                D_00447318 = 0;
                D_00447311 = 1;
                FILL_FADE(D_00425510, D_00446C00, D_00446A78)
                return;
            }
        }
        break;
    case 5:
    case 6:
    case 7:
        D_00425510[0xFF] -= 12.8f;
        if (D_00425510[0xFF] < 0.0f) {
            D_00425510[0xFF] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[0xFF];
        D_00425510[0xFE] -= 12.8f;
        if (D_00425510[0xFE] < 0.0f) {
            D_00425510[0xFE] = 0.0f;
        }
        D_00446C88.alpha = (s32)D_00425510[0xFE];
        D_00425510[0xFD] -= 1.6f;
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A78; i++) {
            D_00425510[i] -= D_00446C00[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447311 == 5) {
                    D_00446CA0 = func_001E88A0;
                    context->unk04 = 1;
                    D_00447310 = 0;
                } else if (D_00447311 == 6) {
                    context->unk04 = 0;
                    D_00446CA0 = func_001E6AB8;
                    ((void (*)(s32))func_001D45D0)(0);
                } else if (D_00447311 == 7) {
                    context->unk04 = 0;
                    D_00446CA4++;
                    ((void (*)(s32))func_001D45D0)(0);
                }
                D_00447311 = 0;
                D_00447312 = 0;
                memset(D_00425510, 0, 0x400);
                return;
            }
        }
        break;
    }
}
