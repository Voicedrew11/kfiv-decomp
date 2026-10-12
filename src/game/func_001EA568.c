#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_link.h"
#include "kf_state.h"

extern const TradeValueTables D_00440300;
extern const TradeStockTables D_00440318;
extern const TradeGroupBases D_00440330;
extern const TradeValueSlots D_00440368;
extern LevelDef D_004147F8[0x17C];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern LevelRecordDef D_0041E118[];
extern PlayerItemDef D_0041B9B8[];
extern u8 D_00413FBC[];
extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern f32 D_00428A10[4];
extern f32 D_0042B7D0[4][4];
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
extern f32 D_00446CBC;
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
s32 func_001DFC60(HandlerContext *, s32);
int func_001E4FE0(s32, u32);
int func_001E5210(u8);
f32 func_001E6A20(s32, s32, f32);
void func_001E6AB8(HandlerContext *);
void func_001EBB98(HandlerContext *);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
int func_001FE9A8(LevelDef *, s32); // match: as func_001D6E60
void func_00200068(s32, s32);
void func_00207478(int, int, int, int);

/* match: the part number is passed sign-extended and no result is kept (the definition takes a u8 and returns int) */
#define SELECT_PART(part) ((void (*)(s32))func_001E5210)(part)

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
        SELECT_PART((s8)(packets)[i].unk42);                                                            \
        func_00207478((int)&(packets)[i], (packets)[i].unk3E + 1000, (param), (int)D_00425910[i]);      \
        SELECT_PART(0);                                                                                 \
    }

/* The trade screen while the list of what the player owns is shown: draws the list, the cursor bar, the turning
   model's matrix and the chosen item's values, and handles the buttons and the fades in and out. */
void func_001EA568(HandlerContext *context) {
    TradeStockTables stock = D_00440318; // match: copied to the frame and never read
    TradeValueTables values = D_00440300;
    s32 details_opened = 0;
    TradeGroupBases bases = D_00440330;
    MenuCursorBar bar;
    MenuCursorBar unused_bar; // match: the frame has room for a second bar
    MenuMatrix view_matrix;
    f32 local_matrix[4][4];
    s32 record;
    s32 i;
    s32 row;
    MenuItem *bar_node;
    MenuItem *slot_node;
    s32 kind;
    MenuItem *node;
    MenuItem *current;
    s32 bar_row;
    s32 bar_depth;
    s32 text_index;
    GsPackedWord position;
    LevelDef *item;
    s32 worn;
    SpritePacket *packets;
    u32 count;
    s32 texts;
    s32 text_offset;
    s32 group;

    if (D_00426110[0] == NULL) {
        D_00446CA0 = func_001E6AB8;
        return;
    }
    if (D_00447311 < 2) {
        func_001DFC60(context, D_00447318);
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
            case 1: // match: an empty case of its own
                break;
            case 2:
                func_00207478((int)&D_00446C00[i], kind + 1000, 2, (int)D_00425510[i]);
                break;
            case 3:
                if (D_00426110[0]->group != 11) {
                    func_00207478((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
                }
                break;
            }
        } else {
            func_00207478((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }

    for (bar_row = 0, bar_node = D_00426120[0]; bar_row < 7; bar_row++, bar_node = bar_node->prev) {
        if (bar_node == D_00426110[0]) {
            bar.first.f.lo16 = 0x6FE0;
            bar.first.f.mid16 = bar_row * 0xB0 + 0x7C30;
            bar_depth = 0x86470; // match: the depth goes through a local
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

    func_001FDCD0(D_00446C78);
    position.raw = 0x0C7A12007C307080L;
    row = 0;
    memset(&view_matrix, 0, 0x40);
    view_matrix.rows[2][2] = view_matrix.rows[1][1] = view_matrix.rows[0][0] = 0.8f;
    view_matrix.rows[3][3] = 1.0f;
    node = D_00426120[0];
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    D_00446CBC += 3.1415927f;
    D_00446CBC -= (s32)(D_00446CBC / 6.2831855f) * 6.2831855f;
    D_00446CBC -= 3.1415927f;
    sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)&view_matrix, D_00446CBC);
    sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, D_00428A10);
    func_001D6E60((f32 *)local_matrix);

    for (; row < 7; node = node->prev, row++) {
        item = &D_004147F8[node->unk0C];
        func_001FDB88(position);
        func_001FE9A8(item, 0x1A);
        position.f.mid16 += 0xB0;
        if (D_00426130[0]->prev == node) {
            break;
        }
    }

    slot_node = D_00426120[0];
    {
        TradeValueSlots slots = D_00440368;

        i = 0;
        do { // match: a for loop here gets its first pass peeled off
            *slots.shown_value[i] = values.values[context->arg][slot_node->unk0C].give_value;
            slot_node = slot_node->prev;
            if (D_00426120[0] == slot_node) {
                break;
            }
            i++;
        } while (i < 7);
        for (i++; i < 7; i++) {
            *slots.shown_value[i] = -1;
        }
    }

    if (D_00447318 == 0) {
        group = D_00426110[0]->group;
        if (group == 11) {
            group = 12;
        }
        if ((D_00446DE8 & 0x1000) && func_001E4FE0(record, group)) {
            details_opened = 1;
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
            if (details_opened != 0) {
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
            worn = D_00413FBC[D_00426110[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C14, D_00446A9C)
            }
            DRAW_PACKETS(D_00446C14, D_00446A9C, worn)
            packets = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (details_opened != 0) {
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
            worn = D_00413FBC[D_00426110[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C0C, D_00446A94)
            }
            DRAW_PACKETS(D_00446C0C, D_00446A94, worn)
            packets = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (details_opened != 0) {
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
        case 3:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= (f32)packets[i].alpha / 10.0f;
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

    current = D_00426110[0];
    texts = func_001CFC58(D_00446F38);
    text_index = current->unk0C + 0x1F5; // match: the index is worked out apart from the load
    text_offset = ((s32 *)texts)[text_index];
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
            D_00447311 = 3;
            D_00447312 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        }
    }
    if (D_00447311 == 2 && D_00446C90 == NULL) {
        D_00447311 = 4;
    }
    if (D_00447311 == 3 && D_00446C90 == NULL) {
        D_00447311 = 5;
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
            D_00425510[i] += (f32)D_00446C00[i].alpha / 10.0f;
            if ((f32)D_00446C00[i].alpha < D_00425510[i]) {
                D_00447318 = 0;
                D_00447311 = 1;
                FILL_FADE(D_00425510, D_00446C00, D_00446A78)
                return;
            }
        }
        break;
    case 4:
    case 5:
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
            D_00425510[i] -= (f32)D_00446C00[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447311 == 4) {
                    D_00446CA0 = func_001EBB98;
                    context->unk04 = 1;
                    D_00447310 = 0;
                } else if (D_00447311 == 5) {
                    context->unk04 = 1;
                    D_00446CA0 = func_001E6AB8;
                    ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
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
