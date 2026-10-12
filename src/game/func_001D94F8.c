/* The equipment screen, run every frame through D_00446B20: moves between the eleven slots and inside the slot's list,
 * draws the screen's parts, the worn weapon's model and the cursor item's model, the item's detail panel (button
 * 0x1000) or the list of everything worn (0x8000), equips the cursor item (0x4000) and fades the screen in and out.
 * Has a jump table (jtbl_0043FDE0) and owns the rodata D_0043FD28..D_0043FDD0.
 */
#include "kf_effect.h"
#include "kf_game.h"
#include "kf_link.h"
#include "kf_load.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_screen.h"
#include "kf_state.h"
#include <eeregs.h>

extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern FxTableEntry D_00402DA8[];
extern PlayerState D_00413F80;
extern u8 D_00413FBC[];
extern LevelDef D_004147F8[380];
extern PlayerItemDef D_0041B9B8[];
extern LevelRecordDef D_0041E118[];
extern MenuItem **D_00420D60[11];
extern MenuItem **D_00420D90[11];
extern LinkNode *D_00420DC8[];
extern f32 D_00421020[4];
extern f32 D_00421040[4];
extern MenuItem D_004231F0[70];
extern MenuItem D_00423650[30];
extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern LinkNode D_00426470;
extern void (*D_0042A290[])(PoolSlot *, MenuColor);
extern PoolSlot D_0042A8F0[];
extern f32 D_0042B7D0[16];
extern const EquipGroupBases D_0043FD28;
extern const EquipUnusedTable D_0043FD58;
extern const VuVector4 D_0043FD90;
extern const VuVector4 D_0043FDA0;
extern const VuVector4 D_0043FDB0;
extern const EquipGroupBytes D_0043FDC0;
extern const EquipGroupBytes D_0043FDD0;
extern u32 D_00446A3C;
extern u32 D_00446A40;
extern u32 D_00446A44;
extern u32 D_00446A48;
extern u32 D_00446A7C;
extern u32 D_00446A80;
extern u32 D_00446AC0;
extern s32 D_00446ACC;
extern u8 D_00446AD4;
extern u8 D_00446AD5;
extern void *D_00446B20;
extern s8 D_00446B50;
extern f32 D_00446B54;
extern f32 D_00446B58;
extern u32 D_00446B5C;
extern SpritePacket *D_00446C6C;
extern MenuColor D_00446C78;
extern MenuColor D_00446C80;
extern MenuColor D_00446C88;
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
extern s32 D_00446E30;
extern SpritePacket *D_00447238;
extern SpritePacket *D_0044723C;
extern SpritePacket *D_00447240;
extern SpritePacket *D_00447244;
extern SpritePacket *D_00447268;
extern SpritePacket *D_0044726C;
extern MenuFreeNode D_004472B8;
extern s8 D_004472C6;
extern s8 D_004472C7;
extern char D_004472CE;
extern char D_004472DA;
extern s32 D_004472E0;
extern int D_004472F0;
extern MenuItem *D_005CE840[];
extern MenuItem *D_005CE6B0[];
extern LinkNode D_005CED40;
extern MenuModelMotion *D_005CEE60[];

// match: D_004472C7 is the second byte of what the symbol list has as one two-byte object. It has to be a variable of
// its own: a store to `D_004472C6[1]` is never put in a delay slot, and retail has it there.
__asm__("D_004472C7 = D_004472C6 + 1");

void *memset(void *, int, u32);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
s32 func_0019CDC0(s32);
void func_001BA310(void);
void func_001D2680(void);
void func_001D37F8(int, void *);
void func_001D39F0(s32); // match: this file only matches with this type; func_001D39F0.c defines it differently
void func_001D45D0(void);
void func_001D4840(void);
void func_001D7C30(void);
int func_001DFFF8(void);
int func_001E44D0(s32);
int func_001E4FE0(s32, u32);
int func_001E5210(u8);
int func_001E5660(u32, s32, s32);
s8 func_001FC0C0(s32);
void func_001FC0D8(KindObject *);
void func_001FDCD0(MenuColor);
void func_001FDD28(void *, s32);
void func_001FF428(void *, s32);
void func_00201E10(ChunkUpload *, s32, RenderSlotArg);
void func_002020A0(RenderSlot *, RenderSlotArg);
s32 func_00203770(void);
void func_00203820(RelocEntry *, PoolSlot *, f32 *, s32);
s32 func_00207320(int, s64);
void func_00207478(int, int, int, int);

