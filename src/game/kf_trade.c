/* Three modal menus that share one source file: the trade screens (Trade_), the smith's screens (Smith_) and the
   save screen (SaveScreen_). Each has a function that runs it until it is left (_Run): every frame that grabs the
   frame buffer and calls the current step, and the steps call the handler of the list or question on screen. */
#include "kf_trade.h"
#include "kf_menu.h"
#include "kf_player.h"
#include "kf_pad.h"
#include "kf_packet.h"
#include <eeregs.h>
#include "kf_state.h"
#include "kf_load.h"
#include "kf_texture.h"
#include "kf_message.h"
#include "kf_link.h"
#include "common.h"
#include "kf_game.h"
#include "kf_gs.h"
#include "kf_save_card.h"
#include "kf_menu_screens.h"

extern u8 item_counts[380];
extern LevelDef item_name_defs[380];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern MenuItem D_004210D0[150];
extern MenuItem D_00421A30[380];
extern MenuItem D_004231F0[70];
extern MenuItem D_00423650[30];
extern MenuItem D_00423830[25];
extern MenuItem D_004239C0[25];
extern MenuItem D_00423B50[25];
extern MenuItem D_00423CE0[25];
extern MenuItem D_00423E70[10];
extern MenuItem D_00423F10[10];
extern MenuItem D_00423FB0[10];
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern MenuItem *D_005CF360[4];
extern MenuItem *D_005CF370[4];
extern MenuItem *D_005CF380[4];
extern MenuItem *D_00446C1C;
extern MenuItem *D_00446C20;
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C34;
extern s32 D_00446C38;
/* Appends node to the ring being built; the seventh one becomes the page mark. */
#define LINK_ITEM(node, page_mark)          \
    count++;                                \
    if (count == 7) {                       \
        (page_mark)[0] = (node);            \
    }                                       \
    if (last != NULL) {                     \
        (node)->next = last;                \
        last->prev = (node);                \
    } else {                                \
        first = (node);                     \
    }                                       \
    last = (node);
/* Lists the owned, unlocked items of one group that the table gives a value for and whose record is not in use. */
#define LINK_OWNED_GROUP(nodes, records, record_base, item_base, item_count)                                       \
    for (i = 1; i < (item_count); i++) {                                                                           \
        if (item_counts[(item_base) + i] != 0 && !(item_name_defs[(item_base) + i].state & 2) &&                        \
            values.values[table][(item_base) + i].give_value != 0 && (records)[(record_base) + i].unk18 == 0) {    \
            LINK_ITEM(&(nodes)[i], D_00426130)                                                                     \
        }                                                                                                          \
    }
int sceGsSyncPath(int, int);
int sceGsSyncV(int);
void Vram_Clear(int, int);
void Sound_MutePositional(void);
void Sound_UnmutePositional(void);
void Sound_UpdateTagged(void);
void func_001B5460(void);
s16 MenuPad_Update(void);
void MenuHeap_FreeToMark(void);
extern s8 D_00447318;
extern s8 D_00447310;
extern s8 D_00446C94;
extern s32 D_0044731C;
extern s32 D_00447320;
extern s32 D_00447328;
extern s32 D_00447304;
extern u16 D_00446DE8;
extern s32 D_00446CA8;
extern u64 D_004261A0[47];             /* the frame register of the first buffer ([0]) and of the second ([46]) */
extern int D_00446F78;
extern int D_00446CA4;
extern HandlerContext D_005CF330;
extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern f32 D_00425D10[0x100];
extern LoadedBlockRef D_0042A2A0[];
extern ColorBytes D_00446C78;
extern ColorBytes D_00446C88;
extern s8 D_00446C08;
extern s8 D_00446C09;
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern s32 D_00446C48;
extern void (*D_00446CA0)(HandlerContext *);
extern s32 D_00446DEC;
extern s32 D_00446F38;
extern s32 D_00446F68;
void *memset(void *, int, u32);
void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_00109E88(void);
void func_0010B238(TextureTableHeader *);
s32 MenuArena_Init(s32 value);
void MenuTexture_Upload(void *, LoadedBlockRef *);
extern LinkNode *D_00446C90;
extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode D_00426560;
extern LinkNode D_004265B0;
extern u32 D_00446A74;
extern SpritePacket *D_00446BFC;
extern s8 D_00447311;
s32 Sound_PlayMenu(s32);
int func_001E5210(u8 mode);
void MenuDraw_Bar(MenuCursorBar *);
void MenuSprite_Draw(int, int, int, int);
/* match: the part number is passed sign-extended and no result is kept (the definition takes a u8 and returns int) */
#define SELECT_PART(part) ((void (*)(s32))func_001E5210)(part)
extern LevelRecordDef worn_defs[];
extern PlayerItemDef weapon_defs[];
extern u8 D_00413FBC[];
extern TradeValue D_00426620[4];
extern TradeValue D_00426630[4];
extern TradeValue D_00426640[4];
extern f32 D_004289F0[4];
extern f32 D_0042B7D0[16];
extern u32 D_00446A78;
extern u32 D_00446A94;
extern u32 D_00446A98;
extern u32 D_00446A9C;
extern SpritePacket *D_00446C00;
extern SpritePacket *D_00446C0C;
extern SpritePacket *D_00446C10;
extern SpritePacket *D_00446C14;
extern u8 D_00446C9C;
extern s32 D_00446CAC;
extern f32 D_00446CB0;
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
extern s8 D_00447312;
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
int Menu_DrawItemModel(f32 *); // match: an int result keeps $2 for the load after the call
int func_001E4FE0(s32, u32);
s32 MenuText_SetColor(ColorBytes bytes);
s32 MenuText_DrawWrapped(u16 *, s32);
s32 MenuText_DrawLine(u16 *text, s32 limit);
s32 MenuText_DrawWordWrappedLong(u16 *, s32);
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
        MenuSprite_Draw((int)&(packets)[i], (packets)[i].unk3E + 1000, (param), (int)D_00425910[i]);      \
        SELECT_PART(0);                                                                                 \
    }
extern PlayerState D_00413F80;
extern f32 D_00428A00[4];
extern u32 D_00446AB4;
extern SpritePacket *D_00446C60;
extern s32 D_00446CB4;
extern f32 D_00446CB8;
extern s32 D_00446E30;
extern s32 D_00446E44;
extern s32 D_00446E48;
extern s32 D_00446E4C;
extern s32 D_00446E50;
s32 MenuText_DrawLineCentered(u16 *, s32);
extern f32 D_00428A10[4];
extern f32 D_00446CBC;
/* Starts every packet of a list at its full alpha. match: the alpha is read as an unsigned bit-field */
#define FILL_FADE_2(levels, packets, count)                               \
    for (i = 0; i < (count); i++) {                                     \
        (levels)[i] = SPRITE_PACKET_COLOR(&(packets)[i])->alpha;        \
    }
extern f32 D_00428A20[4];
extern f32 D_00446CC0;
void func_001E54A8(s8);
int sprintf(char *, char *, ...);
extern char D_00426498[];           /* the names of the five link nodes at D_00426470 */
extern char D_004264E8[];
extern char D_00426538[];
extern char D_00426588[];
extern char D_004265D8[];
#define PAGE_ROWS 7
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 func_001ED738_ScrollUp(void) {
    D_005CF360[0] = D_005CF360[0]->prev;
    D_005CF370[0] = D_005CF370[0]->prev;
    D_005CF380[0] = D_005CF360[0];
    return D_005CF360[0] != D_00446C44;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 func_001ED738_ScrollDown(void) {
    D_005CF360[0] = D_005CF360[0]->next;
    D_005CF370[0] = D_005CF370[0]->next;
    D_005CF380[0] = D_005CF360[0];
    return D_005CF360[0] != D_00446C40;
}
extern MenuItem *D_005CF390[4];
extern MenuItem *D_005CF3A0[4];
extern MenuItem *D_005CF3B0[];
extern ClockTime D_00446A28;
void func_001D20E0(void);
int func_001D2258(ClockTime, ClockTime);
/* Adds a node to the end of the ring being built; `seventh` remembers the seventh one added. */
#define APPEND(node, seventh) \
    count++; \
    if (count == 7) { \
        seventh = (node); \
    } \
    if (last != NULL) { \
        (node)->next = last; \
        last->prev = (node); \
    } else { \
        first = (node); \
    } \
    last = (node);
/* The owned, usable items of one run of definitions whose record is idle and below level 100. */
#define APPEND_UPGRADABLE(nodes, records, base, first_item, limit) \
    for (i = 1; i < limit; i++) { \
        if (item_counts[first_item + i] != 0 && !(item_name_defs[first_item + i].state & 2) && \
            records[base + i].unk18 == 0 && records[base + i].level < 100) { \
            APPEND(&nodes[i], D_00426130[0]) \
        } \
    }
/* The owned, usable items of one run of definitions whose record is idle. */
#define APPEND_IDLE(nodes, records, base, first_item, limit) \
    for (i = 1; i < limit; i++) { \
        if (item_counts[first_item + i] != 0 && !(item_name_defs[first_item + i].state & 2) && \
            records[base + i].unk18 == 0) { \
            APPEND(&nodes[i], D_00426130[0]) \
        } \
    }
extern s8 D_00447313;
extern s8 D_00447314;
extern s8 D_00447315;
extern s32 D_00447308;
extern s32 D_00446CCC;
extern void (*D_00446CC4)(HandlerContext *);
extern s32 D_00446CC8;
extern HandlerContext D_005CF340;
extern u32 D_00446AA0;              /* how many parts the screen has */
extern u32 D_00446AB8;
extern s32 D_00446AD4; // match: this file reads and writes it as a word
extern SpritePacket *D_00446C4C;
extern SpritePacket *D_00446C64;
extern s8 D_00446CD0;               /* nonzero once the cursor is on the three lines */
extern s8 D_00447316;               /* 0: fading in, 1: open, 2 to 4: fading out to a list, 5: fading out to leave */
void MenuSprite_DrawPlain(int, int, int);
extern f32 D_00428A30[4];
extern u32 D_00446AA4;              /* how many parts the screen has */
extern SpritePacket *D_00446C50;
extern s32 D_00446CD4;
extern f32 D_00446CD8;
extern s32 D_00446E34;
extern s8 D_00447317;               /* the same for the selected item's panel */
/* match: the two level lookups are inline functions taking the record index, so the subtraction is not folded into
   the table address; each caller picks the table itself and uses the level inside its own branch. */
static inline u8 GetWeaponLevel(s32 index) {
    return D_003679A0[index].level;
}
static inline u8 GetItemLevel(s32 index) {
    return D_003682F0[index].level;
}
static inline void SetCorner(GsPackedWord *corner, s32 left, s32 top, s32 depth) {
    corner->f.lo16 = left;
    corner->f.mid16 = top;
    corner->f.mid24 = depth;
}
extern f32 D_00428A40[4];
extern s32 D_00446CDC;
extern f32 D_00446CE0;
extern s32 D_00446E38;
/* match: the records are written through these two for the same reason. */
static inline LevelRecord *GetWeaponRecord(s32 index) {
    return &D_003679A0[index];
}
static inline LevelRecord *GetItemRecord(s32 index) {
    return &D_003682F0[index];
}
extern f32 D_00428A50[4];
extern s32 D_00446CE4;
extern f32 D_00446CE8;
extern f32 D_00428A60[4];
extern u32 D_00446AA8;              /* how many parts the screen has */
extern SpritePacket *D_00446C54;
extern f32 D_00446CEC;
extern f32 D_00428A70[4];
extern s32 D_00446CF0;
extern f32 D_00446CF4;
/* match: inline like the level lookups, so the record index is not folded into the table address */
static inline void ClearWeaponMark(s32 index) {
    D_003679A0[index].unk18 = 0;
}
static inline void ClearItemMark(s32 index) {
    D_003682F0[index].unk18 = 0;
}
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 func_001F67C8_ScrollUp(void) {
    D_005CF390[0] = D_005CF390[0]->prev;
    D_005CF3A0[0] = D_005CF3A0[0]->prev;
    D_005CF3B0[0] = D_005CF390[0];
    return D_005CF390[0] != D_00446C44;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 func_001F67C8_ScrollDown(void) {
    D_005CF390[0] = D_005CF390[0]->next;
    D_005CF3A0[0] = D_005CF3A0[0]->next;
    D_005CF3B0[0] = D_005CF390[0];
    return D_005CF390[0] != D_00446C40;
}
extern s32 D_00446D00;
extern int D_00446CFC;
extern int D_0044730C;
extern u8 D_00446C7B;
extern u8 D_00446C8B;
extern HandlerContext D_005CF350;
extern int (*D_00446CF8)();
extern float D_00446C04;
s32 SaveCard_Open(void);
s32 SaveCard_StartCheck(s8);
extern s32 D_00426600[3];
extern s32 D_00426610[3];
extern u32 D_00446A60;
extern SpritePacket *D_00446BF4;
extern s8 D_00446C70;
s32 SaveCard_Update(void);
s32 SaveCard_StartReadList(s32, char);
s32 SaveCard_StartFormat(s32);
void SaveCard_Close(void);
s32 SaveCard_CheckBusy(void);
s32 SaveCard_HasCard(s32);
s32 SaveCard_HasRoomForFolder(s32);
s32 SaveCard_GetSaveMask(void);
s8 SaveCard_IsCheckRunning(void);
s8 SaveCard_IsPresent(s32);
s8 SaveCard_IsFormatted(s32);
s8 SaveCard_HasSaveFolder(s32);
s8 SaveCard_IsDamaged(s32);
void SaveCard_Cancel(s32);
s32 SaveCard_HasAnySave(void);
extern u8 D_0036A130[];
extern u32 D_00446A64;
extern u32 D_00446AAC;
extern SpritePacket *D_00446BF8;
extern SpritePacket *D_00446C58;
extern s32 D_00447324;
extern s32 D_00446E18;
extern s32 D_00446E40;
extern s32 D_00446E54;
extern s32 D_00446E58;
extern s32 D_00446E5C;
extern s32 D_00446E60;
extern s32 D_00446E64;
extern s32 D_00446E68;
extern s32 D_00446E6C;
extern s32 D_00446E70;
extern s32 D_00446E74;
extern s32 D_00446E78;
extern s32 D_00446E7C;
void func_00103C18(void);
s32 SaveCard_StartSave(s32, s32, s32 *);
s8 SaveCard_GetSaveState(s32);
s32 SaveCard_HasRoomFor(s32, s32, s32);
s32 SaveCard_GetFreeClusters(s32);
// match: called as a void function, so the result register is free at once
#define SET_PART_MODE(mode) ((void (*)(u8))func_001E5210)(mode)
// match: this file hands func_001E5210 a signed byte
#define SET_PART_MODE_OF(packet) ((void (*)(s8))func_001E5210)((s8)(packet)->unk42)

static s32 Trade_CopyFrameToTexture(void);
static int Trade_RunStep(int value);
static s32 Trade_Start(HandlerContext *context);
static int Trade_RunHandler(HandlerContext *context);
static int Trade_CutModelChain(void);
static void Trade_ChainModels(void);
static f32 Trade_PulseValue(s32 low, s32 high, f32 value);
static void Trade_UpdateSelectScreen(HandlerContext *context);
static void Trade_UpdateStockList(HandlerContext *context);
static void Trade_UpdateTakeAmount(HandlerContext *context);
static void Trade_UpdateOwnedList(HandlerContext *context);
static void Trade_UpdateGiveAmount(HandlerContext *context);
static void Trade_SetModelNames(void);
static s32 Trade_MoveListCursor(HandlerContext *context);
static void Smith_BuildItemRings(s32 mode);
static s32 Smith_Start(void);
static s32 Smith_CopyFrameToTexture(void);
static int Smith_RunStep(int value);
static int Smith_RunHandler(HandlerContext *context);
static int Smith_CutModelChain(void);
static void Smith_ChainModels(void);
static void Smith_UpdateSelectScreen(HandlerContext *context);
static void Smith_SetModelNames(void);
static void Smith_UpdateUpgradeList(HandlerContext *context);
static void Smith_UpdateUpgradeConfirm(HandlerContext *context);
static void Smith_UpdateListWithConfirm(HandlerContext *context);
static void Smith_UpdateItemScreen(HandlerContext *context);
static void Smith_UpdateCollectList(HandlerContext *context);
static s32 Smith_MoveListCursor(HandlerContext *context);
static s32 SaveScreen_DrawBackground(void);
static int SaveScreen_RunStep(int value);
static int SaveScreen_Start(HandlerContext *context);
static int SaveScreen_RunHandler(void);
static int SaveScreen_CutModelChain(void);
static void SaveScreen_UpdateCardSelect(void);
static void SaveScreen_UpdateSaveList(void);

extern TradeValue D_00426650[];
extern TradeValue D_00426C40[];
extern TradeValue D_00427230[];
extern TradeValue D_00427820[];
extern TradeValue D_00427E10[];
extern TradeValue D_00428400[];
extern u8 D_003699B0[];
extern u8 D_00369B30[];
extern u8 D_00369CB0[];
extern u8 D_00369E30[];
extern u8 D_00369FB0[];

// match: only a named section keeps a table of eight bytes out of small data; retail has these in .rodata
// and reaches them without the global pointer
#define IN_RODATA __attribute__((section(".rodata")))

/* the functions of the trade screens: the two steps of Trade_Run's loop, the steps of the screen itself, and
   the handlers of its two lists */
static int (*const trade_run_steps[2])(s32) IN_RODATA = {
    (int (*)(s32))Trade_CopyFrameToTexture,
    Trade_RunStep,
};
static int (*const trade_steps[4])(HandlerContext *) IN_RODATA = {
    Trade_Start,
    Trade_RunHandler,
    (int (*)(HandlerContext *))Trade_CutModelChain,
    NULL,
};
static void (*const trade_list_handlers[2])(HandlerContext *) IN_RODATA = {
    Trade_UpdateStockList,
    Trade_UpdateOwnedList,
};
static const TradeValueTables trade_value_tables = { { D_00426650, D_00426C40, D_00427230, D_00427820, D_00427E10, D_00428400 } };
static const TradeStockTables trade_stock_tables = { { D_003699B0, D_00369B30, D_00369CB0, D_00369E30, D_00369FB0 } };
static const TradeGroupBases item_group_bases = { { 150, 0, 220, 220, 220, 220, 220, 220, 220, 220, 0, 0, 0 } };
static const TradeValueSlots trade_value_slots = {
    { &D_00446E64, &D_00446E68, &D_00446E6C, &D_00446E70, &D_00446E74, &D_00446E78, &D_00446E7C }
};

/* Builds the two item rings of trade table `table`: what is owned and may be given, and what is in stock. */
static void Trade_BuildItemRings(s32 table) {
    TradeValueTables values = trade_value_tables;
    TradeStockTables stock = trade_stock_tables;
    MenuItem *last = NULL;
    MenuItem *first = NULL;
    s32 count = 0;
    s32 i;
    u8 *stock_counts;

    D_00426120[0] = NULL;
    D_00426130[0] = NULL;
    D_00446C1C = NULL;
    D_00446C20 = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 150; i++) {
        if (item_counts[i] != 0 && !(item_name_defs[i].state & 2) && values.values[table][i].give_value != 0) {
            LINK_ITEM(&D_004210D0[i], D_00426130)
        }
    }
    LINK_OWNED_GROUP(D_004231F0, D_003679A0, 0, 150, 70)
    LINK_OWNED_GROUP(D_00423650, D_003682F0, 0, 220, 30)
    LINK_OWNED_GROUP(D_00423830, D_003682F0, 30, 250, 25)
    LINK_OWNED_GROUP(D_004239C0, D_003682F0, 55, 275, 25)
    LINK_OWNED_GROUP(D_00423B50, D_003682F0, 80, 300, 25)
    LINK_OWNED_GROUP(D_00423CE0, D_003682F0, 105, 325, 25)
    LINK_OWNED_GROUP(D_00423E70, D_003682F0, 130, 350, 10)
    LINK_OWNED_GROUP(D_00423F10, D_003682F0, 140, 360, 10)
    LINK_OWNED_GROUP(D_00423FB0, D_003682F0, 150, 370, 10)
    D_00446C34 = count;
    if (first != NULL) {
        last->prev = first;
        D_00426110[0] = D_00426120[0] = first;
        first->next = last;
        if (D_00426130[0] == NULL) {
            D_00426130[0] = last->next;
        } else {
            D_00426130[0] = D_00426130[0]->next;
        }
        D_00446C1C = first;
        D_00446C20 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C20 = D_00446C20->next;
        }
    } else {
        D_00426110[0] = NULL;
    }

    D_005CF360[0] = NULL;
    D_005CF370[0] = NULL;
    D_00446C24 = NULL;
    D_00446C28 = NULL;
    stock_counts = stock.counts[table];
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 0x17C; i++) {
        if (stock_counts[i] != 0 && values.values[table][i].take_value != 0 && !(item_name_defs[i].state & 2)) {
            LINK_ITEM(&D_00421A30[i], D_005CF370)
        }
    }
    D_00446C38 = count;
    if (first != NULL) {
        last->prev = first;
        D_005CF380[0] = D_005CF360[0] = first;
        first->next = last;
        if (D_005CF370[0] == NULL) {
            D_005CF370[0] = last->next;
        } else {
            D_005CF370[0] = D_005CF370[0]->next;
        }
        D_00446C24 = first;
        D_00446C28 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C28 = D_00446C28->next;
        }
    } else {
        D_005CF380[0] = NULL;
    }
}