/* the item the cursor is on in the category shown */
// match: the category is a MenuItem whose first word is a MenuItem pointer; read as an int it cannot pass the GS stores
#define CURRENT_ITEM (((MenuItemList *)D_005CE840[0])->item)

// match: called as a void function, so the result register is free at once
#define SET_BLEND(mode) ((void (*)(u8))func_001E5210)(mode)

// match: called with the node, a matrix and the colour, not as the definition has it
#define DRAW_ITEM_MODEL(matrix) ((void (*)(LinkNode *, f32 *, MenuColor))func_00201E10)(&D_00426470, matrix, D_00446C88)

// match: called with the slot alone
#define DRAW_SLOT(slot) ((void (*)(PoolSlot *))func_002020A0)(slot)

// match: forty dead additions. Retail's source gave about forty more RTL instructions than this one before the GCSE
// pass; they leave no code, but they size that pass's hash table, and its bucket order numbers the five spilled stack
// addresses (0x330..0x340 of the frame). What they were is not known; 38 to 44 additions give retail's bytes.
#define DEAD_ADDITIONS_8(first, second)                         \
    first += second;                                            \
    second += first;                                            \
    first += second;                                            \
    second += first;                                            \
    first += second;                                            \
    second += first;                                            \
    first += second;                                            \
    second += first

#define GIF_SEND(packet, count)                                 \
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));          \
    *D2_QWC = count;                                            \
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;         \
    *D2_CHCR = 0x100

#define RESET_ALPHAS(parts, count)                              \
    for (i = 0; i < count; i++) {                               \
        alpha = parts[i].alpha;                                 \
        D_00425910[i] = alpha;                                  \
    }

#define DRAW_PARTS(parts, count, variant)                       \
    for (i = 0; i < count; i++) {                               \
        SET_BLEND(parts[i].unk42);                          \
        func_00207478((int)&parts[i], parts[i].unk3E + 1000, variant, (int)D_00425910[i]); \
        SET_BLEND(0);                                       \
    }

void func_001D94F8(void) {
    EquipGroupBases bases = D_0043FD28;
    f32 item_matrix[16];
    f32 unused_matrix[16];
    f32 weapon_matrix[16];
    f32 unit[16];
    f32 scale[16];
    EquipUnusedTable unused_table;
    LinkNode node;
    VuVector4 first_offset;
    VuVector4 second_offset;
    VuVector4 third_offset;
    MenuGlyphSprite sprite;
    GsPackedWord word;
    FxTableEntry *magic;
    void (*draw)(PoolSlot *, MenuColor);
    void *listed;
    u32 *scissor_packet;
    u32 *full_packet;
    u32 *packet;
    u32 count;
    u32 alpha;
    s32 i;
    s32 index;
    s32 offset;
    f32 angle;
    s8 glyph;
    s8 width;
    u8 kind;
    s32 filler_first;
    s32 filler_second;

    memset(unit, 0, 0x40);
    unit[15] = unit[10] = unit[5] = unit[0] = 1.0f;
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.9f;
    scale[15] = 1.0f;
    unused_table = D_0043FD58;
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    {
    void *definition;
    SpritePacket *parts;
    if ((D_00446DE8 & 0x10) && D_004472CE != 0 && D_004472DA != 0 && D_004472F0 == 0) {
        D_005CE840[0] = D_005CE840[0]->next;
        ((void (*)(s32))func_001D37F8)(0); // match: called with the number alone
        D_004472E0--;
        func_0019CDC0(1);
    } else if ((D_00446DE8 & 0x40) && D_004472CE != 0 && D_004472DA != 0 && D_004472F0 == 0) {
        D_005CE840[0] = D_005CE840[0]->prev;
        func_001D39F0(0);
        D_004472E0++;
        func_0019CDC0(1);
    } else if (D_004472C6 < 2 && D_004472F0 == 0) {
        ((void (*)(s32))func_001DFFF8)(0); // match: the callee takes nothing
    }
    if (D_004472E0 > 10) {
        D_004472E0 = 0;
    } else if (D_004472E0 < 0) {
        D_004472E0 = 10;
    }
    index = CURRENT_ITEM->unk0C - bases.first_item[CURRENT_ITEM->group];
    if (CURRENT_ITEM->group == 0) {
        D_00446E30 = D_003679A0[index].unk14 + 1;
    } else if (CURRENT_ITEM->group == 1) {
        D_00446E30 = (index - 1) % 3 + 1;
    }
    for (i = 0; i < D_00446A3C; i++) {
        kind = D_00447238[i].unk3E;
        SET_BLEND(D_00447238[i].unk42);
        if (func_001E5660(D_004472E0, CURRENT_ITEM->group, CURRENT_ITEM->unk0C)) {
            func_00207478((int)&D_00447238[i], kind + 1000, 0, (int)D_00425510[i]);
        } else {
            func_00207478((int)&D_00447238[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        SET_BLEND(0);
    }
    if ((D_00446DE8 & 0x1000) && D_00446AD5 == 0 && D_004472DA != 0
        && func_001E4FE0(index, CURRENT_ITEM->group)) {
        if (D_00446AD4 == 0) {
            D_00446B50 = 1;
            D_00446AD4 = 1;
        } else if (D_004472C7 != 2) {
            func_0019CDC0(1);
            D_004472C7 = 2;
        } else {
            D_004472C7 = 0;
        }
    }
    if ((D_00446DE8 & 0x8000) && D_00446AD4 == 0 && D_004472DA != 0) {
        func_0019CDC0(1);
        if (D_00446AD5 == 0) {
            D_00446AD5 = 1;
        } else if (D_004472C7 != 2) {
            D_004472C7 = 2;
        } else {
            D_004472C7 = 0;
        }
    }
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    if (D_00446DE8 & 0x800) {
        D_00446B58 += 0.03f;
    } else if (D_00446DE8 & 0x400) {
        D_00446B58 -= 0.03f;
    }
    D_00446B58 -= (f32)(s32)(D_00446B58 / 3.1415927f) * 6.2831855f;
    sceVu0RotMatrixY(weapon_matrix, unit, D_00446B58);
    sceVu0TransMatrix(weapon_matrix, weapon_matrix, D_00421040);
    node.pool_node = &D_004472B8;
    node.unk20 = 1;
    node.unk00 = 0;
    D_00446ACC = D_0041B9B8[D_00413F80.unk3C].unk7C;
    if (D_0041B9B8[D_00413F80.unk3C].unk63 == 0) {
        D_00446ACC += D_0041E118[D_00413F80.equipment[0]].unk3F;
    }
    func_00203820((RelocEntry *)D_005CEE60[D_00446ACC]->frames, D_0042A8F0, weapon_matrix, D_00446B5C);
    D_00446B5C++;
    D_00446B5C %= D_005CEE60[D_00446ACC]->frame_count;
    // match: a pointer-typed load, as in CURRENT_ITEM
    D_00446ACC = ((PlayerItemDef *)((MenuItemList *)D_005CE6B0[0])->item)->unk7D;
    switch (D_0041B9B8[D_00413F80.unk3C].unk63) {
    case 0:
        D_00446ACC += D_00413F80.equipment[0] == 0;
        break;
    case 1:
        D_00446ACC++;
        break;
    case 2:
        D_00446ACC = 0;
        break;
    }
    draw = D_0042A290[D_00446ACC];
    scissor_packet = func_001FBE70();
    scissor_packet[0] = 0x8001;
    scissor_packet[1] = 0x10000000;
    scissor_packet[2] = 0xE;
    scissor_packet[3] = 0;
    scissor_packet[4] = 0x02590150;
    scissor_packet[5] = 0x00AD0015;
    scissor_packet[6] = 0x40;
    scissor_packet[7] = 0;
    GIF_SEND(scissor_packet, 2);
    first_offset = D_0043FD90;
    second_offset = D_0043FDA0;
    third_offset = D_0043FDB0;
    sceVu0TransMatrix((f32 *)&D_0042A8F0[21], unit, &first_offset.x);
    sceVu0TransMatrix((f32 *)&D_0042A8F0[22], unit, &second_offset.x);
    sceVu0TransMatrix((f32 *)&D_0042A8F0[23], unit, &third_offset.x);
    DRAW_SLOT(&D_0042A8F0[21]);
    DRAW_SLOT(&D_0042A8F0[22]);
    DRAW_SLOT(&D_0042A8F0[23]);
    draw(D_0042A8F0, D_00446C88);
    full_packet = func_001FBE70();
    full_packet[0] = 0x8001;
    full_packet[1] = 0x10000000;
    full_packet[2] = 0xE;
    full_packet[3] = 0;
    full_packet[4] = 0x02800000;
    full_packet[5] = 0x00E00000;
    full_packet[6] = 0x40;
    full_packet[7] = 0;
    GIF_SEND(full_packet, 2);
    packet = func_001FBE70();
    packet[0] = 0x8002;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0x7580;
    packet[5] = 0x7940;
    packet[6] = 0x19;
    packet[7] = 0;
    packet[8] = 0x44;
    packet[9] = 0;
    packet[10] = 0x43;
    packet[11] = 0;
    GIF_SEND(packet, 3);
    if (CURRENT_ITEM->group == 1) {
        if (D_004472C6 != 2 && D_00426470.unk20 != 0) {
            D_004472CE = 1;
            D_00425510[254] += 12.8f;
            if (D_00425510[254] > 128.0f) {
                D_00425510[254] = 128.0f;
            }
            D_00446C88.alpha = (s32)D_00425510[254];
        }
        sceVu0TransMatrix(item_matrix, scale, D_00421020);
        DRAW_ITEM_MODEL(item_matrix);
    } else {
        if (D_004472C6 != 2 && D_00426470.unk20 != 0) {
            D_004472CE = 1;
            D_00425510[254] += 12.8f;
            if (D_00425510[254] > 128.0f) {
                D_00425510[254] = 128.0f;
            }
            D_00446C88.alpha = (s32)D_00425510[254];
        }
        angle = D_00446B54 + 3.1415927f;
        D_00446B54 = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
        sceVu0RotMatrixY(item_matrix, scale, D_00446B54);
        sceVu0TransMatrix(item_matrix, item_matrix, D_00421020);
        DRAW_ITEM_MODEL(item_matrix);
    }
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    if (CURRENT_ITEM->group == 1) {
        definition = &D_00402DA8[CURRENT_ITEM->unk0C];
    } else {
        definition = &D_004147F8[CURRENT_ITEM->unk0C];
    }
    if (D_00446AD4 != 0) {
        switch (CURRENT_ITEM->group) {
        case 0:
            D_00446DF4 = (D_003679A0[index].unk00[0] + D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[0]) * D_003679A0[index].level / 100;
            D_00446DF8 = (D_003679A0[index].unk00[1] + D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[1]) * D_003679A0[index].level / 100;
            D_00446DFC = (D_003679A0[index].unk00[2] + D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[2]) * D_003679A0[index].level / 100;
            D_00446E00 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[3] * D_003679A0[index].level / 100;
            D_00446E04 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[4] * D_003679A0[index].level / 100;
            D_00446E08 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[5] * D_003679A0[index].level / 100;
            D_00446E0C = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[6] * D_003679A0[index].level / 100;
            D_00446E10 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[7] * D_003679A0[index].level / 100;
            D_00446E14 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[8] * D_003679A0[index].level / 100;
            D_00446E1C = (s32)(D_0041B9B8[index].unk68 * 0.03281f * 10.0f);
            D_00446E20 = D_003679A0[index].level;
            D_00446E18 = D_004147F8[CURRENT_ITEM->unk0C].unk3E;
            D_00446E24 = 100;
            D_00446E28 = D_00413FBC[D_005CE840[0]->group];
            if (D_004472C7 == 1) {
                RESET_ALPHAS(D_0044723C, D_00446A40);
            }
            if (index != 0) {
                if (D_00446B50 != 0) {
                    func_0019CDC0(1);
                }
                D_00446B50 = 0;
                DRAW_PARTS(D_0044723C, D_00446A40, 0);
                func_001FDCD0(D_00446C80);
                func_001E44D0(500);
            }
            parts = D_0044723C;
            count = D_00446A40;
            break;
        case 1:
            if (D_00402DA8[CURRENT_ITEM->unk0C].unk102 == 0) {
                magic = &D_00402DA8[index];
                D_00446DF4 = magic->hit_checks[0].strengths.unk00[0];
                D_00446DF8 = magic->hit_checks[0].strengths.unk00[1];
                D_00446DFC = magic->hit_checks[0].strengths.unk00[2];
                D_00446E00 = magic->hit_checks[0].strengths.unk00[3];
                D_00446E04 = magic->hit_checks[0].strengths.unk00[4];
                D_00446E08 = magic->hit_checks[0].strengths.unk00[5];
                D_00446E0C = magic->hit_checks[0].strengths.unk00[6];
                D_00446E10 = magic->hit_checks[0].strengths.unk00[7];
                D_00446E14 = magic->hit_checks[0].strengths.unk00[8];
                D_00446E1C = magic->unkFC;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00447244, D_00446A48);
                }
                if (index != 0) {
                    if (D_00446B50 != 0) {
                        func_0019CDC0(1);
                    }
                    D_00446B50 = 0;
                    DRAW_PARTS(D_00447244, D_00446A48, (index - 1) / 15);
                    func_001FDCD0(D_00446C80);
                    func_001E44D0(900);
                }
                parts = D_00447244;
                count = D_00446A48;
            } else {
                D_00446E1C = D_00402DA8[index].unkFC;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00446C6C, D_00446AC0);
                }
                if (index != 0) {
                    if (D_00446B50 != 0) {
                        func_0019CDC0(1);
                    }
                    D_00446B50 = 0;
                    DRAW_PARTS(D_00446C6C, D_00446AC0, (index + 1) / 15);
                    func_001FDCD0(D_00446C80);
                    func_001E44D0(900);
                }
                parts = D_00446C6C;
                count = D_00446AC0;
            }
            break;
        case 11:
            if (D_004472C7 == 1) {
                RESET_ALPHAS(D_0044726C, D_00446A80);
            }
            if (index != 0) {
                if (D_00446B50 != 0) {
                    func_0019CDC0(1);
                }
                D_00446B50 = 0;
                for (i = 0; i < D_00446A80; i++) {
                    SET_BLEND(D_0044726C[i].unk42);
                    func_00207478((int)&D_0044726C[i], 1002, 1, (int)D_00425910[i]);
                    SET_BLEND(0);
                }
                func_001FDCD0(D_00446C80);
                func_001E44D0(500);
            }
            parts = D_0044726C;
            count = D_00446A80;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            D_00446DF4 = (D_003682F0[index].unk00[0] + D_0041E118[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + D_0041E118[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + D_0041E118[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = D_0041E118[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = D_0041E118[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = D_0041E118[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = D_0041E118[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = D_0041E118[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = D_0041E118[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E18 = D_004147F8[CURRENT_ITEM->unk0C].unk3E;
            D_00446E1C = D_00413FBC[D_005CE840[0]->group];
            D_00446E20 = D_003682F0[index].level;
            D_00446E24 = 100;
            if (index != 0 && index != 30 && index != 55 && index != 80 && index != 105) {
                if (D_00446B50 != 0) {
                    func_0019CDC0(1);
                }
                D_00446B50 = 0;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00447240, D_00446A44);
                }
                DRAW_PARTS(D_00447240, D_00446A44, D_00446E1C);
                func_001FDCD0(D_00446C80);
                func_001E44D0(500);
            }
            parts = D_00447240;
            count = D_00446A44;
            break;
        case 7:
        case 8:
        case 9:
            D_00446DF4 = (D_003682F0[index].unk00[0] + D_0041E118[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + D_0041E118[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + D_0041E118[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = D_0041E118[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = D_0041E118[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = D_0041E118[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = D_0041E118[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = D_0041E118[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = D_0041E118[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E18 = D_004147F8[CURRENT_ITEM->unk0C].unk3E;
            D_00446E1C = D_00413FBC[D_005CE840[0]->group];
            if (index != 130 && index != 140 && index != 150) {
                if (D_00446B50 != 0) {
                    func_0019CDC0(1);
                }
                D_00446B50 = 0;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00447240, D_00446A44); // match: the armour panel's list; the parts drawn and faded in this case are D_00447268's
                }
                DRAW_PARTS(D_00447268, D_00446A7C, D_00446E1C);
                func_001FDCD0(D_00446C80);
                func_001E44D0(500);
            }
            parts = D_00447268;
            count = D_00446A7C;
            break;
        }
        switch (D_004472C7) {
        case 0:
            D_00425910[255] += 12.8f;
            if (D_00425910[255] > 128.0f) {
                D_00425910[255] = 128.0f;
            }
            D_00446C80.alpha = (s32)D_00425910[255];
            for (i = 0; i < count; i++) {
                D_00425910[i] += parts[i].alpha / 10.0f;
                if (D_00425910[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        alpha = parts[i].alpha;
                        D_00425910[i] = alpha;
                    }
                    D_004472C7 = 1;
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
                D_00425910[i] -= parts[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    D_00446AD4 = 0;
                    D_004472C7 = 0;
                    break;
                }
            }
            break;
        }
    }
    func_001FDCD0(D_00446C78);
    word.raw = 0x0A0A875084106EF0;
    word.f.mid24 = 0xA8750;
    func_001FDB88(word);
    func_001FF428(definition, 0x18);
    {
        EquipGroupBytes glyphs = D_0043FDC0;

        glyph = glyphs.per_group[D_005CE840[0]->group];
        width = func_001FC0C0(glyph + 1) - func_001FC0C0(glyph) - 1;
        offset = (s32)(width * 0.5f * 20.0f);
        sprite.red = D_00446C78.red;
        sprite.green = D_00446C78.green;
        sprite.blue = D_00446C78.blue;
        sprite.alpha = D_00446C78.alpha;
        sprite.kind = 1;
        sprite.glyph = glyph;
        sprite.first.f.lo16 = (1865 << 4) - (offset << 4);
        sprite.first.f.mid16 = 0x7B60;
        sprite.first.f.mid24 = 0xA8750;
        sprite.second.f.lo16 = (width * 20 << 4) + (1865 << 4) - (offset << 4);
        sprite.second.f.mid16 = 0x7C00;
        sprite.second.f.mid24 = 0xA8750;
        func_001FDCD0(D_00446C78);
        ((int (*)(KindObject *))func_001FC0D8)((KindObject *)&sprite); // match: the result register stays taken
    }
    if (D_00446AD5 != 0) {
        func_00207320(0x47, 0x30000);
        word.raw = 0x000A87507AF07D00;
        for (i = 0; i < D_00446A80; i++) {
            SET_BLEND(D_0044726C[i].unk42);
            func_00207478((int)&D_0044726C[i], D_0044726C[i].unk3E + 1000, 0, (int)D_00425910[i]);
            SET_BLEND(0);
        }
        switch (D_004472C7) {
        case 0:
            D_00425910[255] += 12.8f;
            if (D_00425910[255] > 128.0f) {
                D_00425910[255] = 128.0f;
            }
            D_00446C80.alpha = (s32)D_00425910[255];
            for (i = 0; i < D_00446A80; i++) {
                D_00425910[i] += D_0044726C[i].alpha / 10.0f;
                if (D_00425910[i] > D_0044726C[i].alpha) {
                    for (i = 0; i < D_00446A80; i++) {
                        alpha = D_0044726C[i].alpha;
                        D_00425910[i] = alpha;
                    }
                    D_004472C7 = 1;
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
            for (i = 0; i < D_00446A80; i++) {
                D_00425910[i] -= D_0044726C[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    D_004472C7 = 0;
                    D_00446AD5 = 0;
                    break;
                }
            }
            break;
        }
        for (i = 0; i < 11; i++) {
            if (i == 1) {
                listed = &D_00402DA8[(*D_00420D60[1])->unk0C];
            } else if (i == 10) {
                if ((*D_00420D60[10])->group == 1) {
                    listed = &D_00402DA8[(*D_00420D60[10])->unk0C];
                } else {
                    listed = &D_004147F8[(*D_00420D60[10])->unk0C];
                }
            } else {
                listed = &D_004147F8[(*D_00420D60[i])->unk0C];
            }
            func_001FDB88(word);
            func_001FDCD0(D_00446C80);
            ((int (*)(void *, s32))func_001FDD28)(listed, 0x1A); // match: the result register stays taken
            word.f.mid16 += 0xD0;
        }
    }
    if ((D_00446DE8 & 0x4000) && D_004472CE != 0 && D_004472DA != 0) {
        EquipGroupBytes unused_bytes = D_0043FDD0;
        f32 unused_pair[32];

        if (*D_00420D60[D_005CE840[0]->group] != *D_00420D90[D_005CE840[0]->group]) {
            func_0019CDC0(0);
            if (D_0041B9B8[D_00413F80.unk3C].unk63 == 2 && D_005CE840[0]->group == 0
                && D_0041B9B8[index].unk63 != 2) {
*D_00420D60[2] = D_00423650;
D_00413F80.equipment[0] = 0;
D_0042A8F0[20].part = NULL;
            }
            (&D_00413F80.unk3C)[D_005CE840[0]->group] = index;
            *D_00420D60[D_005CE840[0]->group] = CURRENT_ITEM;
            if (D_0041B9B8[D_00413F80.unk3C].unk63 != 0 && D_005CE840[0]->group == 0) {
*D_00420D60[2] = D_00423650;
D_00413F80.equipment[0] = 0;
D_0042A8F0[20].part = NULL;
            }
            if (D_0041B9B8[D_00413F80.unk3C].unk63 != 0 && D_005CE840[0]->group == 2) {
*D_00420D60[0] = D_004231F0;
D_00413F80.unk3C = 0;
D_0042A8F0[15].part = NULL;
            }
            if (D_005CE840[0]->group == 10) {
                if (CURRENT_ITEM->unk0C != 0) {
                    switch (CURRENT_ITEM->group) {
                    case 11:
                        D_00413F80.unk48 = 1;
                        break;
                    case 1:
                        D_00413F80.unk48 = 2;
                        break;
                    default:
                        D_00413F80.unk48 = 0;
                        break;
                    }
                } else {
                    D_00413F80.unk48 = 0;
                }
            }
            func_001BA310();
            if (D_00413F80.unk58 < D_00413F80.unkB4) {
                D_00413F80.unkB4 = D_00413F80.unk58;
            }
            if (D_00413F80.unkB8 > D_00413F80.unk5C) {
                D_00413F80.unkB8 = D_00413F80.unk5C;
            }
            if (CURRENT_ITEM->group != 1 && CURRENT_ITEM->group != 11) {
                func_001D4840();
                D_00420DC8[D_005CE840[0]->group]->unk20 = 0;
                if (D_005CED40.unk10 != NULL) {
                    D_00420DC8[D_005CE840[0]->group]->next = D_005CED40.unk10->next;
                    D_005CED40.unk10->next = D_00420DC8[D_005CE840[0]->group];
                } else {
                    D_005CED40.unk10 = D_00420DC8[D_005CE840[0]->group];
                }
                if (D_00446C90 != NULL) {
                    D_005CED40.next = D_00446C90->next;
                    D_00446C90->next = &D_005CED40;
                } else {
                    D_00446C90 = &D_005CED40;
                }
                D_004472F0 = 1;
            }
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472CE != 0 && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472C6 = 2;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472C6 == 2 && D_00446C90 == NULL) {
        D_004472C6 = 3;
    }
    switch (D_004472C6) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A3C; i++) {
            D_00425510[i] += D_00447238[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447238[i].alpha) {
                for (i = 0; i < D_00446A3C; i++) {
                    alpha = D_00447238[i].alpha;
                    D_00425510[i] = alpha;
                }
                D_004472C6 = 1;
                D_004472DA = 1;
                return;
            }
        }
        break;
    case 3:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[254] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00446C88.alpha = (s32)D_00425510[254];
        for (i = 0; i < D_00446A3C; i++) {
            D_00425510[i] -= D_00447238[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446B20 = func_001D7C30;
                ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                func_00203770();
                D_00446C80.alpha = 0;
                D_00446C80.red = D_00446C80.green = D_00446C80.blue = 0x80;
                D_004472C6 = 0;
                D_004472C7 = 0;
                D_00446AD4 = 0;
                D_00446AD5 = 0;
                D_004472F0 = 0;
                func_001D2680();
                return;
            }
        }
        break;
    }
    }
}