/* Runs the screen loop of the handler D_00446CA8 selects until it returns 0, watching the pad type for changes. */
void Trade_Run(s32 value) {
    D_00447318 = 1;
    D_00447310 = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    Vram_Clear(0x3D00, 0x3F00);
    Sound_MutePositional();
    Sound_UpdateTagged();
    D_00446C94 = 0;
    D_0044731C = D_00447320 = Pad_GetMode(0, 0);
    while (trade_run_steps[D_00446CA8](value) != 0) {
        sceGsSyncPath(0, 0);
        D_00447328 = Menu_EndFrame(1);
        func_001B5460();
        D_0044731C = Pad_GetMode(0, 0);
        if (D_0044731C != D_00447320) {
            D_00447304 = 1;
        }
        D_00447320 = D_0044731C;
        MenuPad_Update();
        if (D_00447304 > 0) {
            D_00447304++;
            if (D_00447304 >= 21) {
                D_00447304 = 0;
            } else {
                D_00446DE8 = 0;
            }
        }
    }
    sceGsSyncPath(0, 0);
    while (sceGsSyncV(0) != 0) {
    }
    Sound_UnmutePositional();
    MenuHeap_FreeToMark();
    D_00446CA8 = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}

/* Copies the shown frame buffer into a work buffer and draws that back into both frame buffers, then steps the screen counter. */
static s32 Trade_CopyFrameToTexture(void) {
    f32 unused[28]; // match: 0x70 bytes of locals nothing uses
    u32 texture_base; /* where the shown buffer is as a texture: 0x1180 when it is the second one */
    u32 *packet;

    packet = MenuDraw_NextPacket();
    texture_base = 0x1180;
    if (D_00446F78 == 0) {
        texture_base = 0;
    }
    packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
    packet[2] = 0xE;
    packet[3] = 0;
    *(u64 *)&packet[4] = (D_004261A0[0] & ~0x1FFL) | 0x192;
    packet[6] = 0x4C;
    packet[7] = 0;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = 0x18;
    packet[11] = 0;
    packet[12] = 0x30000;
    packet[13] = 0;
    packet[14] = 0x47;
    packet[15] = 0;
    packet[16] = 0x2800000;
    packet[17] = 0xE00000;
    packet[18] = 0x40;
    packet[19] = 0;
    *(u64 *)&packet[20] = texture_base | 0x28028000 | 0x640000000L; /* TEX0: 1024 by 512 */
    packet[22] = 0x6;
    packet[23] = 0;
    packet[24] = 0x80808080;
    *(f32 *)&packet[25] = 1.0f;
    packet[26] = 0x1;
    packet[27] = 0;
    packet[28] = 0;
    packet[29] = 0;
    packet[30] = 0x2;
    packet[31] = 0;
    packet[32] = 0;
    packet[33] = 0;
    packet[34] = 0x5;
    packet[35] = 0;
    *(f32 *)&packet[36] = 0.625f;
    *(f32 *)&packet[37] = 0.875f;
    packet[38] = 0x2;
    packet[39] = 0;
    packet[40] = 0xE002800;
    packet[41] = 0;
    packet[42] = 0x5;
    packet[43] = 0;
    packet[44] = 0;
    packet[45] = 0;
    packet[46] = 0x3F;
    packet[47] = 0;
    *(u64 *)&packet[48] = D_004261A0[0];
    packet[50] = 0x4C;
    packet[51] = 0;
    *(u64 *)&packet[52] = 0x66802B240;
    packet[54] = 0x6;
    packet[55] = 0;
    packet[56] = 0;
    packet[57] = 0;
    packet[58] = 0x2;
    packet[59] = 0;
    packet[60] = 0;
    packet[61] = 0;
    packet[62] = 0x5;
    packet[63] = 0;
    *(f32 *)&packet[64] = 0.625f;
    *(f32 *)&packet[65] = 0.4375f;
    packet[66] = 0x2;
    packet[67] = 0;
    packet[68] = 0xE002800;
    packet[69] = 0;
    packet[70] = 0x5;
    packet[71] = 0;
    *(u64 *)&packet[72] = D_004261A0[46];
    packet[74] = 0x4C;
    packet[75] = 0;
    packet[76] = 0;
    packet[77] = 0;
    packet[78] = 0x2;
    packet[79] = 0;
    packet[80] = 0;
    packet[81] = 0;
    packet[82] = 0x5;
    packet[83] = 0;
    *(f32 *)&packet[84] = 0.625f;
    *(f32 *)&packet[85] = 0.4375f;
    packet[86] = 0x2;
    packet[87] = 0;
    packet[88] = 0xE002800;
    packet[89] = 0;
    packet[90] = 0x5;
    packet[91] = 0;
    packet[92] = 0x50000;
    packet[93] = 0;
    packet[94] = 0x47;
    packet[95] = 0;
    packet[96] = 0;
    packet[97] = 0;
    packet[98] = 0x3F;
    packet[99] = 0;
    packet[100] = 0x44;
    packet[101] = 0;
    packet[102] = 0x42;
    packet[103] = 0;
    packet[0] = 0x8019;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 26;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    sceGsSyncPath(0, 0);
    D_00446CA8++;
    return 1;
}

/* Calls the handler selected by D_00446CA4 with the context at D_005CF330, storing value in it first. */
static int Trade_RunStep(int value) {
    int (*handler)(HandlerContext *);

    Menu_StepModelLoads(0);
    handler = trade_steps[D_00446CA4];
    D_005CF330.arg = value;
    if (handler != NULL) {
        return handler(&D_005CF330);
    }
    return 1;
}

/* Start-up step of the trade screens: loads the textures and blocks, builds the item lists of table context->arg and
   picks the screen's handler (table 4 goes straight to the handler in trade_list_handlers with the five link nodes set up). */
static s32 Trade_Start(HandlerContext *context) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;

    func_001095E0();
    func_001095D0();
    func_0010B238((TextureTableHeader *)Message_GetFontTexture(D_00446F38));
    func_00109E88();
    func_001095D8();
    func_001096E8();
    ((s32 (*)(void))func_00109800)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < D_00446DEC; i++) {
        block = &D_0042A2A0[i];
        MenuTexture_Upload(block->data, block);
    }
    MenuArena_Init(D_00446F68);
    packet = MenuDraw_NextPacket();
    packet[0] = 0x8001;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 0x3F;
    packet[7] = 0;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 2;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    Trade_BuildItemRings(context->arg);
    Menu_InitLoadBuffers();
    D_00446C08 = 0;
    D_00446C09 = 0;
    if (context->arg != 4) {
        D_00446CA0 = Trade_UpdateSelectScreen;
    } else {
        D_00446CA0 = trade_list_handlers[0];
        D_00446C40 = D_00446C24;
        D_00446C44 = D_00446C28;
        D_00446C48 = D_00446C38;
        ((void (*)(s32))Trade_ChainModels)(0); // match: the callee takes nothing
        context->unk04 = 0;
    }
    memset(D_00425510, 0, 0x400);
    memset(D_00425910, 0, 0x400);
    memset(D_00425D10, 0, 0x400);
    D_00446C78.blue = 0x80;
    D_00446C78.green = 0x80;
    D_00446C78.red = 0x80;
    D_00446C78.alpha = 0;
    D_00446C88.blue = 0x80;
    D_00446C88.green = 0x80;
    D_00446C88.red = 0x80;
    D_00446C88.alpha = 0;
    D_00446CA4++;
    return TRUE;
}

/* Runs the handler in D_00446CA0 with the context this step was given. */
static int Trade_RunHandler(HandlerContext *context) {
    D_00446CA0(context);
    return 1;
}

/* Cuts the chain after the first link node (and after the node it points at); with no chain, resets D_00446CA4. */
static int Trade_CutModelChain(void) {
    LinkNode *node = D_00446C90;
    LinkNode *other;

    if (node == NULL) {
        D_00446CA4 = 0;
        return 0;
    }
    other = node->unk10;
    node->next = NULL;
    if (other != NULL) {
        other->next = NULL;
    }
    return 1;
}

/* Rebuilds the five link nodes for the selected item and chains them, when there is one. */
static void Trade_ChainModels(void) {
    if (D_005CF380[0] != NULL) {
        Trade_SetModelNames();
        Menu_TakeLoadBuffer(&D_00426470);
        Menu_TakeLoadBuffer(&D_004264C0);
        Menu_TakeLoadBuffer(&D_00426510);
        Menu_TakeLoadBuffer(&D_00426560);
        Menu_TakeLoadBuffer(&D_004265B0);
        D_00426470.next = &D_004264C0;
        D_00426470.callback = Menu_StepModelLoad;
        D_00426470.unk20 = 0;
        D_004264C0.next = &D_00426510;
        D_004264C0.callback = Menu_StepModelLoad;
        D_004264C0.unk20 = 0;
        D_004264C0.unk1C = 0;
        D_00426510.next = &D_00426560;
        D_00426510.callback = Menu_StepModelLoad;
        D_00426510.unk20 = 0;
        D_00426510.unk1C = 0;
        D_00426560.next = &D_004265B0;
        D_00426560.callback = Menu_StepModelLoad;
        D_00426560.unk20 = 0;
        D_00426560.unk1C = 0;
        D_004265B0.next = NULL;
        D_004265B0.callback = Menu_StepModelLoad;
        D_004265B0.unk20 = 0;
        D_00426470.unk1C = D_004265B0.unk1C = 0;
        D_00446C90 = &D_00426470;
    }
}

/* Moves value back and forth between low and high, one sixtieth of the span per call; D_00446C09 is the direction. */
static f32 Trade_PulseValue(s32 low, s32 high, f32 value) {
    if (D_00446C09 == 0) {
        value += (f32)(high - low) / 60.0f;
        if (value > high) {
            value = high;
            D_00446C09 = 1;
        }
    } else {
        value -= (f32)(high - low) / 60.0f;
        if (value < low) {
            D_00446C09 = 0;
            value = low;
        }
    }
    return value;
}

/* The first trade screen, where one of two rows is picked (pad 0x10 / 0x40): draws the screen's parts with the
   cursor bar on the chosen row, fades them in and out, and hands over to the screen of the row taken. */
static void Trade_UpdateSelectScreen(HandlerContext *context) {
    MenuCursorBar bar;
    u32 i;
    u32 full_alpha;
    s32 bar_drawn;
    s32 kind;

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x10) {
            Sound_PlayMenu(1);
            context->unk04--;
        } else if (D_00446DE8 & 0x40) {
            Sound_PlayMenu(1);
            context->unk04++;
        }
    }
    context->unk04 = (context->unk04 + 2) % 2;

    bar_drawn = 0;
    for (i = 0; i < D_00446A74; i++) {
        if (bar_drawn == 0 && D_00446BFC[i].unk43 == 1) {
            bar.first.f.lo16 = 0x76C0;
            bar.first.f.mid16 = context->unk04 * 0xE0 + 0x7F50;
            bar.first.f.mid24 = 900000;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x12C0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xD0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar_drawn = 1;
            if (D_00447311 == 1) {
                D_00425510[0xFD] = Trade_PulseValue(0x10, 0x20, D_00425510[0xFD]);
            } else {
                D_00425510[0xFD] = 0.0f;
            }
            bar.alpha = (s32)D_00425510[0xFD];
            MenuDraw_Bar(&bar);
        }
        kind = D_00446BFC[i].unk3E;
        SELECT_PART((s8)D_00446BFC[i].unk42);
        MenuSprite_Draw((int)&D_00446BFC[i], kind + 1000, 1, (int)D_00425510[i]);
        SELECT_PART(0);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            if (context->unk04 == 0) {
                if (D_005CF380[0] != NULL) {
                    Sound_PlayMenu(0);
                    D_00447311 = 2;
                    D_00447318 = 1;
                } else {
                    Sound_PlayMenu(3);
                }
            } else if (context->unk04 == 1) {
                if (D_00426110[0] != NULL) {
                    Sound_PlayMenu(0);
                    D_00447318 = 1;
                    D_00447311 = 3;
                } else {
                    Sound_PlayMenu(3);
                }
            }
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
            D_00447311 = 4;
            D_00447318 = 1;
        }
    }

    switch (D_00447311) {
    case 0:
        D_00425510[0xFD] = 16.0f;
        for (i = 0; i < D_00446A74; i++) {
            D_00425510[i] += (f32)D_00446BFC[i].alpha / 10.0f;
            if ((f32)D_00446BFC[i].alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A74; i++) {
                    full_alpha = D_00446BFC[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00447318 = 0;
                D_00447311 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 4:
        D_00425510[0xFD] -= 1.6f;
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A74; i++) {
            D_00425510[i] -= (f32)D_00446BFC[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446CA0 = trade_list_handlers[context->unk04];
                if (D_00447311 == 2) {
                    D_00446C40 = D_00446C24;
                    D_00446C44 = D_00446C28;
                    D_00446C48 = D_00446C38;
                    ((void (*)(s32))Trade_ChainModels)(0); // match: the callee takes nothing
                } else if (D_00447311 == 3) {
                    D_00446C40 = D_00446C1C;
                    D_00446C44 = D_00446C20;
                    D_00446C48 = D_00446C34;
                    ((void (*)(s32))Menu_ChainItemListModels)(0); // match: the callee takes nothing
                } else {
                    D_00446CA4++;
                }
                D_00447311 = 0;
                context->unk04 = 0;
                break;
            }
        }
        break;
    }
}

/* The trade screen while the stock list is shown: draws it and the chosen item's values, and handles the buttons. */
static void Trade_UpdateStockList(HandlerContext *context) {
    TradeStockTables stock = trade_stock_tables; // match: copied but never read
    TradeValueTables values = trade_value_tables;
    TradeGroupBases bases = item_group_bases;
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
        D_00446CA0 = Trade_UpdateSelectScreen;
        return;
    }
    if (D_00447311 < 2) {
        Trade_MoveListCursor(context);
    }
    record = D_005CF380[0]->unk0C - bases.first_item[D_005CF380[0]->group];
    if (context->arg == 4) {
        switch (item_name_defs[D_005CF380[0]->unk0C].unk30_20 & 3) {
        case 1:
            D_00446E80 = item_counts[ITEM_BONE];
            break;
        case 2:
            D_00446E80 = item_counts[ITEM_SCALE];
            break;
        case 3:
            D_00446E80 = item_counts[ITEM_CLAW];
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
                MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 0, (int)D_00425510[i]);
                break;
            case 1:
                break;
            case 2:
                if (context->arg != 4) {
                    MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 2, (int)D_00425510[i]);
                }
                break;
            case 3:
                if (D_005CF380[0]->group != 12) {
                    MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
                }
                break;
            case 4:
                if (context->arg == 4) {
                    MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 4, (int)D_00425510[i]);
                }
                break;
            }
        } else if (context->arg != 4) {
            MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 1, (int)D_00425510[i]);
        } else {
            MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 5, (int)D_00425510[i]);
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
                D_00425510[0xFD] = Trade_PulseValue(0x10, 0x20, D_00425510[0xFD]);
            }
            bar.alpha = (s32)D_00425510[0xFD];
            MenuDraw_Bar(&bar);
            break;
        }
    }

    if (context->arg == 4) {
        position.raw = 0x000DBBA079E08A30L;
        switch (item_name_defs[D_005CF380[0]->unk0C].unk30_20 & 3) {
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
        MenuText_SetPen(position);
        MenuText_DrawWrapped(label, 10);
    }

    MenuText_SetColor(D_00446C78);
    row_position.raw = 0x0C7A12007C307080L;
    for (row = 0, node = D_005CF360[0]; row < 7; node = node->prev, row++) {
        item = &item_name_defs[node->unk0C];
        MenuText_SetPen(row_position);
        MenuText_DrawLine((u16 *)item, 0x1A);
        row_position.f.mid16 += 0xB0;
        if (D_005CF370[0]->prev == node) {
            break;
        }
    }

    /* the values of the (up to seven) listed items; -1 for the rows left empty */
    {
        // match: the list head is read before the table is copied, and the loop reads it again
        MenuItem *first_node = D_005CF360[0];
        TradeValueSlots slots = trade_value_slots;
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
        Menu_DrawItemModel((f32 *)local_matrix);
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
                Sound_PlayMenu(1);
            }
            D_00446CAC = 0;
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E24 = D_003682F0[record].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_005CF380[0]->unk0C].unk3E;
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
                Sound_PlayMenu(1);
            }
            D_00446CAC = 0;
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E1C = item_name_defs[D_005CF380[0]->unk0C].unk3E;
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
                Sound_PlayMenu(1);
            }
            D_00446CAC = 0;
            D_00446DF4 = ScaleValue(D_003679A0[record].unk00[0] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[0], D_003679A0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003679A0[record].unk00[1] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[1], D_003679A0[record].level, 100);
            D_00446DFC = ScaleValue(D_003679A0[record].unk00[2] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[2], D_003679A0[record].level, 100);
            D_00446E00 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[3], D_003679A0[record].level, 100);
            D_00446E04 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[4], D_003679A0[record].level, 100);
            D_00446E08 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[5], D_003679A0[record].level, 100);
            D_00446E0C = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[6], D_003679A0[record].level, 100);
            D_00446E10 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[7], D_003679A0[record].level, 100);
            D_00446E14 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[8], D_003679A0[record].level, 100);
            D_00446E20 = (s32)(weapon_defs[record].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[record].level;
            D_00446E1C = item_name_defs[D_005CF380[0]->unk0C].unk3E;
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
    texts = Message_GetFontText(D_00446F38);
    text_number = current->unk0C + MSG_ITEM_TEXT_FIRST + 1; // match: a variable, so the offset is added before the index is scaled
    text_offset = ((s32 *)texts)[text_number];
    MenuText_SetColor(D_00446C78);
    position.raw = 0x000A875083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            Sound_PlayMenu(0);
            D_00447311 = 2;
            D_00447312 = 3;
            D_00447318 = 1;
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
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
                    D_00446CA0 = Trade_UpdateTakeAmount;
                    context->unk04 = 1;
                    D_00447310 = 0;
                } else if (D_00447311 == 6) {
                    context->unk04 = 0;
                    D_00446CA0 = Trade_UpdateSelectScreen;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0);
                } else if (D_00447311 == 7) {
                    context->unk04 = 0;
                    D_00446CA4++;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0);
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

/* The trade screen while the amount is chosen: draws the lists and the chosen item's values, and takes or cancels the trade. */
static void Trade_UpdateTakeAmount(HandlerContext *context) {
    TradeStockTables stock = trade_stock_tables;
    TradeValueTables values = trade_value_tables;
    TradeGroupBases bases = item_group_bases;
    f32 local_matrix[4][4];
    f32 view_matrix[4][4];
    MenuCursorBar bar;
    s32 record;
    u32 i;
    u32 full_alpha;
    s32 row;
    s32 number;
    s32 kind;
    MenuItem *node;
    GsPackedWord position;
    GsPackedWord label_position;
    TradeValue *label;
    LevelDef *item;
    LevelDef *chosen;
    s32 bar_drawn;
    SpritePacket *packets;
    u32 count;
    MenuItem *current;
    s32 texts;
    s32 text_offset;
    f32 angle;

    record = D_005CF380[0]->unk0C - bases.first_item[D_005CF380[0]->group];
    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x20) {
            Sound_PlayMenu(1);
            context->unk04++;
        } else if ((D_00446DE8 & 0x80) && D_00447310 == 0) {
            Sound_PlayMenu(1);
            context->unk04--;
        }
    }
    if (context->unk04 + item_counts[D_005CF380[0]->unk0C] >= 100) {
        if (item_counts[D_005CF380[0]->unk0C] != 99) {
            context->unk04 = 1;
        } else {
            context->unk04 = 0;
        }
    }
    if (stock.counts[context->arg][D_005CF380[0]->unk0C] < context->unk04) {
        context->unk04 = 1;
    } else if (context->unk04 <= 0) {
        context->unk04 = stock.counts[context->arg][D_005CF380[0]->unk0C];
        if (context->unk04 + item_counts[D_005CF380[0]->unk0C] >= 100) {
            context->unk04 = 99 - item_counts[D_005CF380[0]->unk0C];
        }
    }
    if (context->arg != 4) {
        D_00446E44 = item_counts[D_005CF380[0]->unk0C];
        D_00446E48 = stock.counts[context->arg][D_005CF380[0]->unk0C];
        D_00446E4C = context->unk04;
        D_00446E50 = context->unk04 * values.values[context->arg][D_005CF380[0]->unk0C].take_value;
    } else {
        D_00446E44 = item_counts[D_005CF380[0]->unk0C];
        D_00446E48 = stock.counts[4][D_005CF380[0]->unk0C];
        D_00446E4C = context->unk04;
        D_00446E50 = context->unk04 * values.values[4][D_005CF380[0]->unk0C].take_value;
        switch (item_name_defs[D_005CF380[0]->unk0C].unk30_20 & 3) {
        case 1:
            D_00446E30 = item_counts[ITEM_BONE];
            break;
        case 2:
            D_00446E30 = item_counts[ITEM_SCALE];
            break;
        case 3:
            D_00446E30 = item_counts[ITEM_CLAW];
            break;
        }
    }
    if (D_00446E48 >= 100) {
        D_00446E48 = 99;
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
                MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 0, (int)D_00425510[i]);
                break;
            case 1:
                MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 1, (int)D_00425510[i]);
                break;
            case 2:
                break;
            case 3:
                if (D_005CF380[0]->group != 12) {
                    MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
                }
                break;
            }
        } else if (context->arg != 4) {
            MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 2, (int)D_00425510[i]);
        } else {
            MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 6, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }

    MenuText_SetColor(D_00446C78);
    memset(view_matrix, 0, 0x40);
    view_matrix[2][2] = view_matrix[1][1] = view_matrix[0][0] = 0.8f;
    view_matrix[3][3] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CB8 + 3.1415927f;
    D_00446CB8 = angle - (s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)view_matrix, D_00446CB8);
    sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, D_00428A00);
    Menu_DrawItemModel((f32 *)local_matrix);

    if (D_00447318 == 0 && (D_00446DE8 & 0x1000) && func_001E4FE0(record, D_005CF380[0]->group)) {
        D_00446CB4 = 1;
        if (D_00446C9C == 0) {
            D_00446C9C = 1;
        } else {
            D_00447312 = 2;
        }
    }
    if (D_00446C9C != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
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
            if (D_00446CB4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CB4 = 0;
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E24 = D_003682F0[record].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_005CF380[0]->unk0C].unk3E;
            kind = D_00413FBC[D_005CF380[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C14, D_00446A9C)
            }
            DRAW_PACKETS(D_00446C14, D_00446A9C, kind)
            packets = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (D_00446CB4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CB4 = 0;
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E1C = item_name_defs[D_005CF380[0]->unk0C].unk3E;
            kind = D_00413FBC[D_005CF380[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE(D_00425910, D_00446C0C, D_00446A94)
            }
            DRAW_PACKETS(D_00446C0C, D_00446A94, kind)
            packets = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (D_00446CB4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CB4 = 0;
            D_00446DF4 = ScaleValue(D_003679A0[record].unk00[0] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[0], D_003679A0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003679A0[record].unk00[1] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[1], D_003679A0[record].level, 100);
            D_00446DFC = ScaleValue(D_003679A0[record].unk00[2] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[2], D_003679A0[record].level, 100);
            D_00446E00 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[3], D_003679A0[record].level, 100);
            D_00446E04 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[4], D_003679A0[record].level, 100);
            D_00446E08 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[5], D_003679A0[record].level, 100);
            D_00446E0C = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[6], D_003679A0[record].level, 100);
            D_00446E10 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[7], D_003679A0[record].level, 100);
            D_00446E14 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[8], D_003679A0[record].level, 100);
            D_00446E20 = (s32)(weapon_defs[record].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[record].level;
            D_00446E1C = item_name_defs[D_005CF380[0]->unk0C].unk3E;
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

    node = D_005CF360[0];
    MenuText_SetColor(D_00446C78);
    position.raw = 0x0C7A12007C307080L;
    for (row = 0; row < 7; row++) {
        item = &item_name_defs[node->unk0C];
        MenuText_SetPen(position);
        MenuText_DrawLine((u16 *)item, 0x1A);
        position.f.mid16 += 0xB0;
        if (D_005CF370[0]->prev == node) {
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
                D_00425510[0xFD] = Trade_PulseValue(0x10, 0x20, D_00425510[0xFD]);
            }
            bar.alpha = (s32)D_00425510[0xFD];
            MenuDraw_Bar(&bar);
        }
        SELECT_PART((s8)D_00446C60[i].unk42);
        kind = D_00446C60[i].unk3E;
        if (context->arg != 4) {
            MenuSprite_Draw((int)&D_00446C60[i], kind + 1000, 2, (int)D_00425D10[i]);
        } else {
            MenuSprite_Draw((int)&D_00446C60[i], kind + 1000, 6, (int)D_00425D10[i]);
        }
        SELECT_PART(0);
    }

    chosen = &item_name_defs[D_005CF380[0]->unk0C];
    MenuText_SetColor(D_00446C78);
    position.raw = 0x0A2DC6C07DB078C0L;
    MenuText_SetPen(position);
    MenuText_DrawLineCentered(chosen, 0x18);
    if (context->arg == 4) {
        position.raw = 0x000DBBA079E08A30L;
        label_position.raw = 0x000DBBA081807C60L;
        MenuText_SetPen(position);
        switch (item_name_defs[D_005CF380[0]->unk0C].unk30_20 & 3) {
        case 1:
            label = D_00426620;
            break;
        case 2:
            label = D_00426630;
            label_position.raw = 0x000DBBA081807C00L;
            break;
        case 3:
            label = D_00426640;
            break;
        default: // match: a default that repeats case 3
            label = D_00426640;
            break;
        }
        MenuText_DrawWrapped(label, 10);
        MenuText_SetPen(label_position);
        MenuText_DrawWrapped(label, 10); // match: the same label a second time
    }

    current = D_005CF380[0];
    texts = Message_GetFontText(D_00446F38);
    number = current->unk0C + MSG_ITEM_TEXT_FIRST + 1; // match: its own local, or the offset is folded into the load
    text_offset = ((s32 *)texts)[number];
    position.raw = 0x000F424083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            if ((D_00413F80.gold >= D_00446E50 && context->arg != 4) || (D_00446E30 >= D_00446E50 && context->arg == 4)) {
                Sound_PlayMenu(0);
                D_00447311 = 3;
                D_00447312 = 2;
                D_00447318 = 1;
                if (D_00446C90 != NULL) {
                    D_00446C90->next = NULL;
                }
            } else {
                Sound_PlayMenu(3);
            }
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
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
                    if (context->arg != 4) {
                        D_00413F80.gold -= D_00446E50;
                        D_00446CA0 = Trade_UpdateSelectScreen;
                        ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                    } else {
                        switch (item_name_defs[D_005CF380[0]->unk0C].unk30_20 & 3) {
                        case 1:
                            item_counts[ITEM_BONE] -= D_00446E50;
                            break;
                        case 2:
                            item_counts[ITEM_SCALE] -= D_00446E50;
                            break;
                        case 3:
                            item_counts[ITEM_CLAW] -= D_00446E50;
                            break;
                        }
                        D_00446CA0 = trade_list_handlers[0];
                    }
                    item_counts[D_005CF380[0]->unk0C] += context->unk04;
                    if (item_counts[D_005CF380[0]->unk0C] >= 100) {
                        item_counts[D_005CF380[0]->unk0C] = 99;
                    }
                    if (stock.counts[context->arg][D_005CF380[0]->unk0C] != 0xFF) {
                        stock.counts[context->arg][D_005CF380[0]->unk0C] -= context->unk04;
                    }
                    context->unk04 = 0;
                    Trade_BuildItemRings(context->arg);
                    if (context->arg == 4) {
                        ((int (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing; an int result orders the loads after it
                        D_00446C40 = D_00446C24;
                        D_00446C44 = D_00446C28;
                        D_00446C48 = D_00446C38;
                        ((void (*)(s32))Trade_ChainModels)(0); // match: the callee takes nothing
                        D_00447311 = 0;
                        context->unk04 = 0;
                    }
                } else if (D_00447311 == 6) {
                    D_00446CA0 = trade_list_handlers[0];
                }
                D_00447312 = 0;
                D_00447311 = 0;
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

/* The trade screen while the list of what the player owns is shown: draws the list, the cursor bar, the turning
   model's matrix and the chosen item's values, and handles the buttons and the fades in and out. */
static void Trade_UpdateOwnedList(HandlerContext *context) {
    TradeStockTables stock = trade_stock_tables; // match: copied to the frame and never read
    TradeValueTables values = trade_value_tables;
    s32 details_opened = 0;
    TradeGroupBases bases = item_group_bases;
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
        D_00446CA0 = Trade_UpdateSelectScreen;
        return;
    }
    if (D_00447311 < 2) {
        Menu_MoveItemListCursor(context, D_00447318);
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
                MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 0, (int)D_00425510[i]);
                break;
            case 1: // match: an empty case of its own
                break;
            case 2:
                MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 2, (int)D_00425510[i]);
                break;
            case 3:
                if (D_00426110[0]->group != 11) {
                    MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
                }
                break;
            }
        } else {
            MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
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
                D_00425510[0xFD] = Trade_PulseValue(0x10, 0x20, D_00425510[0xFD]);
            }
            bar.alpha = (s32)D_00425510[0xFD];
            MenuDraw_Bar(&bar);
            break;
        }
    }

    MenuText_SetColor(D_00446C78);
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
    Menu_DrawItemModel((f32 *)local_matrix);

    for (; row < 7; node = node->prev, row++) {
        item = &item_name_defs[node->unk0C];
        MenuText_SetPen(position);
        MenuText_DrawLine((u16 *)item, 0x1A);
        position.f.mid16 += 0xB0;
        if (D_00426130[0]->prev == node) {
            break;
        }
    }

    slot_node = D_00426120[0];
    {
        TradeValueSlots slots = trade_value_slots;

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
                Sound_PlayMenu(1);
            }
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E24 = D_003682F0[record].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            worn = D_00413FBC[D_00426110[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE_2(D_00425910, D_00446C14, D_00446A9C)
            }
            DRAW_PACKETS(D_00446C14, D_00446A9C, worn)
            packets = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (details_opened != 0) {
                Sound_PlayMenu(1);
            }
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            worn = D_00413FBC[D_00426110[0]->group];
            if (D_00447312 == 1) {
                FILL_FADE_2(D_00425910, D_00446C0C, D_00446A94)
            }
            DRAW_PACKETS(D_00446C0C, D_00446A94, worn)
            packets = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (details_opened != 0) {
                Sound_PlayMenu(1);
            }
            D_00446DF4 = ScaleValue(D_003679A0[record].unk00[0] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[0], D_003679A0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003679A0[record].unk00[1] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[1], D_003679A0[record].level, 100);
            D_00446DFC = ScaleValue(D_003679A0[record].unk00[2] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[2], D_003679A0[record].level, 100);
            D_00446E00 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[3], D_003679A0[record].level, 100);
            D_00446E04 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[4], D_003679A0[record].level, 100);
            D_00446E08 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[5], D_003679A0[record].level, 100);
            D_00446E0C = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[6], D_003679A0[record].level, 100);
            D_00446E10 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[7], D_003679A0[record].level, 100);
            D_00446E14 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[8], D_003679A0[record].level, 100);
            D_00446E20 = (s32)(weapon_defs[record].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[record].level;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_00426110[0]->group];
            D_00446E2C = 100;
            if (D_00447312 == 1) {
                FILL_FADE_2(D_00425910, D_00446C10, D_00446A98)
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
                    FILL_FADE_2(D_00425910, packets, count)
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
    texts = Message_GetFontText(D_00446F38);
    text_index = current->unk0C + MSG_ITEM_TEXT_FIRST + 1; // match: the index is worked out apart from the load
    text_offset = ((s32 *)texts)[text_index];
    position.raw = 0x000A875083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            Sound_PlayMenu(0);
            D_00447311 = 2;
            D_00447312 = 3;
            D_00447318 = 1;
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
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
                FILL_FADE_2(D_00425510, D_00446C00, D_00446A78)
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
                    D_00446CA0 = Trade_UpdateGiveAmount;
                    context->unk04 = 1;
                    D_00447310 = 0;
                } else if (D_00447311 == 5) {
                    context->unk04 = 1;
                    D_00446CA0 = Trade_UpdateSelectScreen;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
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

/* The trade screen while the amount to give is chosen: draws the lists and the chosen item's values, and gives or cancels. */
static void Trade_UpdateGiveAmount(HandlerContext *context) {
    TradeStockTables stock = trade_stock_tables;
    TradeValueTables values = trade_value_tables;
    TradeGroupBases bases = item_group_bases;
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
            Sound_PlayMenu(1);
            context->unk04++;
        } else if (D_00446DE8 & 0x80) {
            Sound_PlayMenu(1);
            context->unk04--;
        }
    }
    if (item_counts[D_00426110[0]->unk0C] < context->unk04) {
        context->unk04 = 1;
    } else if (context->unk04 <= 0) {
        context->unk04 = item_counts[D_00426110[0]->unk0C];
    }
    D_00446E44 = item_counts[D_00426110[0]->unk0C];
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
                MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 0, (int)D_00425510[i]);
                break;
            case 1:
                MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 1, (int)D_00425510[i]);
                break;
            case 2:
                break;
            case 3:
                if (D_00426110[0]->group != 11) {
                    MenuSprite_Draw((int)&D_00446C00[i], kind + 1000, 3, (int)D_00425510[i]);
                }
                break;
            }
        } else {
            // match: 10000 where the other screens add 1000 (the callee takes it modulo 1000, so it draws the same)
            MenuSprite_Draw((int)&D_00446C00[i], kind + 10000, 4, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }

    MenuText_SetColor(D_00446C78);
    memset(view_matrix, 0, 0x40);
    view_matrix[2][2] = view_matrix[1][1] = view_matrix[0][0] = 0.8f;
    view_matrix[3][3] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CC0 + 3.1415927f;
    D_00446CC0 = angle - (s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)view_matrix, D_00446CC0);
    sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, D_00428A20);
    Menu_DrawItemModel((f32 *)local_matrix);

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
                Sound_PlayMenu(1);
            }
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E24 = D_003682F0[record].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
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
                Sound_PlayMenu(1);
            }
            D_00446DF4 = ScaleValue(D_003682F0[record].unk00[0] + worn_defs[record].unk00[0], D_003682F0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003682F0[record].unk00[1] + worn_defs[record].unk00[1], D_003682F0[record].level, 100);
            D_00446DFC = ScaleValue(D_003682F0[record].unk00[2] + worn_defs[record].unk00[2], D_003682F0[record].level, 100);
            D_00446E00 = ScaleValue(worn_defs[record].unk00[3], D_003682F0[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[4], D_003682F0[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[5], D_003682F0[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[6], D_003682F0[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[7], D_003682F0[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[8], D_003682F0[record].level, 100);
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
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
                Sound_PlayMenu(1);
            }
            D_00446DF4 = ScaleValue(D_003679A0[record].unk00[0] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[0], D_003679A0[record].level, 100);
            D_00446DF8 = ScaleValue(D_003679A0[record].unk00[1] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[1], D_003679A0[record].level, 100);
            D_00446DFC = ScaleValue(D_003679A0[record].unk00[2] + weapon_defs[record].unk00[D_003679A0[record].unk14].values[2], D_003679A0[record].level, 100);
            D_00446E00 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[3], D_003679A0[record].level, 100);
            D_00446E04 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[4], D_003679A0[record].level, 100);
            D_00446E08 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[5], D_003679A0[record].level, 100);
            D_00446E0C = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[6], D_003679A0[record].level, 100);
            D_00446E10 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[7], D_003679A0[record].level, 100);
            D_00446E14 = ScaleValue(weapon_defs[record].unk00[D_003679A0[record].unk14].values[8], D_003679A0[record].level, 100);
            D_00446E20 = (s32)(weapon_defs[record].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[record].level;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
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
    MenuText_SetPen(position);
    for (row = 0; row < 7; row++) {
        MenuText_DrawLine((u16 *)&item_name_defs[node->unk0C], 0x1A);
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
                D_00425510[0xFD] = Trade_PulseValue(0x10, 0x20, D_00425510[0xFD]);
            }
            bar.alpha = (s32)D_00425510[0xFD];
            MenuDraw_Bar(&bar);
        }
        kind = D_00446C60[i].unk3E;
        SELECT_PART((s8)D_00446C60[i].unk42);
        MenuSprite_Draw((int)&D_00446C60[i], kind + 1000, 4, (int)D_00425D10[i]);
        SELECT_PART(0);
    }

    chosen = &item_name_defs[D_00426110[0]->unk0C];
    MenuText_SetColor(D_00446C78);
    position.raw = 0x0A2DC6C07DB078C0L;
    MenuText_SetPen(position);
    MenuText_DrawLineCentered(chosen, 0x18);

    current = D_00426110[0];
    texts = Message_GetFontText(D_00446F38);
    number = current->unk0C + MSG_ITEM_TEXT_FIRST + 1; // match: its own local, or the offset is folded into the load
    text_offset = ((s32 *)texts)[number];
    position.raw = 0x000A875083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            Sound_PlayMenu(0);
            D_00413F80.gold += D_00446E50;
            if (D_00413F80.gold > 999999) {
                D_00413F80.gold = 999999;
            }
            item_counts[D_00426110[0]->unk0C] -= context->unk04;
            if (D_00426110[0]->unk0C == D_00413F80.weapon + ITEM_WEAPON_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST ||
                D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_HELM] + ITEM_WORN_FIRST ||
                D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST ||
                D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_RING] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST ||
                D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.shortcut) {
                if (item_counts[D_00426110[0]->unk0C] == 0) {
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
            Sound_PlayMenu(2);
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
                    D_00446CA0 = Trade_UpdateSelectScreen;
                    Trade_BuildItemRings(context->arg);
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else if (D_00447311 == 6) {
                    D_00446CA0 = trade_list_handlers[1];
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

/* Builds the model file names of the five link nodes: the selected item, its two neighbours, and the items that
   come into view next on either side (found by walking both ways from the page mark, seven steps in all). */
static void Trade_SetModelNames(void) {
    MenuItem *backward;
    MenuItem *forward;
    s32 i;

    forward = backward = D_005CF360[0];
    if (D_00446C48 >= 8) {
        for (i = 0; i < 7; i++) {
            forward = forward->next;
            backward = backward->prev;
            if (forward == D_00446C40) {
                while (++i < 7 && backward != D_00446C44) {
                    backward = backward->prev;
                }
                break;
            }
            if (backward == D_00446C44) {
                while (++i < 7 && forward != D_00446C40) {
                    forward = forward->next;
                }
                break;
            }
        }
    } else {
        forward = backward = D_00446C40;
    }
    sprintf(D_00426498, "menu/select2/i%04d.mod", item_name_defs[D_005CF380[0]->unk0C].file_number);
    sprintf(D_00426538, "menu/select2/i%04d.mod", item_name_defs[D_005CF380[0]->prev->unk0C].file_number);
    sprintf(D_004264E8, "menu/select2/i%04d.mod", item_name_defs[D_005CF380[0]->next->unk0C].file_number);
    sprintf(D_00426588, "menu/select2/i%04d.mod", item_name_defs[forward->unk0C].file_number);
    sprintf(D_004265D8, "menu/select2/i%04d.mod", item_name_defs[backward->unk0C].file_number);
}

/* Moves the list cursor one row (0x10, 0x40) or one page (0x20, 0x80, lists of 8 and more), scrolling the window. */
static s32 Trade_MoveListCursor(HandlerContext *context) {
    MenuItem *cursor = D_005CF380[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if (D_00446DE8 & 0x10) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == D_005CF360[0] && next != D_005CF370[0]->prev) {
                    D_005CF360[0] = next;
                    D_005CF370[0] = D_005CF370[0]->next;
                }
                Sound_PlayMenu(1);
                D_005CF380[0] = cursor->next;
            }
            Menu_ShiftListModelsNext(0);
        } else if (D_00446DE8 & 0x40) {
            if (cursor->prev != NULL) {
                if (D_005CF370[0]->prev == cursor && D_005CF360[0]->next != cursor) {
                    D_005CF370[0] = cursor;
                    D_005CF360[0] = D_005CF360[0]->prev;
                }
                Sound_PlayMenu(1);
                D_005CF380[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((D_00446DE8 & 0x20) && D_00446C48 >= 8) {
            if (cursor->prev != NULL) {
                if (D_005CF360[0]->next != D_005CF370[0]->prev) {
                    rows = 0;
                    while (func_001ED738_ScrollUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageUp(0);
        } else if ((D_00446DE8 & 0x80) && D_00446C48 >= 8) {
            if (cursor->next != NULL) {
                if (D_005CF360[0]->next != D_005CF370[0]->prev) {
                    rows = 0;
                    while (func_001ED738_ScrollDown()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageDown(0);
        } else {
            return FALSE;
        }
        Trade_SetModelNames();
        return TRUE;
    }
    return FALSE;
}

/* Builds the two item rings of the trade screen: the items that can be handed in (mode 0: those that can still
   gain a level, mode 1: any idle one) at D_00426110, and at D_005CF390 the items handed in earlier whose time is
   up. Each ring's count goes to D_00446C34 / D_00446C38. */
static void Smith_BuildItemRings(s32 mode) {
    MenuItem *last = NULL;
    MenuItem *first = NULL;
    s32 count = 0;
    s32 i;

    D_00426120[0] = NULL;
    D_00426130[0] = NULL;
    D_00446C1C = NULL;
    D_00446C20 = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    if (mode == 0) {
        APPEND_UPGRADABLE(D_004231F0, D_003679A0, 0, 150, 70)
        APPEND_UPGRADABLE(D_00423650, D_003682F0, 0, 220, 30)
        APPEND_UPGRADABLE(D_00423830, D_003682F0, 30, 250, 25)
        APPEND_UPGRADABLE(D_004239C0, D_003682F0, 55, 275, 25)
        APPEND_UPGRADABLE(D_00423B50, D_003682F0, 80, 300, 25)
        APPEND_UPGRADABLE(D_00423CE0, D_003682F0, 105, 325, 25)
    } else if (mode == 1) {
        APPEND_IDLE(D_004231F0, D_003679A0, 0, 150, 70)
        APPEND_IDLE(D_00423650, D_003682F0, 0, 220, 30)
        APPEND_IDLE(D_00423830, D_003682F0, 30, 250, 25)
        APPEND_IDLE(D_004239C0, D_003682F0, 55, 275, 25)
        APPEND_IDLE(D_00423B50, D_003682F0, 80, 300, 25)
        APPEND_IDLE(D_00423CE0, D_003682F0, 105, 325, 25)
        APPEND_IDLE(D_00423E70, D_003682F0, 130, 350, 10)
        APPEND_IDLE(D_00423F10, D_003682F0, 140, 360, 10)
        APPEND_IDLE(D_00423FB0, D_003682F0, 150, 370, 10)
    }
    D_00446C34 = count;
    if (first != NULL) {
        last->prev = first;
        D_00426120[0] = first;
        D_00426110[0] = first;
        first->next = last;
        if (D_00426130[0] == NULL) {
            D_00426130[0] = last->next;
        } else {
            D_00426130[0] = D_00426130[0]->next;
        }
        D_00446C1C = first;
        D_00446C20 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C20 = D_00446C20->next;
        }
    } else {
        D_00426110[0] = NULL;
    }

    D_005CF390[0] = NULL;
    D_005CF3A0[0] = NULL;
    last = NULL;
    first = NULL;
    count = 0;
    D_00446C24 = NULL;
    D_00446C28 = NULL;
    func_001D20E0();
    for (i = 1; i < 380; i++) {
        if (i >= 150) {
            if (i < 220) {
                if (i != 150 && D_003679A0[i - 150].unk18 != 0 && !(item_name_defs[i].state & 2) &&
                    func_001D2258(D_003679A0[i - 150].unk19.time, D_00446A28) >= item_name_defs[i].unk36 * 60) {
                    // match: written out; through APPEND's `(node)->next` the item pointer is reduced differently
                    count++;
                    if (count == 7) {
                        D_005CF3A0[0] = &D_00421A30[i];
                    }
                    if (last != NULL) {
                        D_00421A30[i].next = last;
                        last->prev = &D_00421A30[i];
                    } else {
                        first = &D_00421A30[i];
                    }
                    last = &D_00421A30[i];
                }
            } else {
                if (i != 220 && D_003682F0[i - 220].unk18 != 0 && !(item_name_defs[i].state & 2) &&
                    func_001D2258(D_003682F0[i - 220].unk19.time, D_00446A28) >= item_name_defs[i].unk36 * 60) {
                    count++;
                    if (count == 7) {
                        D_005CF3A0[0] = &D_00421A30[i];
                    }
                    if (last != NULL) {
                        D_00421A30[i].next = last;
                        last->prev = &D_00421A30[i];
                    } else {
                        first = &D_00421A30[i];
                    }
                    last = &D_00421A30[i];
                }
            }
        }
    }
    D_00446C38 = count;
    if (first != NULL) {
        last->prev = first;
        D_005CF390[0] = first;
        D_005CF3B0[0] = first;
        first->next = last;
        if (D_005CF3A0[0] == NULL) {
            D_005CF3A0[0] = last->next;
        } else {
            D_005CF3A0[0] = D_005CF3A0[0]->next;
        }
        D_00446C24 = first;
        D_00446C28 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C28 = D_00446C28->next;
        }
    } else {
        D_005CF3B0[0] = NULL;
    }
}

/* the same for the smith's screens */
static int (*const smith_run_steps[2])(s32) IN_RODATA = {
    (int (*)(s32))Smith_CopyFrameToTexture,
    Smith_RunStep,
};
static int (*const smith_steps[4])(HandlerContext *) IN_RODATA = {
    (int (*)(HandlerContext *))Smith_Start,
    Smith_RunHandler,
    (int (*)(HandlerContext *))Smith_CutModelChain,
    NULL,
};
static void (*const smith_list_handlers[3])(HandlerContext *) IN_RODATA = {
    Smith_UpdateUpgradeList,
    Smith_UpdateListWithConfirm,
    Smith_UpdateCollectList,
};

/* Runs the screen loop of the handler D_00446CCC selects until it returns 0, watching the pad type for changes. */
void Smith_Run(void) {
    D_00447318 = 1;
    D_00447313 = 0;
    D_00447314 = 0;
    D_00447315 = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    Vram_Clear(0x3D00, 0x3F00);
    Sound_MutePositional();
    Sound_UpdateTagged();
    D_00446C94 = 0;
    D_0044731C = D_00447320 = Pad_GetMode(0, 0);
    while (smith_run_steps[D_00446CCC](0) != 0) {
        sceGsSyncPath(0, 0);
        D_00447328 = Menu_EndFrame(1);
        func_001B5460();
        D_0044731C = Pad_GetMode(0, 0);
        if (D_0044731C != D_00447320) {
            D_00447308 = 1;
        }
        D_00447320 = D_0044731C;
        MenuPad_Update();
        if (D_00447308 > 0) {
            D_00447308++;
            if (D_00447308 >= 21) {
                D_00447308 = 0;
            } else {
                D_00446DE8 = 0;
            }
        }
    }
    sceGsSyncPath(0, 0);
    while (sceGsSyncV(0) != 0) {
    }
    Sound_UnmutePositional();
    MenuHeap_FreeToMark();
    D_00446CCC = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}

/* Start-up step of the screen Smith_UpdateSelectScreen runs: loads its textures and blocks, clears the alpha tables and moves on. */
static s32 Smith_Start(void) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;

    func_001095E0();
    func_001095D0();
    func_0010B238((TextureTableHeader *)Message_GetFontTexture(D_00446F38));
    func_00109E88();
    func_001095D8();
    func_001096E8();
    ((s32 (*)(void))func_00109800)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < D_00446DEC; i++) {
        block = &D_0042A2A0[i];
        MenuTexture_Upload(block->data, block);
    }
    MenuArena_Init(D_00446F68);
    packet = MenuDraw_NextPacket();
    packet[0] = 0x8001;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 0x3F;
    packet[7] = 0;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 2;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    memset(D_00425510, 0, 0x400);
    memset(D_00425910, 0, 0x400);
    memset(D_00425D10, 0, 0x400);
    D_00446C78.blue = 0x80;
    D_00446C78.green = 0x80;
    D_00446C78.red = 0x80;
    D_00446C78.alpha = 0;
    D_00446C88.blue = 0x80;
    D_00446C88.green = 0x80;
    D_00446C88.red = 0x80;
    D_00446C88.alpha = 0;
    Menu_InitLoadBuffers();
    D_00446CC4 = Smith_UpdateSelectScreen;
    D_00446CC8++;
    return TRUE;
}

/* Copies the shown frame buffer into a work buffer and draws that back into both frame buffers, then steps the screen counter. */
static s32 Smith_CopyFrameToTexture(void) {
    f32 unused[28]; // match: 0x70 bytes of locals nothing uses
    u32 texture_base; /* where the shown buffer is as a texture: 0x1180 when it is the second one */
    u32 *packet;

    packet = MenuDraw_NextPacket();
    texture_base = 0x1180;
    if (D_00446F78 == 0) {
        texture_base = 0;
    }
    packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
    packet[2] = 0xE;
    packet[3] = 0;
    *(u64 *)&packet[4] = (D_004261A0[0] & ~0x1FFL) | 0x192;
    packet[6] = 0x4C;
    packet[7] = 0;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = 0x18;
    packet[11] = 0;
    packet[12] = 0x30000;
    packet[13] = 0;
    packet[14] = 0x47;
    packet[15] = 0;
    packet[16] = 0x2800000;
    packet[17] = 0xE00000;
    packet[18] = 0x40;
    packet[19] = 0;
    *(u64 *)&packet[20] = texture_base | 0x28028000 | 0x640000000L; /* TEX0: 1024 by 512 */
    packet[22] = 0x6;
    packet[23] = 0;
    packet[24] = 0x80808080;
    *(f32 *)&packet[25] = 1.0f;
    packet[26] = 0x1;
    packet[27] = 0;
    packet[28] = 0;
    packet[29] = 0;
    packet[30] = 0x2;
    packet[31] = 0;
    packet[32] = 0;
    packet[33] = 0;
    packet[34] = 0x5;
    packet[35] = 0;
    *(f32 *)&packet[36] = 0.625f;
    *(f32 *)&packet[37] = 0.875f;
    packet[38] = 0x2;
    packet[39] = 0;
    packet[40] = 0xE002800;
    packet[41] = 0;
    packet[42] = 0x5;
    packet[43] = 0;
    packet[44] = 0;
    packet[45] = 0;
    packet[46] = 0x3F;
    packet[47] = 0;
    *(u64 *)&packet[48] = D_004261A0[0];
    packet[50] = 0x4C;
    packet[51] = 0;
    *(u64 *)&packet[52] = 0x66802B240;
    packet[54] = 0x6;
    packet[55] = 0;
    packet[56] = 0;
    packet[57] = 0;
    packet[58] = 0x2;
    packet[59] = 0;
    packet[60] = 0;
    packet[61] = 0;
    packet[62] = 0x5;
    packet[63] = 0;
    *(f32 *)&packet[64] = 0.625f;
    *(f32 *)&packet[65] = 0.4375f;
    packet[66] = 0x2;
    packet[67] = 0;
    packet[68] = 0xE002800;
    packet[69] = 0;
    packet[70] = 0x5;
    packet[71] = 0;
    *(u64 *)&packet[72] = D_004261A0[46];
    packet[74] = 0x4C;
    packet[75] = 0;
    packet[76] = 0;
    packet[77] = 0;
    packet[78] = 0x2;
    packet[79] = 0;
    packet[80] = 0;
    packet[81] = 0;
    packet[82] = 0x5;
    packet[83] = 0;
    *(f32 *)&packet[84] = 0.625f;
    *(f32 *)&packet[85] = 0.4375f;
    packet[86] = 0x2;
    packet[87] = 0;
    packet[88] = 0xE002800;
    packet[89] = 0;
    packet[90] = 0x5;
    packet[91] = 0;
    packet[92] = 0x50000;
    packet[93] = 0;
    packet[94] = 0x47;
    packet[95] = 0;
    packet[96] = 0;
    packet[97] = 0;
    packet[98] = 0x3F;
    packet[99] = 0;
    packet[100] = 0x44;
    packet[101] = 0;
    packet[102] = 0x42;
    packet[103] = 0;
    packet[0] = 0x8019;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 26;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    sceGsSyncPath(0, 0);
    D_00446CCC++;
    return 1;
}

/* Calls the handler selected by D_00446CC8 with the context at D_005CF340, storing value in it first. */
static int Smith_RunStep(int value) {
    int (*handler)(HandlerContext *);

    Menu_StepModelLoads(0);
    handler = smith_steps[D_00446CC8];
    D_005CF340.arg = value;
    if (handler != NULL) {
        return handler(&D_005CF340);
    }
    return 1;
}

/* Runs the handler in D_00446CC4 with the context this step was given. */
static int Smith_RunHandler(HandlerContext *context) {
    D_00446CC4(context);
    return 1;
}

/* Cuts the chain after the first link node (and after the node it points at); with no chain, resets D_00446CC8. */
static int Smith_CutModelChain(void) {
    LinkNode *node = D_00446C90;
    LinkNode *other;

    if (node == NULL) {
        D_00446CC8 = 0;
        return 0;
    }
    other = node->unk10;
    node->next = NULL;
    if (other != NULL) {
        other->next = NULL;
    }
    return 1;
}

/* Rebuilds the five link nodes for the current D_005CF3B0 item and chains them (the Menu_ChainItemListModels of this list). */
static void Smith_ChainModels(void) {
    if (D_005CF3B0[0] != NULL) {
        Smith_SetModelNames();
        Menu_TakeLoadBuffer(&D_00426470);
        Menu_TakeLoadBuffer(&D_004264C0);
        Menu_TakeLoadBuffer(&D_00426510);
        Menu_TakeLoadBuffer(&D_00426560);
        Menu_TakeLoadBuffer(&D_004265B0);
        D_00426470.next = &D_004264C0;
        D_00426470.callback = (void (*)(void))Menu_StepModelLoad;
        D_00426470.unk20 = 0;
        D_004264C0.next = &D_00426510;
        D_004264C0.callback = (void (*)(void))Menu_StepModelLoad;
        D_004264C0.unk20 = 0;
        D_004264C0.unk1C = 0;
        D_00426510.next = &D_00426560;
        D_00426510.callback = (void (*)(void))Menu_StepModelLoad;
        D_00426510.unk20 = 0;
        D_00426510.unk1C = 0;
        D_00426560.next = &D_004265B0;
        D_00426560.callback = (void (*)(void))Menu_StepModelLoad;
        D_00426560.unk20 = 0;
        D_00426560.unk1C = 0;
        D_004265B0.next = NULL;
        D_004265B0.callback = (void (*)(void))Menu_StepModelLoad;
        D_004265B0.unk20 = 0;
        D_00426470.unk1C = D_004265B0.unk1C = 0;
        D_00446C90 = &D_00426470;
    }
}

/* The three-line menu in front of the item lists: draws its parts and the cursor bar, opens the list of the chosen
   line, and fades the parts in and out. */
static void Smith_UpdateSelectScreen(HandlerContext *context) {
    u32 i;
    s32 line_count;
    s32 bar_drawn;
    MenuCursorBar bar;
    u32 full_alpha;
    u8 kind;

    if (D_00447318 == 0 && D_00446CD0 != 0) {
        if (D_00446DE8 & 0x10) {
            Sound_PlayMenu(1);
            context->unk04--;
        } else if (D_00446DE8 & 0x40) {
            Sound_PlayMenu(1);
            context->unk04++;
        }
    }
    line_count = 3;
    context->unk04 = (context->unk04 + 3) % line_count;

    bar_drawn = FALSE;
    for (i = 0; i < D_00446AA0; i++) {
        if (!bar_drawn && D_00446C4C[i].unk43 == 1) {
            bar.first.f.lo16 = 0x76C0;
            bar.first.f.mid16 = context->unk04 * 0xD0 + 0x7EF0;
            bar.first.f.mid24 = 0xDBBA0;
            bar.green = bar.red = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x12C0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xC0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar_drawn = TRUE;
            if (D_00447316 == 1) {
                D_00425510[253] = Trade_PulseValue(0x10, 0x20, D_00425510[253]);
            } else {
                D_00425510[253] = 0.0f;
            }
            bar.alpha = (s32)D_00425510[253];
            MenuDraw_Bar(&bar);
        }
        ((int (*)(s8))func_001E5210)(D_00446C4C[i].unk42); // match: the byte is passed sign-extended here
        MenuSprite_Draw((int)&D_00446C4C[i], 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if (D_00446CD0 == 0) {
        for (i = 0; i < D_00446AB8; i++) {
            kind = D_00446C64[i].unk3E;
            MenuSprite_DrawPlain((int)&D_00446C64[i], kind, 1);
        }
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            if (D_00446CD0 == 0) {
                Sound_PlayMenu(0);
                D_00446CD0 = 1;
            } else {
                D_00446AD4 = 0;
                switch (context->unk04) {
                case 0:
                    Smith_BuildItemRings(0);
                    if (D_00426110[0] != NULL) {
                        Sound_PlayMenu(0);
                        D_00447318 = 1;
                        D_00447316 = 2;
                    } else {
                        Sound_PlayMenu(3);
                    }
                    break;
                case 1:
                    Sound_PlayMenu(0);
                    Smith_BuildItemRings(1);
                    if (item_counts[ITEM_EARTH_STONE] != 0) {
                        D_00447318 = 1;
                        D_00447316 = 3;
                    } else {
                        D_00446CD0 = 0;
                    }
                    break;
                case 2:
                    Smith_BuildItemRings(2);
                    if (D_00446C38 > 0) {
                        Sound_PlayMenu(0);
                        D_00447318 = 1;
                        D_00447316 = 4;
                    } else {
                        Sound_PlayMenu(3);
                    }
                    break;
                }
            }
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
            if (D_00446CD0 == 0) {
                D_00446CD0 = 1;
            } else {
                D_00447316 = 5;
                D_00447318 = 1;
            }
        }
    }

    switch (D_00447316) {
    case 0:
        D_00425510[253] += 1.6f;
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446AA0; i++) {
            D_00425510[i] += D_00446C4C[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C4C[i].alpha) {
                for (i = 0; i < D_00446AA0; i++) {
                    full_alpha = D_00446C4C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00447318 = 0;
                D_00447316 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        D_00425510[253] -= 1.6f;
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446AA0; i++) {
            D_00425510[i] -= D_00446C4C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                // match: the table has two entries; line 2 reads the word after it (trade_value_tables's first pointer)
                D_00446CA0 = trade_list_handlers[context->unk04];
                if (D_00447316 == 2) {
                    D_00446C40 = D_00446C1C;
                    D_00446CC4 = smith_list_handlers[context->unk04];
                    D_00446C44 = D_00446C20;
                    D_00446C48 = D_00446C34;
                    ((void (*)(s32))Menu_ChainItemListModels)(0); // match: the callee takes nothing
                } else if (D_00447316 == 3) {
                    D_00446C40 = D_00446C1C;
                    D_00446CC4 = smith_list_handlers[context->unk04];
                    D_00446C44 = D_00446C20;
                    D_00446C48 = D_00446C34;
                    D_00447313 = 0;
                    ((void (*)(s32))Menu_ChainItemListModels)(0); // match: the callee takes nothing
                } else if (D_00447316 == 4) {
                    D_00446C40 = D_00446C24;
                    D_00446CC4 = smith_list_handlers[context->unk04];
                    D_00446C44 = D_00446C28;
                    D_00446C48 = D_00446C38;
                    ((void (*)(s32))Smith_ChainModels)(0); // match: the callee takes nothing
                } else if (D_00447316 == 5) {
                    D_00446CC8++;
                }
                D_00447316 = 0;
                context->unk04 = 0;
                break;
            }
        }
        break;
    }
}

/* Writes the model file names of the selected item, its two neighbours, and the two items just outside the visible
   window (found by walking at most 7 steps out from D_005CF390 towards the list's ends) into the five link nodes. */
static void Smith_SetModelNames(void) {
    MenuItem *before = D_005CF390[0];
    MenuItem *after = before;
    int steps;

    if (D_00446C48 >= 8) {
        for (steps = 0; steps < 7; steps++) {
            after = after->next;
            before = before->prev;
            if (after == D_00446C40) {
                for (steps++; steps < 7 && before != D_00446C44; steps++) {
                    before = before->prev;
                }
                break;
            }
            if (before == D_00446C44) {
                for (steps++; steps < 7 && after != D_00446C40; steps++) {
                    after = after->next;
                }
                break;
            }
        }
    } else {
        after = before = D_00446C40;
    }
    sprintf(D_00426498, "menu/select2/i%04d.mod", item_name_defs[D_005CF3B0[0]->unk0C].file_number);
    sprintf(D_00426538, "menu/select2/i%04d.mod", item_name_defs[D_005CF3B0[0]->prev->unk0C].file_number);
    sprintf(D_004264E8, "menu/select2/i%04d.mod", item_name_defs[D_005CF3B0[0]->next->unk0C].file_number);
    sprintf(D_00426588, "menu/select2/i%04d.mod", item_name_defs[after->unk0C].file_number);
    sprintf(D_004265D8, "menu/select2/i%04d.mod", item_name_defs[before->unk0C].file_number);
}

/* The smith's list of items that can be handed in: draws the screen parts, the cursor bar, the seven rows, the
   turning model and the selected item's panel, and handles the buttons and the fades. */
static void Smith_UpdateUpgradeList(HandlerContext *context) {
    TradeGroupBases bases = item_group_bases;
    MenuCursorBar bar;
    f32 rotation[16];
    f32 scale[16];
    GsPackedWord word;
    MenuItem *item;
    MenuItem *selected;
    SpritePacket *parts;
    u32 count;
    u32 i;
    s32 row;
    s32 number;
    s32 index;
    s32 table;
    s32 entry;
    s32 variant;
    f32 angle;
    u8 kind;
    u32 full_alpha;
    MenuItem *node;
    s32 line;
    s32 text_index;
    s32 depth;
    LevelDef *def;

    if (D_00426110[0] == NULL) {
        D_00446CC4 = Smith_UpdateSelectScreen;
        return;
    }
    if (D_00447316 < 2) {
        Menu_MoveItemListCursor(context, D_00447318);
    }
    number = D_00426110[0]->unk0C;
    index = number - bases.first_item[D_00426110[0]->group];
    if (D_00426110[0]->group == 0) {
        D_00446E3C = D_003679A0[index].unk14 + 1;
    }
    D_00446E30 = 100;
    if (D_00426110[0]->unk0C < 220) {
        D_00446E34 = GetWeaponLevel(D_00426110[0]->unk0C - 150);
    } else {
        D_00446E34 = GetItemLevel(D_00426110[0]->unk0C - 220);
    }
    for (i = 0; i < D_00446AA4; i++) {
        kind = D_00446C50[i].unk3E;
        SELECT_PART((s8)D_00446C50[i].unk42);
        if (kind == 3) {
            switch (D_00446C50[i].slot) {
            case 0:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 0, (int)D_00425510[i]);
                break;
            case 1:
                break;
            case 2:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 2, (int)D_00425510[i]);
                break;
            case 3:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 3, (int)D_00425510[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&D_00446C50[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }
    item = D_00426120[0];
    for (row = 0; row < 7; row++, item = item->prev) {
        if (item == D_00426110[0]) {
            depth = 0x86470;
            bar.first.f.lo16 = 0x6FE0;
            bar.first.f.mid16 = 0x7C30 + row * 0xB0;
            bar.first.f.mid24 = depth;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_00447316 == 1) {
                D_00425510[253] = Trade_PulseValue(0x10, 0x20, D_00425510[253]);
            }
            bar.alpha = (s32)D_00425510[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }
    MenuText_SetColor(D_00446C78);
    word.raw = 0x0C7A12007C307080;
    for (line = 0, node = D_00426120[0]; line < 7; node = node->prev, line++) {
        def = &item_name_defs[node->unk0C];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (D_00426130[0]->prev == node) {
            break;
        }
    }
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CD8 + 3.1415927f;
    D_00446CD8 = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY(rotation, scale, D_00446CD8);
    sceVu0TransMatrix(rotation, rotation, D_00428A30);
    Menu_DrawItemModel(rotation);
    if ((D_00446DE8 & 0x1000) && D_00447318 == 0 && func_001E4FE0(index, D_00426110[0]->group)) {
        D_00446CD4 = 1;
        if (D_00446AD4 == 0) {
            D_00446AD4 = 1;
        } else {
            D_00447317 = 2;
        }
    }
    if (D_00446AD4 != 0) {
        switch (D_00426110[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (D_00446CD4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CD4 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E24 = D_003682F0[index].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A9C; i++) {
                    full_alpha = D_00446C14[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A9C; i++) {
                SELECT_PART((s8)D_00446C14[i].unk42);
                MenuSprite_Draw((int)&D_00446C14[i], D_00446C14[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (D_00446CD4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CD4 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A94; i++) {
                    full_alpha = D_00446C0C[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A94; i++) {
                SELECT_PART((s8)D_00446C0C[i].unk42);
                MenuSprite_Draw((int)&D_00446C0C[i], D_00446C0C[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (D_00446CD4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CD4 = 0;
            D_00446DF4 = (D_003679A0[index].unk00[0] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[0]) * D_003679A0[index].level / 100;
            D_00446DF8 = (D_003679A0[index].unk00[1] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[1]) * D_003679A0[index].level / 100;
            D_00446DFC = (D_003679A0[index].unk00[2] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[2]) * D_003679A0[index].level / 100;
            D_00446E00 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[3] * D_003679A0[index].level / 100;
            D_00446E04 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[4] * D_003679A0[index].level / 100;
            D_00446E08 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[5] * D_003679A0[index].level / 100;
            D_00446E0C = weapon_defs[index].unk00[D_003679A0[index].unk14].values[6] * D_003679A0[index].level / 100;
            D_00446E10 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[7] * D_003679A0[index].level / 100;
            D_00446E14 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[8] * D_003679A0[index].level / 100;
            D_00446E20 = (s32)(weapon_defs[index].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[index].level;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_00426110[0]->group];
            D_00446E2C = 100;
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A98; i++) {
                    full_alpha = D_00446C10[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A98; i++) {
                SELECT_PART((s8)D_00446C10[i].unk42);
                MenuSprite_Draw((int)&D_00446C10[i], D_00446C10[i].unk3E + 1000, 0, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C10;
            count = D_00446A98;
            break;
        }
        switch (D_00447317) {
        case 0:
            for (i = 0; i < count; i++) {
                D_00425910[i] += parts[i].alpha / 10.0f;
                if (D_00425910[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        D_00425910[i] = full_alpha;
                    }
                    D_00447317 = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= parts[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    if (D_00447317 == 2) {
                        D_00446AD4 = 0;
                    }
                    D_00447317 = 0;
                    break;
                }
            }
            break;
        }
    }
    selected = D_00426110[0];
    table = Message_GetFontText(D_00446F38);
    text_index = selected->unk0C + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong(table + entry, 0x33);
    }
    if ((D_00446DE8 & 0x4000) && D_00447318 == 0) {
        if ((D_00426110[0]->unk0C < 220 ? GetWeaponLevel(D_00426110[0]->unk0C - 150) < 100
                                        : GetItemLevel(D_00426110[0]->unk0C - 220) < 100) ||
            D_00426110[0]->unk0C == 183) {
            Sound_PlayMenu(0);
            D_00447316 = 2;
            D_00447317 = 3;
            D_00447318 = 1;
        }
    } else if ((D_00446DE8 & 0x2000) && D_00447318 == 0) {
        Sound_PlayMenu(2);
        D_00447316 = 3;
        D_00447317 = 2;
        D_00447318 = 1;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_00447316 == 2 && D_00446C90 == NULL) {
        D_00447316 = 4;
    }
    if (D_00447316 == 3 && D_00446C90 == NULL) {
        D_00447316 = 5;
    }
    switch (D_00447316) {
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
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] += D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C50[i].alpha) {
                D_00447316 = 1;
                for (i = 0; i < D_00446AA4; i++) {
                    full_alpha = D_00446C50[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00447318 = 0;
                return;
            }
        }
        break;
    case 4:
    case 5:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[254] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C88.alpha = (s32)D_00425510[254];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] -= D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447316 == 4) {
                    context->unk04 = 1;
                    D_00446CC4 = Smith_UpdateUpgradeConfirm;
                    D_00447313 = 0;
                } else if (D_00447316 == 5) {
                    context->unk04 = 0;
                    D_00446CC4 = Smith_UpdateSelectScreen;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                }
                D_00447316 = 0;
                D_00447317 = 0;
                memset(D_00425510, 0, 0x400);
                return;
            }
        }
        break;
    }
}

/* The screen Smith_UpdateUpgradeList opens for the item chosen from its list: draws the parts, the list with the item's panel,
   its name and its text, waits for accept or cancel and fades out. On accept the item's record gets level 100, the
   play time of now and the flag that makes that time count (LevelDef.unk36 minutes, shown through D_00446E38); item
   183 is given up for item 184; and the item is taken off if it was equipped. Looks like leaving an item to be
   restored (a guess from what is written). */
static void Smith_UpdateUpgradeConfirm(HandlerContext *context) {
    TradeGroupBases bases = item_group_bases;
    f32 rotation[16];
    f32 scale[16];
    MenuCursorBar bar; // match: never used; the frame keeps its 0x60 bytes
    GsPackedWord word;
    MenuItem *item;
    MenuItem *selected;
    SpritePacket *parts;
    u32 count;
    u32 i;
    s32 row;
    s32 number;
    s32 index;
    s32 table;
    s32 entry;
    s32 variant;
    f32 angle;
    u8 kind;
    u32 full_alpha;
    s32 text_index;
    LevelDef *def;
    LevelDef *selected_def; // match: its own variable, so that both definitions get the short-lived register
    MenuItem *bought; // match: its own variable (no call while it is used, so it stays in a scratch register)

    D_00446E30 = 100;
    if (D_00426110[0]->unk0C < 220) {
        D_00446E34 = GetWeaponLevel(D_00426110[0]->unk0C - 150);
    } else {
        D_00446E34 = GetItemLevel(D_00426110[0]->unk0C - 220);
    }
    number = D_00426110[0]->unk0C;
    index = number - bases.first_item[D_00426110[0]->group];
    if (D_00426110[0]->group == 0) {
        D_00446E3C = D_003679A0[index].unk14 + 1;
    }
    for (i = 0; i < D_00446AA4; i++) {
        kind = D_00446C50[i].unk3E;
        SELECT_PART((s8)D_00446C50[i].unk42);
        if (kind == 3) {
            switch (D_00446C50[i].slot) {
            case 0:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 0, (int)D_00425510[i]);
                break;
            case 1:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 1, (int)D_00425510[i]);
                break;
            case 2:
                break;
            case 3:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 3, (int)D_00425510[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&D_00446C50[i], kind + 1000, 2, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }
    MenuText_SetColor(D_00446C78);
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CE0 + 3.1415927f;
    D_00446CE0 = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY(rotation, scale, D_00446CE0);
    sceVu0TransMatrix(rotation, rotation, D_00428A40);
    Menu_DrawItemModel(rotation);
    if ((D_00446DE8 & 0x1000) && D_00447318 == 0 && func_001E4FE0(index, D_00426110[0]->group)) {
        D_00446CDC = 1;
        if (D_00446AD4 == 0) {
            D_00446AD4 = 1;
        } else {
            D_00447317 = 2;
        }
    }
    if (D_00446AD4 != 0) {
        switch (D_00426110[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (D_00446CDC != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CDC = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E24 = D_003682F0[index].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A9C; i++) {
                    full_alpha = D_00446C14[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A9C; i++) {
                SELECT_PART((s8)D_00446C14[i].unk42);
                MenuSprite_Draw((int)&D_00446C14[i], D_00446C14[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (D_00446CDC != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CDC = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A94; i++) {
                    full_alpha = D_00446C0C[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A94; i++) {
                SELECT_PART((s8)D_00446C0C[i].unk42);
                // match: called as returning a value, which keeps $2 away from the pointer formed for the next call
                ((int (*)(int, int, int, int))MenuSprite_Draw)((int)&D_00446C0C[i], D_00446C0C[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART((s8)D_00446C0C[i].unk42); // match: the other lists hand 0 here
            }
            parts = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (D_00446CDC != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CDC = 0;
            D_00446DF4 = (D_003679A0[index].unk00[0] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[0]) * D_003679A0[index].level / 100;
            D_00446DF8 = (D_003679A0[index].unk00[1] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[1]) * D_003679A0[index].level / 100;
            D_00446DFC = (D_003679A0[index].unk00[2] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[2]) * D_003679A0[index].level / 100;
            D_00446E00 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[3] * D_003679A0[index].level / 100;
            D_00446E04 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[4] * D_003679A0[index].level / 100;
            D_00446E08 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[5] * D_003679A0[index].level / 100;
            D_00446E0C = weapon_defs[index].unk00[D_003679A0[index].unk14].values[6] * D_003679A0[index].level / 100;
            D_00446E10 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[7] * D_003679A0[index].level / 100;
            D_00446E14 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[8] * D_003679A0[index].level / 100;
            D_00446E20 = (s32)(weapon_defs[index].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[index].level;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_00426110[0]->group];
            D_00446E2C = 100;
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A98; i++) {
                    full_alpha = D_00446C10[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A98; i++) {
                SELECT_PART((s8)D_00446C10[i].unk42);
                MenuSprite_Draw((int)&D_00446C10[i], D_00446C10[i].unk3E + 1000, 0, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C10;
            count = D_00446A98;
            break;
        }
        switch (D_00447317) {
        case 0:
            for (i = 0; i < count; i++) {
                D_00425910[i] += parts[i].alpha / 10.0f;
                if (D_00425910[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        D_00425910[i] = full_alpha;
                    }
                    D_00447317 = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= parts[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    D_00447317 = 0;
                    D_00446AD4 = 0;
                    break;
                }
            }
            break;
        }
    }
    word.raw = 0x0C0975E07C307080;
    for (row = 0, item = D_00426120[0]; row < 7; item = item->prev, row++) {
        def = &item_name_defs[item->unk0C];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (D_00426130[0]->prev == item) {
            break;
        }
    }
    D_00446E38 = item_name_defs[D_00426110[0]->unk0C].unk36;
    for (i = 0; i < D_00446AB8; i++) {
        kind = D_00446C64[i].unk3E;
        SELECT_PART((s8)D_00446C64[i].unk42);
        MenuSprite_Draw((int)&D_00446C64[i], kind + 1000, 0, (int)D_00425D10[i]);
        SELECT_PART(0);
    }
    selected_def = &item_name_defs[D_00426110[0]->unk0C];
    MenuText_SetColor(D_00446C78);
    word.raw = 0x0A2DC6C07F5078C0;
    MenuText_SetPen(word);
    MenuText_DrawLineCentered(selected_def, 0x18);
    selected = D_00426110[0];
    table = Message_GetFontText(D_00446F38);
    text_index = selected->unk0C + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong((u16 *)(table + entry), 0x33);
    }
    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            Sound_PlayMenu(0);
            D_00447316 = 2;
            D_00447317 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
            D_00447316 = 3;
            D_00447317 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        }
    }
    if (D_00447316 == 2 && D_00446C90 == NULL) {
        D_00447316 = 4;
    }
    if (D_00447316 == 3 && D_00446C90 == NULL) {
        D_00447316 = 5;
    }
    switch (D_00447316) {
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
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] += D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C50[i].alpha) {
                D_00447316 = 1;
                D_00447318 = 0;
                for (i = 0; i < D_00446AA4; i++) {
                    full_alpha = D_00446C50[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                break;
            }
        }
        for (i = 0; i < D_00446AB8; i++) {
            D_00425D10[i] += D_00446C64[i].alpha / 10.0f;
            if (D_00425D10[i] > D_00446C64[i].alpha) {
                for (i = 0; i < D_00446AB8; i++) {
                    full_alpha = D_00446C64[i].alpha;
                    D_00425D10[i] = full_alpha;
                }
                break;
            }
        }
        break;
    case 4:
    case 5:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[254] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C88.alpha = (s32)D_00425510[254];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] -= D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447316 == 4) {
                    func_001D20E0();
                    bought = D_00426110[0];
                    if (bought->unk0C < 220) {
                        if (bought->unk0C == ITEM_BROKEN_KATANA) {
                            item_counts[ITEM_BROKEN_KATANA] = 0;
                            item_counts[bought->unk0C + 1] = 1;
                            // match: "- 150 + 1" wraps the constant offset around, which keeps the compiler from
                            // forming the table's address out of this one (it does for "- 149")
                            *(ClockTime *)D_003679A0[bought->unk0C - 150 + 1].unk19.bytes = D_00446A28;
                            GetWeaponRecord(bought->unk0C - 149)->level = 100;
                            GetWeaponRecord(bought->unk0C - 149)->unk18 = 1;
                        } else {
                            *(ClockTime *)D_003679A0[bought->unk0C - 150].unk19.bytes = D_00446A28;
                            GetWeaponRecord(bought->unk0C - 150)->level = 100;
                            GetWeaponRecord(bought->unk0C - 150)->unk18 = 1;
                        }
                    } else {
                        *(ClockTime *)D_003682F0[bought->unk0C - 220].unk19.bytes = D_00446A28;
                        GetItemRecord(bought->unk0C - 220)->level = 100;
                        GetItemRecord(bought->unk0C - 220)->unk18 = 1;
                    }
                    if (D_00426110[0]->unk0C == D_00413F80.weapon + ITEM_WEAPON_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_HELM] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_RING] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.shortcut) {
                        func_001E54A8(D_00426110[0]->group);
                    }
                    context->unk04 = 0;
                    D_00446CC4 = Smith_UpdateSelectScreen;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else if (D_00447316 == 5) {
                    D_00446CC4 = (void (*)(HandlerContext *))smith_list_handlers[0];
                }
                D_00447316 = 0;
                D_00447317 = 0;
                memset(D_00425510, 0, 0x400);
                break;
            }
        }
        for (i = 0; i < D_00446AB8; i++) {
            D_00425D10[i] -= D_00446C64[i].alpha / 10.0f;
            if (D_00425D10[i] < 0.0f) {
                memset(D_00425D10, 0, 0x400);
                break;
            }
        }
        break;
    }
}

/* The smith's item list screen with its confirmation window, a twin of Smith_UpdateUpgradeList: draws the screen parts, the
   cursor bar, the seven rows, the turning model, the selected item's panel and, with D_00447313 set, the window that
   asks to confirm. The 0x4000 button confirms and 0x2000 cancels; when the fade-out ends, state 6 goes on to
   Smith_UpdateItemScreen, 7 stamps the item's record with the play time and counts the chosen upgrade, 8 goes back to
   Smith_UpdateSelectScreen and 9 only closes the window. */
static void Smith_UpdateListWithConfirm(HandlerContext *context) {
    TradeGroupBases bases = item_group_bases;
    MenuCursorBar bar;
    f32 rotation[16];
    f32 scale[16];
    GsPackedWord word;
    MenuItem *item;
    MenuItem *selected;
    SpritePacket *parts;
    u32 count;
    u32 i;
    s32 row;
    s32 number;
    s32 index;
    s32 table;
    s32 entry;
    s32 variant;
    f32 angle;
    u8 kind;
    u32 full_alpha;
    MenuItem *node;
    s32 line;
    s32 text_index;
    LevelDef *def;
    LevelDef *chosen;
    GsPackedWord corner;

    if (D_00426110[0] == NULL) {
        D_00446CC4 = Smith_UpdateSelectScreen;
        return;
    }
    if (D_00447313 == 0 && D_00447316 < 2) {
        Menu_MoveItemListCursor(context, D_00447318);
    }
    number = D_00426110[0]->unk0C;
    index = number - bases.first_item[D_00426110[0]->group];
    if (D_00426110[0]->group == 0) {
        D_00446E3C = D_003679A0[index].unk14 + 1;
    }
    D_00446E30 = 100;
    if (D_00426110[0]->unk0C < 220) {
        D_00446E34 = GetWeaponLevel(D_00426110[0]->unk0C - 150);
    } else {
        D_00446E34 = GetItemLevel(D_00426110[0]->unk0C - 220);
    }
    for (i = 0; i < D_00446AA4; i++) {
        kind = D_00446C50[i].unk3E;
        SELECT_PART((s8)D_00446C50[i].unk42);
        if (kind == 3) {
            switch (D_00446C50[i].slot) {
            case 0:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 0, (int)D_00425510[i]);
                break;
            case 1:
                if (D_00447313 != 0) {
                    MenuSprite_Draw((int)&D_00446C50[i], 1003, 1, (int)D_00425510[i]);
                }
                break;
            case 2:
                if (D_00447313 == 0) {
                    MenuSprite_Draw((int)&D_00446C50[i], 1003, 2, (int)D_00425510[i]);
                }
                break;
            case 3:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 3, (int)D_00425510[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&D_00446C50[i], kind + 1000, 3, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }
    if (D_00447313 == 0) {
        // match: the bar's corner is a packed word set in front of the loop (as in Menu_UpdateVisibleItem), not literals in
        // the loop body: its three fields then leave the loop in the first loop pass and 0xFF only in the second.
        // The word's top byte is not used and cannot be told from the code.
        corner.raw = 0x000864707C306FE0;
        item = D_00426120[0];
        for (row = 0; row < 7; row++, item = item->prev) {
            if (item == D_00426110[0]) {
                bar.first.f.lo16 = corner.f.lo16;
                bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
                bar.first.f.mid24 = corner.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
                bar.second.f.mid24 = bar.first.f.mid24;
                if (D_00447316 == 1) {
                    if (D_00447313 == 0) {
                        D_00425510[253] = Trade_PulseValue(0x10, 0x20, D_00425510[253]);
                    } else { // match: never taken, the loop only runs with D_00447313 clear
                        D_00425510[253] = 80.0f;
                    }
                }
                bar.alpha = (s32)D_00425510[253];
                MenuDraw_Bar(&bar);
                break;
            }
        }
    }
    MenuText_SetColor(D_00446C78);
    word.raw = 0x0C0975E07C307080;
    for (line = 0, node = D_00426120[0]; line < 7; node = node->prev, line++) {
        def = &item_name_defs[node->unk0C];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (D_00426130[0]->prev == node) {
            break;
        }
    }
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CE8 + 3.1415927f;
    D_00446CE8 = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY(rotation, scale, D_00446CE8);
    sceVu0TransMatrix(rotation, rotation, D_00428A50);
    Menu_DrawItemModel(rotation);
    if ((D_00446DE8 & 0x1000) && D_00447318 == 0 && func_001E4FE0(index, D_00426110[0]->group)) {
        if (D_00446AD4 == 0) {
            D_00446CE4 = 1;
            D_00446AD4 = 1;
        } else if (D_00447317 != 2) {
            D_00447317 = 2;
            Sound_PlayMenu(1);
        } else {
            D_00447317 = 0;
        }
    }
    if (D_00446AD4 != 0) {
        switch (D_00426110[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (D_00446CE4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CE4 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E24 = D_003682F0[index].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A9C; i++) {
                    full_alpha = D_00446C14[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A9C; i++) {
                SELECT_PART((s8)D_00446C14[i].unk42);
                MenuSprite_Draw((int)&D_00446C14[i], D_00446C14[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (D_00446CE4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CE4 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A94; i++) {
                    full_alpha = D_00446C0C[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A94; i++) {
                SELECT_PART((s8)D_00446C0C[i].unk42);
                MenuSprite_Draw((int)&D_00446C0C[i], D_00446C0C[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (D_00446CE4 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CE4 = 0;
            D_00446DF4 = (D_003679A0[index].unk00[0] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[0]) * D_003679A0[index].level / 100;
            D_00446DF8 = (D_003679A0[index].unk00[1] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[1]) * D_003679A0[index].level / 100;
            D_00446DFC = (D_003679A0[index].unk00[2] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[2]) * D_003679A0[index].level / 100;
            D_00446E00 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[3] * D_003679A0[index].level / 100;
            D_00446E04 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[4] * D_003679A0[index].level / 100;
            D_00446E08 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[5] * D_003679A0[index].level / 100;
            D_00446E0C = weapon_defs[index].unk00[D_003679A0[index].unk14].values[6] * D_003679A0[index].level / 100;
            D_00446E10 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[7] * D_003679A0[index].level / 100;
            D_00446E14 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[8] * D_003679A0[index].level / 100;
            D_00446E20 = (s32)(weapon_defs[index].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[index].level;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_00426110[0]->group];
            D_00446E2C = 100;
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A98; i++) {
                    full_alpha = D_00446C10[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A98; i++) {
                SELECT_PART((s8)D_00446C10[i].unk42);
                MenuSprite_Draw((int)&D_00446C10[i], D_00446C10[i].unk3E + 1000, 0, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C10;
            count = D_00446A98;
            break;
        }
        switch (D_00447317) {
        case 0:
            for (i = 0; i < count; i++) {
                D_00425910[i] += parts[i].alpha / 10.0f;
                if (D_00425910[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        D_00425910[i] = full_alpha;
                    }
                    D_00447317 = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= parts[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    D_00447317 = 0;
                    D_00446AD4 = 0;
                    break;
                }
            }
            break;
        }
    }
    if (D_00447313 != 0) {
        D_00446E38 = item_name_defs[D_00426110[0]->unk0C].unk36;
        for (i = 0; i < D_00446AB8; i++) {
            kind = D_00446C64[i].unk3E;
            SELECT_PART((s8)D_00446C64[i].unk42);
            MenuSprite_Draw((int)&D_00446C64[i], kind + 1000, 0, (int)D_00425D10[i]);
            SELECT_PART(0);
        }
        chosen = &item_name_defs[D_00426110[0]->unk0C];
        MenuText_SetColor(D_00446C78);
        word.raw = 0x0A2DC6C07F5078C0;
        MenuText_SetPen(word);
        MenuText_DrawLineCentered((u16 *)chosen, 0x18);
    }
    selected = D_00426110[0];
    table = Message_GetFontText(D_00446F38);
    text_index = selected->unk0C + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong((u16 *)(table + entry), 0x33);
    }
    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            if (D_00447313 == 0) {
                Sound_PlayMenu(0);
                D_00447316 = 2;
                D_00447318 = 1;
            } else {
                Sound_PlayMenu(0);
                D_00447316 = 3;
                D_00447318 = 1;
                if (D_00446C90 != NULL) {
                    D_00446C90->next = NULL;
                }
            }
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
            if (D_00447313 == 0) {
                D_00447316 = 4;
                D_00447318 = 1;
                if (D_00446C90 != NULL) {
                    D_00446C90->next = NULL;
                }
            } else {
                D_00447316 = 5;
                D_00447318 = 1;
                if (D_00446C90 != NULL) {
                    D_00446C90->next = NULL;
                }
            }
        }
    }
    if (D_00447316 == 2 && D_00446C90 == NULL) {
        D_00447316 = 6;
    }
    if (D_00447316 == 3 && D_00446C90 == NULL) {
        D_00447316 = 7;
    }
    if (D_00447316 == 4 && D_00446C90 == NULL) {
        D_00447316 = 8;
    }
    if (D_00447316 == 5 && D_00446C90 == NULL) {
        D_00447316 = 9;
    }
    switch (D_00447316) {
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
        D_00425510[252] += 1.6f;
        if (D_00425510[252] > 16.0f) {
            D_00425510[252] = 16.0f;
        }
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] += D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C50[i].alpha) {
                D_00447318 = 0;
                D_00447316 = 1;
                for (i = 0; i < D_00446AA4; i++) {
                    full_alpha = D_00446C50[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                break;
            }
        }
        for (i = 0; i < D_00446AB8; i++) {
            D_00425D10[i] += D_00446C64[i].alpha / 10.0f;
            if (D_00425D10[i] > D_00446C64[i].alpha) {
                for (i = 0; i < D_00446AB8; i++) {
                    full_alpha = D_00446C64[i].alpha;
                    D_00425D10[i] = full_alpha;
                }
                break;
            }
        }
        break;
    case 6:
    case 7:
    case 8:
    case 9:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[254] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C88.alpha = (s32)D_00425510[254];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        D_00425510[252] -= 1.6f;
        if (D_00425510[252] < 0.0f) {
            D_00425510[252] = 0.0f;
        }
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] -= D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447316 == 6) {
                    D_00446CC4 = Smith_UpdateItemScreen;
                    context->unk04 = 1;
                    D_00447315 = 0;
                } else if (D_00447316 == 7) {
                    func_001D20E0();
                    if (D_00426110[0]->unk0C < 220) {
                        *(ClockTime *)D_003679A0[D_00426110[0]->unk0C - 150].unk19.bytes = D_00446A28;
                        switch (D_00447315) {
                        case 0:
                            D_003679A0[D_00426110[0]->unk0C - 150].unk00[0]++;
                            D_003679A0[D_00426110[0]->unk0C - 150].unk18 = 1;
                            break;
                        case 1:
                            D_003679A0[D_00426110[0]->unk0C - 150].unk00[1]++;
                            D_003679A0[D_00426110[0]->unk0C - 150].unk18 = 1;
                            break;
                        case 2:
                            D_003679A0[D_00426110[0]->unk0C - 150].unk00[2]++;
                            D_003679A0[D_00426110[0]->unk0C - 150].unk18 = 1;
                            break;
                        }
                    } else {
                        *(ClockTime *)D_003682F0[D_00426110[0]->unk0C - 220].unk19.bytes = D_00446A28;
                        switch (D_00447315) {
                        case 0:
                            D_003682F0[D_00426110[0]->unk0C - 220].unk00[0]++;
                            D_003682F0[D_00426110[0]->unk0C - 220].unk18 = 1;
                            break;
                        case 1:
                            D_003682F0[D_00426110[0]->unk0C - 220].unk00[1]++;
                            D_003682F0[D_00426110[0]->unk0C - 220].unk18 = 1;
                            break;
                        case 2:
                            D_003682F0[D_00426110[0]->unk0C - 220].unk00[2]++;
                            D_003682F0[D_00426110[0]->unk0C - 220].unk18 = 1;
                            break;
                        }
                    }
                    if (D_00426110[0]->unk0C == D_00413F80.weapon + ITEM_WEAPON_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_HELM] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_RING] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST ||
                        D_00426110[0]->unk0C == D_00413F80.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST || D_00426110[0]->unk0C == D_00413F80.shortcut) {
                        func_001E54A8(D_00426110[0]->group);
                    }
                    D_00446CC4 = Smith_UpdateSelectScreen;
                    item_counts[ITEM_EARTH_STONE]--;
                    context->unk04 = 1;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else if (D_00447316 == 8) {
                    context->unk04 = 1;
                    D_00446CC4 = Smith_UpdateSelectScreen;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else if (D_00447316 == 9) {
                    D_00447313 = 0;
                }
                D_00447314 = 0;
                D_00447316 = 0;
                D_00447317 = 0;
                memset(D_00425510, 0, 0x400);
                break;
            }
        }
        for (i = 0; i < D_00446AB8; i++) {
            D_00425D10[i] -= D_00446C64[i].alpha / 10.0f;
            if (D_00425D10[i] < 0.0f) {
                memset(D_00425D10, 0, 0x400);
                break;
            }
        }
        break;
    }
}

/* The three-line screen shown for the selected item (a sibling of Smith_UpdateUpgradeList without the item list): moves the
   cursor over its three lines, draws the parts, the turning model and the item's panel of numbers, and fades in and
   out; leaving either way hands over to smith_list_handlers[1]. */
static void Smith_UpdateItemScreen(HandlerContext *context) {
    TradeGroupBases bases = item_group_bases;
    MenuCursorBar bar;
    f32 rotation[16];
    f32 scale[16];
    GsPackedWord word;
    s32 play_sound = FALSE;
    s32 bar_drawn;
    MenuItem *selected;
    SpritePacket *parts;
    u32 count;
    u32 i;
    s32 line_count;
    s32 line_offset;
    s32 number;
    s32 index;
    s32 table;
    s32 entry;
    s32 variant;
    f32 angle;
    u8 kind;
    u32 full_alpha;
    s32 text_index;

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x40) {
            Sound_PlayMenu(1);
            D_00447315++;
        } else if (D_00446DE8 & 0x10) {
            Sound_PlayMenu(1);
            D_00447315--;
        }
    }
    line_count = 3;
    D_00447315 = (s8)(D_00447315 + 3) % line_count;
    D_00446E30 = 100;
    if (D_00426110[0]->unk0C < 220) {
        D_00446E34 = GetWeaponLevel(D_00426110[0]->unk0C - 150);
    } else {
        D_00446E34 = GetItemLevel(D_00426110[0]->unk0C - 220);
    }
    number = D_00426110[0]->unk0C;
    index = number - bases.first_item[D_00426110[0]->group];
    if (D_00426110[0]->group == 0) {
        D_00446E3C = D_003679A0[index].unk14 + 1;
    }
    bar_drawn = FALSE;
    for (i = 0; i < D_00446AA8; i++) {
        if (!bar_drawn && D_00446C54[i].unk43 == 1) {
            word.raw = 0x0E0864707DF07490L;
            bar_drawn = TRUE;
            bar.first.f.lo16 = word.f.lo16;
            line_offset = D_00447315 * 0xB0; // match: kept in a variable, or the sums below fold to constants
            bar.first.f.mid16 = word.f.mid16 + line_offset;
            if (D_00447315 == 1) {
                bar.first.f.mid16 = word.f.mid16 + line_offset + 0x30;
            } else if (D_00447315 == 2) {
                bar.first.f.mid16 = word.f.mid16 + line_offset + 0x60;
            }
            bar.first.f.mid24 = word.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x8B0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_00447316 == 1) {
                D_00425510[253] = Trade_PulseValue(0x10, 0x20, D_00425510[253]);
            }
            bar.alpha = (s32)D_00425510[253];
            MenuDraw_Bar(&bar);
        }
        kind = D_00446C54[i].unk3E;
        SELECT_PART((s8)D_00446C54[i].unk42);
        MenuSprite_Draw((int)&D_00446C54[i], kind + 1000, 0, (int)D_00425510[i]);
        SELECT_PART(0);
    }
    MenuText_SetColor(D_00446C78);
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CEC + 3.1415927f;
    D_00446CEC = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY(rotation, scale, D_00446CEC);
    sceVu0TransMatrix(rotation, rotation, D_00428A60);
    Menu_DrawItemModel(rotation);
    if ((D_00446DE8 & 0x1000) && D_00447318 == 0 && func_001E4FE0(index, D_00426110[0]->group)) {
        if (D_00446AD4 == 0) {
            play_sound = TRUE;
            D_00446AD4 = 1;
        } else if (D_00447317 != 2) { // match: this way round for the branch sense
            D_00447317 = 2;
            Sound_PlayMenu(1);
        } else {
            D_00447317 = 0;
        }
    }
    if (D_00446AD4 != 0) {
        switch (D_00426110[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (play_sound) {
                Sound_PlayMenu(1);
            }
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E24 = D_003682F0[index].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            for (i = 0; i < D_00446A9C; i++) {
                SELECT_PART((s8)D_00446C14[i].unk42);
                MenuSprite_Draw((int)&D_00446C14[i], D_00446C14[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (play_sound) {
                Sound_PlayMenu(1);
            }
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            for (i = 0; i < D_00446A94; i++) {
                SELECT_PART((s8)D_00446C0C[i].unk42);
                MenuSprite_Draw((int)&D_00446C0C[i], D_00446C0C[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (play_sound) {
                Sound_PlayMenu(1);
            }
            D_00446DF4 = (D_003679A0[index].unk00[0] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[0]) * D_003679A0[index].level / 100;
            D_00446DF8 = (D_003679A0[index].unk00[1] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[1]) * D_003679A0[index].level / 100;
            D_00446DFC = (D_003679A0[index].unk00[2] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[2]) * D_003679A0[index].level / 100;
            D_00446E00 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[3] * D_003679A0[index].level / 100;
            D_00446E04 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[4] * D_003679A0[index].level / 100;
            D_00446E08 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[5] * D_003679A0[index].level / 100;
            D_00446E0C = weapon_defs[index].unk00[D_003679A0[index].unk14].values[6] * D_003679A0[index].level / 100;
            D_00446E10 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[7] * D_003679A0[index].level / 100;
            D_00446E14 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[8] * D_003679A0[index].level / 100;
            D_00446E20 = (s32)(weapon_defs[index].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[index].level;
            D_00446E1C = item_name_defs[D_00426110[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_00426110[0]->group];
            D_00446E2C = 100;
            for (i = 0; i < D_00446A98; i++) {
                SELECT_PART((s8)D_00446C10[i].unk42);
                MenuSprite_Draw((int)&D_00446C10[i], D_00446C10[i].unk3E + 1000, 0, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C10;
            count = D_00446A98;
            break;
        }
        switch (D_00447317) {
        case 0:
            for (i = 0; i < count; i++) {
                D_00425910[i] += parts[i].alpha / 10.0f;
                if (D_00425910[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        D_00425910[i] = full_alpha;
                    }
                    D_00447317 = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= parts[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    D_00447317 = 0;
                    D_00446AD4 = 0;
                    break;
                }
            }
            break;
        }
    }
    selected = D_00426110[0];
    table = Message_GetFontText(D_00446F38);
    text_index = selected->unk0C + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080L;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong(table + entry, 0x33);
    }
    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            Sound_PlayMenu(0);
            D_00447316 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
            D_00447316 = 3;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        }
    }
    if (D_00447316 == 2 && D_00446C90 == NULL) {
        D_00447316 = 4;
    }
    if (D_00447316 == 3 && D_00446C90 == NULL) {
        D_00447316 = 5;
    }
    switch (D_00447316) {
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
        for (i = 0; i < D_00446AA8; i++) {
            D_00425510[i] += D_00446C54[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C54[i].alpha) {
                D_00447316 = 1;
                D_00447318 = 0;
                for (i = 0; i < D_00446AA8; i++) {
                    full_alpha = D_00446C54[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                return;
            }
        }
        break;
    case 4:
    case 5:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[254] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C88.alpha = (s32)D_00425510[254];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446AA8; i++) {
            D_00425510[i] -= D_00446C54[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447316 == 4) {
                    D_00447313 = 1;
                    D_00446CC4 = smith_list_handlers[1];
                } else if (D_00447316 == 5) {
                    D_00447313 = 0;
                    D_00446CC4 = smith_list_handlers[1];
                }
                D_00447316 = 0;
                D_00447317 = 0;
                memset(D_00425510, 0, 0x400);
                return;
            }
        }
        break;
    }
}

/* The item list screen over the D_005CF3B0 ring, a twin of Smith_UpdateUpgradeList: draws the seven rows, the turning model
   and the selected item's panel, and fades the screen in and out. Leaving with the 0x4000 button clears the
   chosen item's LevelRecord.unk18 before going back to Smith_UpdateSelectScreen. */
static void Smith_UpdateCollectList(HandlerContext *context) {
    TradeGroupBases bases = item_group_bases;
    MenuCursorBar bar;
    f32 rotation[16];
    f32 scale[16];
    GsPackedWord word;
    MenuItem *item;
    MenuItem *selected;
    SpritePacket *parts;
    u32 count;
    u32 i;
    s32 row;
    s32 number;
    s32 index;
    s32 table;
    s32 entry;
    s32 variant;
    f32 angle;
    u8 kind;
    u32 full_alpha;
    MenuItem *node;
    s32 line;
    s32 text_index;
    s32 depth;
    LevelDef *def;

    if (D_005CF3B0[0] == NULL) {
        D_00446CC4 = Smith_UpdateSelectScreen;
        return;
    }
    if (D_00447316 < 2) {
        Smith_MoveListCursor(context);
    }
    number = D_005CF3B0[0]->unk0C;
    index = number - bases.first_item[D_005CF3B0[0]->group];
    if (D_005CF3B0[0]->group == 0) {
        D_00446E3C = D_003679A0[index].unk14 + 1;
    }
    D_00446E30 = 100;
    if (D_005CF3B0[0]->unk0C < 220) {
        D_00446E34 = GetWeaponLevel(D_005CF3B0[0]->unk0C - 150);
    } else {
        D_00446E34 = GetItemLevel(D_005CF3B0[0]->unk0C - 220);
    }
    for (i = 0; i < D_00446AA4; i++) {
        kind = D_00446C50[i].unk3E;
        SELECT_PART((s8)D_00446C50[i].unk42);
        if (kind == 3) {
            switch (D_00446C50[i].slot) {
            case 0:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 0, (int)D_00425510[i]);
                break;
            case 1:
                break;
            case 2:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 2, (int)D_00425510[i]);
                break;
            case 3:
                MenuSprite_Draw((int)&D_00446C50[i], 1003, 3, (int)D_00425510[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&D_00446C50[i], kind + 1000, 4, (int)D_00425510[i]);
        }
        SELECT_PART(0);
    }
    item = D_005CF390[0];
    for (row = 0; row < 7; row++, item = item->prev) {
        if (item == D_005CF3B0[0]) {
            depth = 0x86470;
            bar.first.f.lo16 = 0x6FE0;
            bar.first.f.mid16 = 0x7C30 + row * 0xB0;
            bar.first.f.mid24 = depth;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_00447316 == 1) {
                D_00425510[253] = Trade_PulseValue(0x10, 0x20, D_00425510[253]);
            }
            bar.alpha = (s32)D_00425510[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }
    MenuText_SetColor(D_00446C78);
    word.raw = 0x0C7A12007C307080;
    for (line = 0, node = D_005CF390[0]; line < 7; node = node->prev, line++) {
        def = &item_name_defs[node->unk0C];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (D_005CF3A0[0]->prev == node) {
            break;
        }
    }
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CF4 + 3.1415927f;
    D_00446CF4 = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY(rotation, scale, D_00446CF4);
    sceVu0TransMatrix(rotation, rotation, D_00428A70);
    Menu_DrawItemModel(rotation);
    if ((D_00446DE8 & 0x1000) && D_00447318 == 0 && func_001E4FE0(index, D_005CF3B0[0]->group)) {
        D_00446CF0 = 1;
        if (D_00446AD4 == 0) {
            D_00446AD4 = 1;
        } else {
            D_00447317 = 2;
        }
    }
    if (D_00446AD4 != 0) {
        switch (D_005CF3B0[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (D_00446CF0 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CF0 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E24 = D_003682F0[index].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_005CF3B0[0]->unk0C].unk3E;
            variant = D_00413FBC[D_005CF3B0[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A9C; i++) {
                    full_alpha = D_00446C14[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A9C; i++) {
                SELECT_PART((s8)D_00446C14[i].unk42);
                MenuSprite_Draw((int)&D_00446C14[i], D_00446C14[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (D_00446CF0 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CF0 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + worn_defs[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + worn_defs[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + worn_defs[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E1C = item_name_defs[D_005CF3B0[0]->unk0C].unk3E;
            variant = D_00413FBC[D_005CF3B0[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A94; i++) {
                    full_alpha = D_00446C0C[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A94; i++) {
                SELECT_PART((s8)D_00446C0C[i].unk42);
                MenuSprite_Draw((int)&D_00446C0C[i], D_00446C0C[i].unk3E + 1000, variant, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (D_00446CF0 != 0) {
                Sound_PlayMenu(1);
            }
            D_00446CF0 = 0;
            D_00446DF4 = (D_003679A0[index].unk00[0] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[0]) * D_003679A0[index].level / 100;
            D_00446DF8 = (D_003679A0[index].unk00[1] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[1]) * D_003679A0[index].level / 100;
            D_00446DFC = (D_003679A0[index].unk00[2] + weapon_defs[index].unk00[D_003679A0[index].unk14].values[2]) * D_003679A0[index].level / 100;
            D_00446E00 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[3] * D_003679A0[index].level / 100;
            D_00446E04 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[4] * D_003679A0[index].level / 100;
            D_00446E08 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[5] * D_003679A0[index].level / 100;
            D_00446E0C = weapon_defs[index].unk00[D_003679A0[index].unk14].values[6] * D_003679A0[index].level / 100;
            D_00446E10 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[7] * D_003679A0[index].level / 100;
            D_00446E14 = weapon_defs[index].unk00[D_003679A0[index].unk14].values[8] * D_003679A0[index].level / 100;
            D_00446E20 = (s32)(weapon_defs[index].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[index].level;
            D_00446E1C = item_name_defs[D_005CF3B0[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_005CF3B0[0]->group];
            D_00446E2C = 100;
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A98; i++) {
                    full_alpha = D_00446C10[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A98; i++) {
                SELECT_PART((s8)D_00446C10[i].unk42);
                MenuSprite_Draw((int)&D_00446C10[i], D_00446C10[i].unk3E + 1000, 0, (int)D_00425910[i]);
                SELECT_PART(0);
            }
            parts = D_00446C10;
            count = D_00446A98;
            break;
        }
        switch (D_00447317) {
        case 0:
            for (i = 0; i < count; i++) {
                D_00425910[i] += parts[i].alpha / 10.0f;
                if (D_00425910[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        D_00425910[i] = full_alpha;
                    }
                    D_00447317 = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= parts[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    D_00446AD4 = 0;
                    D_00447317 = 0;
                    break;
                }
            }
            break;
        }
    }
    selected = D_005CF3B0[0];
    table = Message_GetFontText(D_00446F38);
    text_index = selected->unk0C + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong(table + entry, 0x33);
    }
    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            Sound_PlayMenu(0);
            D_00447316 = 2;
            D_00447317 = 3;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else if (D_00446DE8 & 0x2000) {
            Sound_PlayMenu(2);
            D_00447316 = 3;
            D_00447317 = 2;
            D_00447318 = 1;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        }
    }
    if (D_00447316 == 2 && D_00446C90 == NULL) {
        D_00447316 = 4;
    }
    if (D_00447316 == 3 && D_00446C90 == NULL) {
        D_00447316 = 5;
    }
    switch (D_00447316) {
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
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] += D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C50[i].alpha) {
                D_00447316 = 1;
                D_00447318 = 0;
                for (i = 0; i < D_00446AA4; i++) {
                    full_alpha = D_00446C50[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                return;
            }
        }
        break;
    case 4:
    case 5:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[254] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C88.alpha = (s32)D_00425510[254];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] -= D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447316 == 4) {
                    if (D_005CF3B0[0]->unk0C < 220) {
                        ClearWeaponMark(D_005CF3B0[0]->unk0C - 150);
                    } else {
                        ClearItemMark(D_005CF3B0[0]->unk0C - 220);
                    }
                    context->unk04 = 2;
                    D_00446CC4 = Smith_UpdateSelectScreen;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else if (D_00447316 == 5) {
                    context->unk04 = 2;
                    D_00446CC4 = Smith_UpdateSelectScreen;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                }
                D_00447316 = 0;
                D_00447317 = 0;
                memset(D_00425510, 0, 0x400);
                return;
            }
        }
        break;
    }
}

/* Trade_MoveListCursor for the D_005CF3B0 list: moves the cursor one row (0x10, 0x40) or one page (0x20, 0x80, lists of 8
   and more), scrolling the window, then renames the link nodes' models. TRUE when a button was handled. */
static s32 Smith_MoveListCursor(HandlerContext *context) {
    MenuItem *cursor = D_005CF3B0[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if (D_00446DE8 & 0x10) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == D_005CF390[0] && next != D_005CF3A0[0]->prev) {
                    D_005CF390[0] = next;
                    D_005CF3A0[0] = D_005CF3A0[0]->next;
                }
                Sound_PlayMenu(1);
                D_005CF3B0[0] = cursor->next;
            }
            Menu_ShiftListModelsNext(0);
        } else if (D_00446DE8 & 0x40) {
            if (cursor->prev != NULL) {
                if (D_005CF3A0[0]->prev == cursor && D_005CF390[0]->next != cursor) {
                    D_005CF3A0[0] = cursor;
                    D_005CF390[0] = D_005CF390[0]->prev;
                }
                Sound_PlayMenu(1);
                D_005CF3B0[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((D_00446DE8 & 0x20) && D_00446C48 >= 8) {
            if (cursor->prev != NULL) {
                if (D_005CF390[0]->next != D_005CF3A0[0]->prev) {
                    rows = 0;
                    while (func_001F67C8_ScrollUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageUp(0);
        } else if ((D_00446DE8 & 0x80) && D_00446C48 >= 8) {
            if (cursor->next != NULL) {
                if (D_005CF390[0]->next != D_005CF3A0[0]->prev) {
                    rows = 0;
                    while (func_001F67C8_ScrollDown()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageDown(0);
        } else {
            return FALSE;
        }
        Smith_SetModelNames();
        return TRUE;
    }
    return FALSE;
}

/* the same for the save screen, which has no lists */
static int (*const save_screen_run_steps[2])(int) IN_RODATA = {
    (int (*)(int))SaveScreen_DrawBackground,
    SaveScreen_RunStep,
};
static int (*const save_screen_steps[4])(HandlerContext *) IN_RODATA = {
    SaveScreen_Start,
    (int (*)(HandlerContext *))SaveScreen_RunHandler,
    (int (*)(HandlerContext *))SaveScreen_CutModelChain,
    NULL,
};

/* Runs the current mode handler every frame until it reports done, then tears the mode down. */
void SaveScreen_Run(void) {
    int field;

    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    Vram_Clear(0x3D00, 0x3F00);
    Sound_MutePositional();
    Sound_UpdateTagged();
    D_00446C94 = 0;
    D_0044731C = D_00447320 = Pad_GetMode(0, 0);
    while (save_screen_run_steps[D_00446D00](0) != 0) {
        sceGsSyncPath(0, 0);
        D_00447328 = Menu_EndFrame(1);
        func_001B5460();
        field = Pad_GetMode(0, 0);
        D_0044731C = field;
        if (field != D_00447320) {
            D_0044730C = 1;
        }
        D_00447320 = field;
        MenuPad_Update();
        if (D_0044730C > 0) {
            if (++D_0044730C >= 0x15) {
                D_0044730C = 0;
            } else {
                D_00446DE8 = 0;
            }
        }
    }
    sceGsSyncPath(0, 0);
    while (sceGsSyncV(0) != 0) {
    }
    Sound_UnmutePositional();
    MenuHeap_FreeToMark();
    D_00446CFC = 0;
    D_00446D00 = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}

/* Fills the GIF packet that draws the screen background and sends it over DMA channel 2. */
static s32 SaveScreen_DrawBackground(void) {
    char reserved[0x70]; /* unused, but retail reserves the 0x70 bytes */
    GsFillPacket *packet;
    u32 flag;

    packet = MenuDraw_NextPacket();
    flag = D_00446F78 != 0 ? 0x1180 : 0;
    packet->unk04 = 0x100B4000;
    packet->unk08 = 0xE;
    packet->unk0C = 0;
    packet->unk10 = (D_004261A0[0] & ~0x1FFLL) | 0x192;
    packet->unk18 = 0x4C;
    packet->unk1C = 0;
    packet->unk20 = 0;
    packet->unk24 = 0;
    packet->unk28 = 0x18;
    packet->unk2C = 0;
    packet->unk30 = 0x30000;
    packet->unk34 = 0;
    packet->unk38 = 0x47;
    packet->unk3C = 0;
    packet->unk40 = 0x02800000;
    packet->unk44 = 0xE00000;
    packet->unk48 = 0x40;
    packet->unk4C = 0;
    packet->unk50 = (s64)(flag | 0x28028000 | ((s64)0xC800 << 19));
    packet->unk58 = 6;
    packet->unk5C = 0;
    packet->unk60 = 0x80808080;
    packet->unk64 = 1.0f;
    packet->unk68 = 1;
    packet->unk6C = 0;
    packet->unk70 = 0;
    packet->unk74 = 0;
    packet->unk78 = 2;
    packet->unk7C = 0;
    packet->unk80 = 0;
    packet->unk84 = 0;
    packet->unk88 = 5;
    packet->unk8C = 0;
    packet->unk90 = 0.625f;
    packet->unk94 = 0.875f;
    packet->unk98 = 2;
    packet->unk9C = 0;
    packet->unkA0 = 0x0E002800;
    packet->unkA4 = 0;
    packet->unkA8 = 5;
    packet->unkAC = 0;
    packet->unkB0 = 0;
    packet->unkB4 = 0;
    packet->unkB8 = 0x3F;
    packet->unkBC = 0;
    packet->unkC0 = D_004261A0[0];
    packet->unkC8 = 0x4C;
    packet->unkCC = 0;
    packet->unkD0 = 0x66802B240LL;
    packet->unkD8 = 6;
    packet->unkDC = 0;
    packet->unkE0 = 0;
    packet->unkE4 = 0;
    packet->unkE8 = 2;
    packet->unkEC = 0;
    packet->unkF0 = 0;
    packet->unkF4 = 0;
    packet->unkF8 = 5;
    packet->unkFC = 0;
    packet->unk100 = 0.625f;
    packet->unk104 = 0.4375f;
    packet->unk108 = 2;
    packet->unk10C = 0;
    packet->unk110 = 0x0E002800;
    packet->unk114 = 0;
    packet->unk118 = 5;
    packet->unk11C = 0;
    packet->unk120 = D_004261A0[0x2E];
    packet->unk128 = 0x4C;
    packet->unk12C = 0;
    packet->unk130 = 0;
    packet->unk134 = 0;
    packet->unk138 = 2;
    packet->unk13C = 0;
    packet->unk140 = 0;
    packet->unk144 = 0;
    packet->unk148 = 5;
    packet->unk14C = 0;
    packet->unk150 = 0.625f;
    packet->unk154 = 0.4375f;
    packet->unk158 = 2;
    packet->unk15C = 0;
    packet->unk160 = 0x0E002800;
    packet->unk164 = 0;
    packet->unk168 = 5;
    packet->unk16C = 0;
    packet->unk170 = 0x50000;
    packet->unk174 = 0;
    packet->unk178 = 0x47;
    packet->unk17C = 0;
    packet->unk180 = 0;
    packet->unk184 = 0;
    packet->unk188 = 0x3F;
    packet->unk18C = 0;
    packet->unk190 = 0x44;
    packet->unk194 = 0;
    packet->unk198 = 0x42;
    packet->unk19C = 0;
    packet->unk00 = 0x8019;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 0x1A;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    sceGsSyncPath(0, 0);
    D_00446C78.red = D_00446C78.green = D_00446C78.blue = 0x80;
    D_00446C7B = 0;
    D_00446C88.red = D_00446C88.green = D_00446C88.blue = 0x80;
    D_00446C8B = 0;
    D_00446D00++;
    return 1;
}

/* Calls the handler selected by D_00446CFC with the context at D_005CF350, storing value in it first. */
static int SaveScreen_RunStep(int value) {
    int (*handler)(HandlerContext *);

    Menu_StepModelLoads(0);
    handler = save_screen_steps[D_00446CFC];
    D_005CF350.arg = value;
    if (handler != NULL) {
        return handler(&D_005CF350);
    }
    return 1;
}

/* Start-up step of the save screen: loads the menu blocks, sends the first packet and resets the save screen state. */
static int SaveScreen_Start(HandlerContext *context) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;
    SaveScreen *screen;
    int (*next_handler)();

    func_001095E0();
    func_001095D0();
    func_0010B238((TextureTableHeader *)Message_GetFontTexture(D_00446F38));
    func_00109E88();
    func_001095D8();
    func_001096E8();
    ((s32 (*)(void))func_00109800)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < D_00446DEC; i++) {
        block = &D_0042A2A0[i];
        MenuTexture_Upload(block->data, block);
    }
    MenuArena_Init(D_00446F68);
    packet = MenuDraw_NextPacket();
    packet[0] = 0x8001;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 0x3F;
    packet[7] = 0;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 2;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    Menu_InitLoadBuffers();
    // match: taking the handler first orders its lui before the save screen's
    next_handler = SaveScreen_UpdateCardSelect;
    screen = &D_005CF3C0;
    screen->unk12 = -1;
    screen->unk00 = 0;
    D_00446CF8 = next_handler;
    // match: this store order schedules into retail's unk18, unk11, unk14, ... order
    screen->unk14 = 0;
    screen->unk0C = 0;
    screen->unk0D = 1;
    screen->unk0E = 1;
    screen->unk1B = 0;
    screen->unk1E = 0;
    screen->unk1F = 0;
    screen->unk20 = 0;
    screen->unk18 = 1;
    screen->unk11 = 0;
    SaveCard_Open();
    SaveCard_StartCheck(1);
    memset(D_00425510, 0, 0x400);
    memset(D_00425910, 0, 0x400);
    memset(D_00425D10, 0, 0x400);
    D_00446C08 = 0;
    D_00446C04 = 128.0f;
    D_00446CFC++;
    return TRUE;
}

/* Runs the handler in D_00446CF8. */
static int SaveScreen_RunHandler(void) {
    D_00446CF8();
    return 1;
}

/* Cuts the chain after the first link node (and after the node it points at); with no chain, resets D_00446CFC. */
static int SaveScreen_CutModelChain(void) {
    LinkNode *head;
    LinkNode *child;

    head = D_00446C90;
    if (head == NULL) {
        D_00446CFC = 0;
        return 0;
    }
    child = head->unk10;
    head->next = NULL;
    if (child != NULL) {
        child->next = NULL;
    }
    return 1;
}

/* The save screen: draws its parts, picks a card, asks the format and overwrite questions, runs the card
   checks and hands over to the save list (SaveScreen_UpdateSaveList). */
static void SaveScreen_UpdateCardSelect(void) {
    MenuCursorBar bar;
    GsPackedWord unused_corner;
    u32 i;
    s32 card_result;
    u8 kind;
    s32 slot;
    s32 card_flags;
    PadReport *report;

    card_result = SaveCard_Update();
    if (SaveCard_HasCard(D_005CF3C0.unk0C) == 0 && D_005CF3C0.unk12 == 14) {
        card_result = -1;
    }
    if (D_005CF3C0.unk1B == 0 && SaveCard_IsCheckRunning() == 0) {
        D_005CF3C0.unk18 = 0;
        D_005CF3C0.unk1B = 1;
        if (SaveCard_IsPresent(0) == 0 && SaveCard_IsPresent(1) != 0) {
            D_005CF3C0.unk0C = 1;
        }
    }

    for (i = 0; i < D_00446A60; i++) {
        kind = D_00446BF4[i].unk3E;
        SELECT_PART((s8)D_00446BF4[i].unk42);
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
            break;
        case 29:
            slot = D_00446BF4[i].slot;
            if (slot == 0 || slot == 2) {
                MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, slot, D_00425510[i]);
            }
            break;
        case 31:
            if (D_005CF3C0.unk1B != 0) {
                if (D_00446C70 != 1) {
                    MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, D_005CF3C0.unk0C, D_00425510[i]);
                } else {
                    MenuSprite_Draw((int)&D_00446BF4[i], kind, D_005CF3C0.unk0C, D_00425510[i]);
                }
            }
            break;
        case 3:
            switch (D_005CF3C0.unk12) {
            case 4:
            case 5:
            case 10:
            case 12:
            case 13:
            case 14:
            case 15:
            case 21:
                MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
            break;
        case 26:
            if (SaveCard_IsPresent(0) == 0) {
                if (D_00446C70 == 0) {
                    MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0x20, D_00425510[i]);
                } else if (D_00446C70 < 2) {
                    MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 0x20);
                }
            } else if (D_00446C70 != 1) {
                MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0x80, D_00425510[i]);
            } else {
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 0x80);
            }
            break;
        case 27:
            if (SaveCard_IsPresent(1) == 0) {
                if (D_00446C70 == 0) {
                    MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0x20, D_00425510[i]);
                } else if (D_00446C70 < 2) {
                    MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 0x20);
                }
            } else if (D_00446C70 != 1) {
                MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0x80, D_00425510[i]);
            } else {
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 0x80);
            }
            break;
        case 28:
            switch (D_005CF3C0.unk12) {
            case 4:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 4);
                break;
            case 5:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 5);
                break;
            case 12:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 12);
                break;
            case 13:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 13);
                break;
            case 14:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 14);
                break;
            case 15:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 15);
                break;
            case 21:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 21);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 10);
                break;
            case 22:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 22);
                break;
            }
            break;
        default:
            MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
            break;
        }
        SELECT_PART(0);
    }

    if (D_005CF3C0.unk12 == 12) {
        // match: a depth nothing reads; without it the 64-bit mask of the bar's depth field is loaded after the branch below
        unused_corner.f.mid24 = 0x5F370;
        bar.first.f.lo16 = 0x7580;
        bar.first.f.mid16 = D_005CF3C0.unk0D * 0xB0 + 0x80E0;
        if (D_005CF3C0.unk0D != 0) {
            bar.first.f.mid16 = D_005CF3C0.unk0D * 0xB0 + 0x80F0;
        }
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1540;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFF] = Trade_PulseValue(0x10, 0x20, D_00425510[0xFF]);
        bar.alpha = D_00425510[0xFF];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    if (D_005CF3C0.unk21 != 0) {
        if (SaveCard_IsCheckRunning() == 0) {
            D_005CF3C0.unk21 = 0;
            D_005CF3C0.unk18 = 0;
            if (D_005CF3C0.unk12 == 12) {
                if (D_005CF3C0.unk0D == 0) {
                    SaveCard_StartFormat(D_005CF3C0.unk0C);
                    D_005CF3C0.unk12 = 13;
                    D_005CF3C0.unk14 = 1;
                } else {
                    D_005CF3C0.unk12 = -1;
                }
            } else if (D_005CF3C0.unk12 == 14) {
                if (SaveCard_StartReadList(D_005CF3C0.unk0C, 0) == 0) {
                    D_005CF3C0.unk14 = 1;
                }
            } else if (D_005CF3C0.unk12 == 21) {
                D_005CF3C0.unk12 = -1;
            } else if (D_005CF3C0.unk12 != -1) {
                D_005CF3C0.unk12 = -1;
            } else if (SaveCard_IsPresent(D_005CF3C0.unk0C) != 0) {
                if (SaveCard_IsFormatted(D_005CF3C0.unk0C) == 0) {
                    D_005CF3C0.unk12 = 12;
                } else if (SaveCard_HasSaveFolder(D_005CF3C0.unk0C) == 0 && SaveCard_HasRoomForFolder(D_005CF3C0.unk0C) < 0) {
                    D_005CF3C0.unk12 = 21;
                    SaveCard_StartCheck(1);
                } else if (SaveCard_StartReadList(D_005CF3C0.unk0C, 1) == 0) {
                    D_005CF3C0.unk14 = 1;
                }
            } else if (D_005CF3C0.unk0C == 0) {
                D_005CF3C0.unk12 = 4;
            } else if (D_005CF3C0.unk0C == 1) {
                D_005CF3C0.unk12 = 5;
            }
        }
    } else if (D_005CF3C0.unk14 != 0) {
        if (SaveCard_CheckBusy() == 0) {
            if (D_005CF3C0.unk12 == 13) {
                D_005CF3C0.unk12 = 14;
                D_005CF3C0.unk0D = 1;
                D_005CF3C0.unk13 = -1;
            } else if (SaveCard_HasAnySave() != 0 || SaveCard_HasRoomForFolder(D_005CF3C0.unk0C) > 0) {
                D_005CF3C0.unk18 = 1;
                D_005CF3C0.unk13 = -1;
                D_00446C70 = 2;
                D_005CF3C0.unk0D = 0;
            } else {
                D_005CF3C0.unk12 = 21;
                SaveCard_StartCheck(1);
            }
            D_005CF3C0.unk14 = 0;
        } else if (card_result < 0) {
            if (D_005CF3C0.unk12 == 13) {
                D_005CF3C0.unk12 = 15;
                D_005CF3C0.unk14 = 0;
                D_005CF3C0.unk0D = 1;
                SaveCard_StartCheck(1);
                card_result = 0;
            } else if (D_005CF3C0.unk12 == -1) {
                if (SaveCard_IsDamaged(D_005CF3C0.unk0C) != 0) {
                    if (SaveCard_HasAnySave() != 0 || SaveCard_HasRoomForFolder(D_005CF3C0.unk0C) > 0) {
                        D_005CF3C0.unk18 = 1;
                        D_00446C70 = 2;
                        D_005CF3C0.unk13 = -1;
                        D_005CF3C0.unk0D = 0;
                        SaveCard_Cancel(D_005CF3C0.unk0C);
                    } else {
                        D_005CF3C0.unk12 = 21;
                        SaveCard_StartCheck(1);
                    }
                } else {
                    D_005CF3C0.unk12 = 10;
                }
                card_result = 0;
                D_005CF3C0.unk14 = 0;
            }
        }
    }

    if (D_005CF3C0.unk11 != 0) {
        if (card_result < 0) {
            D_005CF3C0.unk18 = 1;
            D_00446C70 = 3;
        }
    } else if (D_005CF3C0.unk12 == 12 && SaveCard_IsPresent(D_005CF3C0.unk0C) == 0) {
        D_005CF3C0.unk12 = 10;
        D_005CF3C0.unk0D = 1;
    } else if (card_result < 0) {
        D_005CF3C0.unk12 = 10;
        D_005CF3C0.unk0D = 1;
        D_005CF3C0.unk14 = 0;
        SaveCard_StartCheck(1);
    } else if (D_005CF3C0.unk14 == 0 && D_005CF3C0.unk18 == 0) {
        if ((D_00446DE8 & 0xA0) && D_0044731C != 0xFF && D_005CF3C0.unk12 == -1) {
            Sound_PlayMenu(1);
            D_005CF3C0.unk0C = (D_005CF3C0.unk0C + 1) & 1;
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & 0x4000) && D_0044731C != 0xFF) {
                Sound_PlayMenu(0);
                D_005CF3C0.unk21 = 1;
                D_005CF3C0.unk18 = 1;
            } else {
                report = Pad_GetReport(0, 0);
                if ((report->pressed & 0x2000) && D_0044731C != 0xFF) {
                    Sound_PlayMenu(2);
                    if (D_005CF3C0.unk12 == 14) {
                        if (SaveCard_StartReadList(D_005CF3C0.unk0C, 0) == 0) {
                            D_005CF3C0.unk14 = 1;
                        }
                    } else if (D_005CF3C0.unk12 != -1) {
                        D_005CF3C0.unk12 = -1;
                        D_005CF3C0.unk0D = 1;
                    } else {
                        D_005CF3C0.unk11 = 1;
                        SaveCard_Close();
                    }
                } else if ((D_00446DE8 & 0x10) && D_0044731C != 0xFF && D_005CF3C0.unk12 == 12) {
                    Sound_PlayMenu(1);
                    D_005CF3C0.unk0D = (D_005CF3C0.unk0D + 1) & 1;
                } else if ((D_00446DE8 & 0x40) && D_0044731C != 0xFF && D_005CF3C0.unk12 == 12) {
                    Sound_PlayMenu(1);
                    D_005CF3C0.unk0D = (D_005CF3C0.unk0D - 1) & 1;
                }
            }
        }
    }

    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446A60; i++) {
            // match: the cursor bar's level is stepped once for every part, inside the loop
            D_00425510[0xFF] += 1.6f;
            if (D_00425510[0xFF] > 16.0f) {
                D_00425510[0xFF] = 16.0f;
            }
            if (D_00446BF4[i].unk3E == 26) {
                if (SaveCard_IsPresent(0) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x20;
                }
            } else if (D_00446BF4[i].unk3E == 27) {
                if (SaveCard_IsPresent(1) != 0) {
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
                if (D_005CF3C0.unk1B != 0) {
                    D_005CF3C0.unk18 = 0;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446A60; i++) {
            D_00425510[0xFF] -= 1.6f; // match: stepped once for every part here too
            if (D_00425510[0xFF] < 0.0f) {
                D_00425510[0xFF] = 0.0f;
            }
            if (D_00446BF4[i].unk3E == 26) {
                if (SaveCard_IsPresent(0) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x20;
                }
            } else if (D_00446BF4[i].unk3E == 27) {
                if (SaveCard_IsPresent(1) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha = 0x20;
                }
            }
            D_00425510[i] -= SPRITE_PACKET_COLOR_VIEW(&D_00446BF4[i])->alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_00446C70 == 2) {
                    D_005CF3C0.unk1D = 0;
                    D_00446CF8 = (int (*)())SaveScreen_UpdateSaveList;
                    D_005CF3C0.unk08 = 0;
                    D_005CF3C0.unk0D = 0;
                    D_00446C09 = 0;
                    D_00446C08 = 0;
                    card_flags = SaveCard_GetSaveMask();
                    if (card_flags != 0 && (card_flags & 1)) {
                        D_00426610[0] = 0;
                        if (card_flags & 2) {
                            D_00426610[1] = 1;
                            D_00426610[2] = 2;
                        } else {
                            D_00426610[1] = 1;
                            D_00426610[2] = 1;
                        }
                    } else {
                        D_00426610[0] = 0;
                        D_00426610[1] = 0;
                        D_00426610[2] = 0;
                    }
                    D_00426600[0] = 0;
                    D_00426600[1] = 1;
                    D_00426600[2] = 2;
                } else if (D_00446C70 == 3) {
                    D_005CF3C0.unk0D = 0;
                    D_00446CFC++;
                    MenuHeap_FreeToMark();
                }
                D_005CF3C0.unk12 = -1;
                D_00446C70 = 0;
                break;
            }
        }
        break;
    }
}

/* The save list of the save screen: shows the three saves newest first, moves between them, asks before
   overwriting, starts the save and reorders the lines once it is written. */
static void SaveScreen_UpdateSaveList(void) {
    CardSaveInfo *info[3];
    MenuCursorBar bar;
    s32 i;
    s32 first_arg;
    s32 second_arg;
    s32 kind;
    s32 slot;
    s32 group;
    s32 *table;
    char *text;
    GsPackedWord word;
    PadReport *report;
    s32 same_count;
    s32 save_state;
    s32 line;
    s32 swapped;

    info[0] = (CardSaveInfo *)SaveCard_GetSaveInfo(D_00426600[0]);
    D_00446DF4 = info[0]->unk0C;
    D_00446DF8 = info[0]->unk08;
    D_00446DFC = info[0]->unk28;
    D_00446E00 = info[0]->unk10;
    D_00446E04 = info[0]->unk14;
    D_00446E08 = info[0]->unk18;
    D_00446E0C = info[0]->unk1C;
    D_00446E10 = info[0]->unk20;
    D_00446E14 = info[0]->unk24;
    D_00446E18 = info[0]->unk17B;
    D_00446E1C = info[0]->unk17A;
    D_00446E20 = info[0]->unk179;
    info[1] = (CardSaveInfo *)SaveCard_GetSaveInfo(D_00426600[1]);
    D_00446E24 = info[1]->unk0C;
    D_00446E28 = info[1]->unk08;
    D_00446E2C = info[1]->unk28;
    D_00446E30 = info[1]->unk10;
    D_00446E34 = info[1]->unk14;
    D_00446E38 = info[1]->unk18;
    D_00446E3C = info[1]->unk1C;
    D_00446E40 = info[1]->unk20;
    D_00446E44 = info[1]->unk24;
    D_00446E48 = info[1]->unk17B;
    D_00446E4C = info[1]->unk17A;
    D_00446E50 = info[1]->unk179;
    info[2] = (CardSaveInfo *)SaveCard_GetSaveInfo(D_00426600[2]);
    D_00446E54 = info[2]->unk0C;
    D_00446E58 = info[2]->unk08;
    D_00446E5C = info[2]->unk28;
    D_00446E60 = info[2]->unk10;
    D_00446E64 = info[2]->unk14;
    D_00446E68 = info[2]->unk18;
    D_00446E6C = info[2]->unk1C;
    D_00446E70 = info[2]->unk20;
    D_00446E74 = info[2]->unk24;
    D_00446E78 = info[2]->unk17B;
    D_00446E7C = info[2]->unk17A;
    D_00446E80 = info[2]->unk179;

    if ((D_005CF3C0.unk13 == -1 && (D_005CF3C0.unk08 == 0 || D_005CF3C0.unk08 != SaveCard_GetFreeClusters(D_005CF3C0.unk0C))) ||
        (D_005CF3C0.unk13 == 2 && D_005CF3C0.unk08 != SaveCard_GetFreeClusters(D_005CF3C0.unk0C))) {
        D_005CF3C0.unk08 = 0;
        for (i = 0; i < 3; i++) {
            MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] = SaveCard_GetSaveState(i);
        }
        for (i = 0; i < 3; i++) {
            if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] != 1) {
                switch (i) {
                case 0:
                    if (D_005CF3C0.unk1F > 0 || D_005CF3C0.unk20 > 0) {
                        first_arg = 1;
                        second_arg = 0;
                    } else {
                        first_arg = 0;
                        second_arg = 1;
                    }
                    break;
                case 1:
                    if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk20 > 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk20 == 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk20 > 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk20 == 0) { // match: no else: a negative byte sets neither argument
                        second_arg = 1;
                        first_arg = 1;
                    }
                    break;
                case 2:
                    if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk1F > 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF3C0.unk1E > 0 && D_005CF3C0.unk1F == 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk1F > 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF3C0.unk1E == 0 && D_005CF3C0.unk1F == 0) { // match: a last test, not an else: retail keeps a path that sets neither
                        second_arg = 1;
                        first_arg = 2;
                    }
                    break;
                }
                if (SaveCard_HasRoomFor(D_005CF3C0.unk0C, first_arg, second_arg) > 0) {
                    MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] = 0;
                } else {
                    MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[i] = -1;
                }
            }
        }
    }

    for (i = 0; i < D_00446A64; i++) {
        kind = D_00446BF8[i].unk3E;
        slot = D_00446BF8[i].slot;
        ((int (*)(s8))func_001E5210)((s8)D_00446BF8[i].unk42); // match: a signed byte, see SET_PART_MODE_OF
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&D_00446BF8[i], 0x3EA, 0, D_00425510[i]);
            break;
        case 3:
            group = slot / 3;
            if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[group]] == 1) {
                MenuSprite_Draw((int)&D_00446BF8[i], 0x3EB, group * 3, D_00425510[i]);
            } else if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[group]] == 0) {
                MenuSprite_Draw((int)&D_00446BF8[i], 0x3EB, group * 3 + 1, D_00425510[i]);
            }
            break;
        case 25:
            if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[slot]] != -1) {
                MenuSprite_Draw((int)&D_00446BF8[i], 0x401, D_005CF3C0.unk0D, D_00425510[i]);
            }
            break;
        default:
            MenuSprite_Draw((int)&D_00446BF8[i], kind + 1000, D_005CF3C0.unk0D, D_00425510[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    table = (s32 *)Message_GetFontText(D_00446F38);
    word.raw = 0x000A87507C107120;
    for (i = 0; i < 3; i++) {
        if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_00426600[i]] == 1) {
            kind = table[info[i]->unk04 + 1]; // match: the text's offset in the kind variable: retail keeps both in one register
            if (kind != 0) {
                text = (char *)table + kind;
                MenuText_SetPen(word);
                MenuText_SetColor(D_00446C78);
                MenuText_DrawWrapped((TradeValue *)text, 0x18);
            }
        }
        word.f.mid16 += 0x340;
    }

    for (i = 0; i < D_00446AAC; i++) {
        kind = D_00446C58[i].unk3E;
        SET_PART_MODE_OF(&D_00446C58[i]);
        if (kind == 3) {
            switch (D_005CF3C0.unk13) {
            case 6:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 6);
                break;
            case 7:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 7);
                break;
            case 0:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 0);
                break;
            case 2:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 2);
                break;
            case 20:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 20);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 10);
                break;
            case 11:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 11);
                break;
            }
        } else {
            switch (D_005CF3C0.unk13) {
            case 0:
            case 2:
            case 6:
            case 7:
            case 10:
            case 11:
            case 20:
                MenuSprite_Draw((int)&D_00446C58[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
        }
        SET_PART_MODE(0);
    }

    if (D_005CF3C0.unk13 == 6 || D_005CF3C0.unk13 == 7) {
        bar.first.f.lo16 = 0x7490;
        bar.first.f.mid16 = D_005CF3C0.unk0E * 0xC0 + 0x80A0;
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1720;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFF] = Trade_PulseValue(0x10, 0x20, D_00425510[0xFF]);
        bar.alpha = D_00425510[0xFF];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    D_005CF3C0.unk00 = SaveCard_Update();
    if (SaveCard_HasCard(D_005CF3C0.unk0C) == 0) {
        D_005CF3C0.unk00 = -1;
    }
    if (D_005CF3C0.unk00 < 0) {
        if (D_005CF3C0.unk13 == 0 || D_005CF3C0.unk13 == 11) {
            D_005CF3C0.unk13 = 11;
        } else {
            D_005CF3C0.unk13 = 10;
        }
    }
    if (D_005CF3C0.unk13 == 0 && SaveCard_CheckBusy() == 0) {
        D_005CF3C0.unk13 = 2;
        if (D_00447324 != 0) {
            D_00447324 = 0;
            same_count = 0;
            if (D_00426610[0] == D_00426600[0]) {
                same_count++;
            }
            if (D_00426610[1] == D_00426600[1]) {
                same_count++;
            }
            if (D_00426610[2] == D_00426600[2]) {
                same_count++;
            }
            for (i = 0; i < 3; i++) {
                if (i != D_005CF3C0.unk0D && D_00426610[i] >= D_00426610[D_005CF3C0.unk0D]) {
                    D_00426610[i]++;
                }
            }
            if (D_00426600[D_005CF3C0.unk0D] != 0) {
                if (same_count == 1) {
                    swapped = D_00426600[D_00426610[D_005CF3C0.unk0D]];
                    D_00426600[D_00426610[D_005CF3C0.unk0D]] = D_00426600[D_005CF3C0.unk0D];
                    D_00426600[D_005CF3C0.unk0D] = swapped;
                } else if (same_count == 2) {
                    if (D_00426600[D_005CF3C0.unk0D] != 1) {
                        for (i = 0; i < 3; i++) {
                            if (D_00426600[i] == 1) {
                                break;
                            }
                        }
                        line = i;
                        // match: the group variable as the second counter: retail keeps both in one register
                        for (group = 0; group < 3; group++) {
                            if (D_00426600[group] == 2) {
                                break;
                            }
                        }
                        D_00426600[group] = 1;
                        D_00426600[line] = 2;
                    }
                }
            }
        }
    }

    report = Pad_GetReport(0, 0);
    if ((report->pressed & 0x4000) && D_0044731C != 0xFF) {
        if (D_005CF3C0.unk13 != 0) {
            report = Pad_GetReport(0, 0);
            if (report->held & 0x50) {
                D_005CF3C0.unk1D = 1;
            } else {
                D_005CF3C0.unk1D = 3;
            }
        }
    }
    if (D_005CF3C0.unk1D == 2) {
        D_005CF3C0.unk1D = 3;
    }
    if (D_005CF3C0.unk1D == 1) {
        report = Pad_GetReport(0, 0);
        if (report->held & 0x50) {
            if (D_00446DE8 & 0x50) {
                D_005CF3C0.unk1D = 2;
            }
        } else {
            D_005CF3C0.unk1D = 3;
        }
    }

    if (D_005CF3C0.unk13 != 0 && D_005CF3C0.unk18 == 0) {
        if (D_005CF3C0.unk1D == 3) {
            D_005CF3C0.unk1D = 0;
            switch (D_005CF3C0.unk13) {
            case -1:
                Sound_PlayMenu(0);
                save_state = SaveCard_GetSaveState(D_00426610[D_005CF3C0.unk0D]);
                if (save_state == 0) {
                    if (SaveCard_HasRoomFor(D_005CF3C0.unk0C, 1, 0) > 0 &&
                        MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_005CF3C0.unk0D] != -1) {
                        D_00447324 = 1;
                        D_005CF3C0.unk13 = 6;
                        D_005CF3C0.unk08 = SaveCard_GetFreeClusters(D_005CF3C0.unk0C);
                    } else {
                        D_005CF3C0.unk13 = 20;
                        D_00447324 = 0;
                    }
                } else if (save_state == 1) {
                    D_005CF3C0.unk13 = 7;
                }
                break;
            case 6:
            case 7:
                if (D_005CF3C0.unk0E == 0) {
                    Sound_PlayMenu(0);
                    D_005CF3C0.unk13 = 0;
                    func_001D20E0();
                    func_00103C18();
                    SaveCard_StartSave(D_005CF3C0.unk0C, D_00426610[D_005CF3C0.unk0D], (s32 *)D_0036A130);
                } else {
                    Sound_PlayMenu(2);
                    D_005CF3C0.unk13 = -1;
                }
                D_005CF3C0.unk0E = 1;
                break;
            case 2:
            case 20:
                Sound_PlayMenu(0);
                D_005CF3C0.unk13 = -1;
                break;
            case 10:
            case 11:
                Sound_PlayMenu(0);
                D_00446C70 = 3;
                D_005CF3C0.unk18 = 1;
                break;
            }
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & 0x2000) && D_0044731C != 0xFF && D_005CF3C0.unk1D == 0) {
                Sound_PlayMenu(2);
                switch (D_005CF3C0.unk13) {
                case -1:
                    D_00446C70 = 2;
                    D_005CF3C0.unk18 = 1;
                    break;
                case 10:
                case 11:
                    D_00446C70 = 3;
                    D_005CF3C0.unk18 = 1;
                    break;
                case 6:
                case 7:
                    D_005CF3C0.unk0E = 1;
                case 2:
                case 20:
                    D_005CF3C0.unk13 = -1;
                    break;
                }
            } else if (D_00446DE8 & 0x40) {
                switch (D_005CF3C0.unk13) {
                case -1:
                    for (i = 0; i < 3; i++) {
                        D_005CF3C0.unk0D = ((s8)(D_005CF3C0.unk0D + 1) + 3) % 3;
                        if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_005CF3C0.unk0D] >= 0) {
                            if (i != 2) {
                                Sound_PlayMenu(1);
                            }
                            break;
                        }
                    }
                    break;
                case 6:
                case 7:
                    Sound_PlayMenu(1);
                    D_005CF3C0.unk0E = (D_005CF3C0.unk0E + 1) & 1;
                    break;
                }
            } else if (D_00446DE8 & 0x10) {
                switch (D_005CF3C0.unk13) {
                case -1:
                    for (i = 0; i < 3; i++) {
                        D_005CF3C0.unk0D = ((s8)(D_005CF3C0.unk0D - 1) + 3) % 3;
                        if (MENU_SCREEN_SAVES(&D_005CF3C0)->save_state[D_005CF3C0.unk0D] >= 0) {
                            if (i != 2) {
                                Sound_PlayMenu(1);
                            }
                            break;
                        }
                    }
                    break;
                case 6:
                case 7:
                    Sound_PlayMenu(1);
                    D_005CF3C0.unk0E = (D_005CF3C0.unk0E + 1) & 1;
                    break;
                }
            }
        }
    }

    switch (D_00446C70) {
    case 0:
        D_00425510[0xFF] += 12.8f;
        if (D_00425510[0xFF] > 128.0f) {
            D_00425510[0xFF] = 128.0f;
        }
        D_00446C78.alpha = D_00425510[0xFF];
        for (i = 0; i < D_00446A64; i++) {
            D_00425510[i] += SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A64; i++) {
                    D_00425510[i] = SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha;
                }
                D_00446C70 = 1;
                D_005CF3C0.unk18 = 0;
                break;
            }
        }
        break;
    case 2:
    case 3:
        D_00425510[0xFF] -= 12.8f;
        if (D_00425510[0xFF] < 0.0f) {
            D_00425510[0xFF] = 0.0f;
        }
        D_00446C78.alpha = D_00425510[0xFF];
        for (i = 0; i < D_00446A64; i++) {
            D_00425510[i] -= SPRITE_PACKET_COLOR_VIEW(&D_00446BF8[i])->alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446C04 = 128.0f;
                D_00446CF8 = (int (*)())SaveScreen_UpdateCardSelect;
                D_00446C08 = 0;
                D_005CF3C0.unk0D = 1;
                D_005CF3C0.unk0E = 1;
                if (D_00446C70 == 2) {
                    SaveCard_StartCheck(0);
                } else {
                    SaveCard_StartCheck(1);
                }
                D_00446C70 = 0;
                break;
            }
        }
        break;
    }
}
