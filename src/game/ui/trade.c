/* Three modal menus that share one source file: the trade screens (Trade_), the smith's screens (Smith_) and the
   save screen (SaveScreen_). Each has a function that runs it until it is left (_Run): every frame that grabs the
   frame buffer and calls the current step, and the steps call the handler of the list or question on screen. */
#include "ui/trade.h"
#include "ui/menu.h"
#include "player/player.h"
#include "input/pad.h"
#include "gfx/packet.h"
#include <eeregs.h>
#include "core/state.h"
#include "core/load.h"
#include "gfx/texture.h"
#include "ui/message.h"
#include "ui/link.h"
#include "common.h"
#include "core/game.h"
#include "gfx/gs.h"
#include "save/save_card.h"
#include "ui/menu_screens.h"

extern u8 item_counts[380];
extern LevelDef item_name_defs[380];
extern LevelRecord weapon_level_records[70];
extern LevelRecord worn_item_level_records[160];
extern MenuItem common_item_entries[150];
extern MenuItem all_item_entries[380];
extern MenuItem weapon_entries[70];
extern MenuItem shield_entries[30];
extern MenuItem helm_entries[25];
extern MenuItem armor_entries[25];
extern MenuItem gauntlet_entries[25];
extern MenuItem boot_entries[25];
extern MenuItem ring_entries[10];
extern MenuItem bracelet_entries[10];
extern MenuItem amulet_entries[10];
extern MenuItem *item_list_cursor[4];
extern MenuItem *item_list_top[4];
extern MenuItem *item_list_page_mark[4];
extern MenuItem *trade_stock_list_top[4]; /* the stock list's first row on screen */
extern MenuItem *trade_stock_list_page_mark[4]; /* the stock list's page mark: its prev is the last row on screen */
extern MenuItem *trade_stock_list_cursor[4]; /* the stock item under the cursor */
/* the ring of items that may be handed over (item_list_cursor; the smith's ring of items to hand in) */
extern MenuItem *handover_list_first;  /* its first node */
extern MenuItem *handover_list_last_page; /* the row the window starts at on the ring's last page */
/* the menu's ring of everything owned; here they hold the stock ring (the smith's: the items to collect) */
extern MenuItem *owned_list_first;
extern MenuItem *owned_list_last_page;
extern s32 handover_list_count;        /* how many nodes the ring has */
extern s32 owned_list_count;
/* Appends node to the ring being built; the seventh one becomes the page mark. */
/* Feet per centimetre, rounded the way the retail code has it. */
#define CM_TO_FEET 0.03281f
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
            values.values[table][(item_base) + i].give_value != 0 && (records)[(record_base) + i].in_smith_rings == 0) {    \
            LINK_ITEM(&(nodes)[i], item_list_page_mark)                                                                     \
        }                                                                                                          \
    }
int sceGsSyncPath(int, int);
int sceGsSyncV(int);
void Vram_Clear(int, int);
void Sound_MutePositional(void);
void Sound_UnmutePositional(void);
void Sound_UpdateTagged(void);
void Input_Update(void);
s16 MenuPad_Update(void);
void MenuHeap_FreeToMark(void);
extern s8 modal_input_locked;       /* TRUE while a trade or smith screen fades: the buttons are not read */
extern s8 trade_amount_down_locked;  /* TRUE stops LEFT lowering the take amount; the game only clears it */
extern s8 menu_mode;
extern s32 modal_pad_mode;          /* Pad_GetMode of pad 0 this frame; 0xFF: no pad */
extern s32 modal_pad_mode_last;     /* the same, last frame */
extern s32 modal_even_field;       /* what Menu_EndFrame returned (the field); never read */
extern s32 trade_pad_change_frames; /* counts 20 frames without buttons after the pad type changed */
extern u16 menu_buttons;
extern s32 trade_run_step;          /* which of trade_run_steps runs */
extern u64 menu_first_draw_env[47];             /* the frame register of the first buffer ([0]) and of the second ([46]) */
extern int context1_buffer_half;
extern int trade_step;              /* which of trade_steps runs */
extern HandlerContext trade_context; /* what the trade steps and handlers get */
extern f32 screen_part_alphas[0x100];
extern f32 panel_part_alphas[0x100];
extern f32 message_part_alphas[0x100]; /* the fade level of each part of the shop's (smith's) message window */
extern LoadedBlockRef menu_file_blocks[];
extern ColorBytes menu_draw_color;
extern ColorBytes model_draw_color;
extern s8 pulse_sprite_falling;
extern s8 pulse_value_falling;
/* the ring the list on screen walks: a copy of the handover_list_* or the owned_list_* three */
extern MenuItem *item_list_first;
extern MenuItem *item_list_last_page;
extern s32 item_list_count;
extern void (*trade_handler)(HandlerContext *); /* the screen Trade_RunHandler runs */
extern s32 menu_file_block_count;
extern s32 menu_heap;
extern s32 trade_menu_arena;
void *memset(void *, int, u32);
void Packet_OnFrameStart(void);
void Packet_OnFrameEnd(void);
void Packet_SwapBuffers(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void GsDraw_QueueTexFlush(void);
void TextureTable_QueueAll(TextureTableHeader *);
s32 MenuArena_Init(s32 value);
void MenuTexture_Upload(void *, LoadedBlockRef *);
extern LinkNode *model_load_chain;
/* the loads of the model shown and of the four the cursor brings on with up (0x10), down (0x40), left (0x80) and
   right (0x20): in a list a row and a page either way, on the equipment screen the next list and the next item */
extern LinkNode model_load_shown;
extern LinkNode model_load_up;
extern LinkNode model_load_down;
extern LinkNode model_load_left;
extern LinkNode model_load_right;
extern u32 preshop_part_count;
extern SpritePacket *preshop_parts;
extern s8 trade_screen_state;       /* 0: fading in, 1: open, 2 and up: fading out to the next screen */
s32 Sound_PlayMenu(s32);
int Menu_SetBlendMode(u8 mode);
void MenuDraw_Bar(MenuCursorBar *);
void MenuSprite_Draw(int, int, int, int);
/* match: the part number is passed sign-extended and no result is kept (the definition takes a u8 and returns int) */
#define SELECT_PART(part) ((void (*)(s32))Menu_SetBlendMode)(part)
extern LevelRecordDef worn_defs[];
extern PlayerItemDef weapon_defs[];
extern u8 player_weapon[];
/* the three kind words table 4's stock list shows: "Bones", "Scales", "Claws" in half-width glyph codes
   (0x800 | character - 0x20; MenuText_DrawWrapped draws code 0x800 + n as half-width cell n) */
extern TradeValue bones_label[4];
extern TradeValue scales_label[4];
extern TradeValue claws_label[4];
extern f32 trade_stock_list_model_position[4]; /* where Trade_UpdateStockList draws the item's model */
extern f32 menu_view_screen_matrix[16];
extern u32 shop_part_count;
extern u32 shop_g2_part_count;
extern u32 shop_a_part_count;
extern u32 shop_g_part_count;
extern SpritePacket *shop_parts;
extern SpritePacket *shop_g2_parts;
extern SpritePacket *shop_a_parts;
extern SpritePacket *shop_g_parts;
extern u8 trade_panel_shown;        /* TRUE while the selected item's panel is up */
extern s32 trade_stock_list_panel_sound_pending; /* TRUE until the panel's opening sound has played */
extern f32 trade_stock_list_model_angle; /* the model's turn */
extern s32 menu_number_44;
extern s32 menu_number_45;
extern s32 menu_number_46;
extern s32 menu_number_47;
extern s32 menu_number_48;
extern s32 menu_number_49;
extern s32 menu_number_50;
extern s32 menu_number_51;
extern s32 menu_number_52;
extern s32 menu_number_54;
extern s32 menu_number_70;
extern s32 menu_number_71;
extern s32 menu_number_72;
extern s32 menu_number_73;
extern s32 menu_number_77;
extern s32 menu_number_94;
extern s8 trade_panel_state;        /* the panel's fade: 0 in, 1 open, 2 out and close, 3 out */
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
int Menu_DrawItemModel(f32 *); // match: an int result keeps $2 for the load after the call
int Menu_IsEquipSlotFilled(s32, u32);
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
#define DRAW_PACKETS(packets, count, param)                                                          \
    for (i = 0; i < (count); i++) {                                                                  \
        SELECT_PART((s8)(packets)[i].blend_mode);                                                    \
        MenuSprite_Draw((int)&(packets)[i], (packets)[i].handler_index + 1000, (param), (int)panel_part_alphas[i]); \
        SELECT_PART(0);                                                                              \
    }
extern PlayerState player_state;
extern f32 trade_take_amount_model_position[4];
extern u32 shop_mess_part_count;
extern SpritePacket *shop_mess_parts;
extern s32 trade_take_amount_panel_sound_pending;
extern f32 trade_take_amount_model_angle;
extern s32 menu_number_74;
extern s32 menu_number_79;
extern s32 menu_number_80;
extern s32 menu_number_81;
extern s32 menu_number_82;
s32 MenuText_DrawLineCentered(u16 *, s32);
extern f32 trade_owned_list_model_position[4];
extern f32 trade_owned_list_model_angle;
/* Starts every packet of a list at its full alpha. match: the alpha is read as an unsigned bit-field */
#define FILL_FADE_2(levels, packets, count)                               \
    for (i = 0; i < (count); i++) {                                     \
        (levels)[i] = SPRITE_PACKET_COLOR(&(packets)[i])->alpha;        \
    }
extern f32 trade_give_amount_model_position[4];
extern f32 trade_give_amount_model_angle;
void Menu_ClearEquipSlot(s8);
int sprintf(char *, char *, ...);
extern char model_name_shown[];           /* the names of the five link nodes at model_load_shown */
extern char model_name_up[];
extern char model_name_down[];
extern char model_name_left[];
extern char model_name_right[];
#define PAGE_ROWS 7
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 Trade_StockListScrollUp(void) {
    trade_stock_list_top[0] = trade_stock_list_top[0]->prev;
    trade_stock_list_page_mark[0] = trade_stock_list_page_mark[0]->prev;
    trade_stock_list_cursor[0] = trade_stock_list_top[0];
    return trade_stock_list_top[0] != item_list_last_page;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 Trade_StockListScrollDown(void) {
    trade_stock_list_top[0] = trade_stock_list_top[0]->next;
    trade_stock_list_page_mark[0] = trade_stock_list_page_mark[0]->next;
    trade_stock_list_cursor[0] = trade_stock_list_top[0];
    return trade_stock_list_top[0] != item_list_first;
}
extern MenuItem *smith_collect_list_top[4]; /* the same three for the list of items to collect */
extern MenuItem *smith_collect_list_page_mark[4];
extern MenuItem *smith_collect_list_cursor[];
extern ClockTime current_play_time;
void PlayTime_Update(void);
int PlayTime_SecondsBetween(ClockTime, ClockTime);
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
            records[base + i].in_smith_rings == 0 && records[base + i].level < 100) { \
            APPEND(&nodes[i], item_list_page_mark[0]) \
        } \
    }
/* The owned, usable items of one run of definitions whose record is idle. */
#define APPEND_IDLE(nodes, records, base, first_item, limit) \
    for (i = 1; i < limit; i++) { \
        if (item_counts[first_item + i] != 0 && !(item_name_defs[first_item + i].state & 2) && \
            records[base + i].in_smith_rings == 0) { \
            APPEND(&nodes[i], item_list_page_mark[0]) \
        } \
    }
extern s8 smith_confirm_shown;      /* TRUE while Smith_UpdateListWithConfirm shows its question */
extern s8 smith_unread_flag;         /* cleared with the smith screen state; nothing sets or reads it */
extern s8 smith_upgrade_choice;     /* which of the three lines of Smith_UpdateItemScreen is picked */
extern s32 smith_pad_change_frames; /* counts 20 frames without buttons after the pad type changed */
extern s32 smith_run_step;          /* which of smith_run_steps runs */
extern void (*smith_handler)(HandlerContext *); /* the screen Smith_RunHandler runs */
extern s32 smith_step;              /* which of smith_steps runs */
extern HandlerContext smith_context; /* what the smith steps and handlers get */
extern u32 pretune_part_count;              /* how many parts the screen has */
extern u32 tune_mess_part_count;
extern s32 detail_panel_open; // match: this file reads and writes it as a word
extern SpritePacket *pretune_parts;
extern SpritePacket *tune_mess_parts;
extern s8 smith_choices_active;     /* nonzero once the cursor is on the three lines */
extern s8 smith_screen_state;       /* 0: fading in, 1: open, 2 to 4: fading out to a list, 5: fading out to leave */
void MenuSprite_DrawPlain(int, int, int);
extern f32 smith_upgrade_list_model_position[4];
extern u32 tune_part_count;              /* how many parts the screen has */
extern SpritePacket *tune_parts;
extern s32 smith_upgrade_list_panel_sound_pending;
extern f32 smith_upgrade_list_model_angle;
extern s32 menu_number_75;
extern s8 smith_panel_state;        /* the same for the selected item's panel */
/* match: the two level lookups are inline functions taking the record index, so the subtraction is not folded into
   the table address; each caller picks the table itself and uses the level inside its own branch. */
static inline u8 GetWeaponLevel(s32 index) {
    return weapon_level_records[index].level;
}
static inline u8 GetItemLevel(s32 index) {
    return worn_item_level_records[index].level;
}
static inline void SetCorner(GsPackedWord *corner, s32 left, s32 top, s32 depth) {
    corner->f.lo16 = left;
    corner->f.mid16 = top;
    corner->f.mid24 = depth;
}
extern f32 smith_upgrade_confirm_model_position[4];
extern s32 smith_upgrade_confirm_panel_sound_pending;
extern f32 smith_upgrade_confirm_model_angle;
extern s32 menu_number_76;
/* match: the records are written through these two for the same reason. */
static inline LevelRecord *GetWeaponRecord(s32 index) {
    return &weapon_level_records[index];
}
static inline LevelRecord *GetItemRecord(s32 index) {
    return &worn_item_level_records[index];
}
extern f32 smith_confirm_list_model_position[4];
extern s32 smith_confirm_list_panel_sound_pending;
extern f32 smith_confirm_list_model_angle;
extern f32 smith_item_screen_model_position[4];
extern u32 tune_s_part_count;              /* how many parts the screen has */
extern SpritePacket *tune_s_parts;
extern f32 smith_item_screen_model_angle;
extern f32 smith_collect_list_model_position[4];
extern s32 smith_collect_list_panel_sound_pending;
extern f32 smith_collect_list_model_angle;
/* match: inline like the level lookups, so the record index is not folded into the table address */
static inline void ClearWeaponMark(s32 index) {
    weapon_level_records[index].in_smith_rings = 0;
}
static inline void ClearItemMark(s32 index) {
    worn_item_level_records[index].in_smith_rings = 0;
}
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 Smith_CollectListScrollUp(void) {
    smith_collect_list_top[0] = smith_collect_list_top[0]->prev;
    smith_collect_list_page_mark[0] = smith_collect_list_page_mark[0]->prev;
    smith_collect_list_cursor[0] = smith_collect_list_top[0];
    return smith_collect_list_top[0] != item_list_last_page;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 Smith_CollectListScrollDown(void) {
    smith_collect_list_top[0] = smith_collect_list_top[0]->next;
    smith_collect_list_page_mark[0] = smith_collect_list_page_mark[0]->next;
    smith_collect_list_cursor[0] = smith_collect_list_top[0];
    return smith_collect_list_top[0] != item_list_first;
}
extern s32 save_screen_run_step;    /* which of save_screen_run_steps runs */
extern int save_screen_step;        /* which of save_screen_steps runs */
extern int save_screen_pad_change_frames; /* counts 20 frames without buttons after the pad type changed */
extern u8 menu_draw_color_alpha;    /* the alpha byte of menu_draw_color */
extern u8 model_draw_color_alpha;   /* the alpha byte of model_draw_color */
extern HandlerContext save_screen_context; /* what the save screen steps get */
extern int (*save_screen_handler)(); /* the screen SaveScreen_RunHandler runs */
extern float pulse_sprite_alpha;
s32 SaveCard_Open(void);
s32 SaveCard_StartCheck(s8);
extern s32 save_line_slots[3];      /* the save slot each of the three lines shows */
extern s32 save_line_write_slots[3]; /* the save slot a save from each line is written to */
extern u32 slot_part_count;
extern SpritePacket *slot_parts;
extern s8 system_screen_state;
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
extern u8 save_image[];
extern u32 data_part_count;
extern u32 mc_part_count;
extern SpritePacket *data_parts;
extern SpritePacket *mc_parts;
extern s32 save_lines_reorder_pending; /* TRUE while a save into an empty slot runs: the lines are reordered after it */
extern s32 menu_number_53;
extern s32 menu_number_78;
extern s32 menu_number_83;
extern s32 menu_number_84;
extern s32 menu_number_85;
extern s32 menu_number_86;
extern s32 menu_number_87;
extern s32 menu_number_88;
extern s32 menu_number_89;
extern s32 menu_number_90;
extern s32 menu_number_91;
extern s32 menu_number_92;
extern s32 menu_number_93;
void Game_WriteSaveImage(void);
s32 SaveCard_StartSave(s32, s32, s32 *);
s8 SaveCard_GetSaveState(s32);
s32 SaveCard_HasRoomFor(s32, s32, s32);
s32 SaveCard_GetFreeClusters(s32);
// match: called as a void function, so the result register is free at once
#define SET_PART_MODE(mode) ((void (*)(u8))Menu_SetBlendMode)(mode)
// match: this file hands Menu_SetBlendMode a signed byte
#define SET_PART_MODE_OF(packet) ((void (*)(s8))Menu_SetBlendMode)((s8)(packet)->blend_mode)

static s32 Trade_CopyFrameToTexture(s32 unused_step);
static int Trade_RunStep(int value);
static s32 Trade_Start(HandlerContext *context);
static int Trade_RunHandler(HandlerContext *context);
static int Trade_CutModelChain(HandlerContext *context);
static void Trade_ChainModels(s32 unused_arg);
static f32 Trade_PulseValue(s32 low, s32 high, f32 value);
static void Trade_UpdateSelectScreen(HandlerContext *context);
static void Trade_UpdateStockList(HandlerContext *context);
static void Trade_UpdateTakeAmount(HandlerContext *context);
static void Trade_UpdateOwnedList(HandlerContext *context);
static void Trade_UpdateGiveAmount(HandlerContext *context);
static void Trade_SetModelNames(void);
static s32 Trade_MoveListCursor(HandlerContext *context);
static void Smith_BuildItemRings(s32 mode);
static s32 Smith_Start(HandlerContext *context);
static s32 Smith_CopyFrameToTexture(s32 unused_step);
static int Smith_RunStep(int value);
static int Smith_RunHandler(HandlerContext *context);
static int Smith_CutModelChain(HandlerContext *context);
static void Smith_ChainModels(s32 unused_arg);
static void Smith_UpdateSelectScreen(HandlerContext *context);
static void Smith_SetModelNames(void);
static void Smith_UpdateUpgradeList(HandlerContext *context);
static void Smith_UpdateUpgradeConfirm(HandlerContext *context);
static void Smith_UpdateListWithConfirm(HandlerContext *context);
static void Smith_UpdateItemScreen(HandlerContext *context);
static void Smith_UpdateCollectList(HandlerContext *context);
static s32 Smith_MoveListCursor(HandlerContext *context);
static s32 SaveScreen_DrawBackground(int unused);
static int SaveScreen_RunStep(int value);
static int SaveScreen_Start(HandlerContext *context);
static int SaveScreen_RunHandler(HandlerContext *context);
static int SaveScreen_CutModelChain(HandlerContext *context);
static void SaveScreen_UpdateCardSelect(void);
static void SaveScreen_UpdateSaveList(void);

/* the six value tables (trade_value_tables), numbered from 1 like trade_stock_1...: trade_values_N is the one
   of trade table N - 1, the number the script hands Trade_Run */
extern TradeValue trade_values_1[];
extern TradeValue trade_values_2[];
extern TradeValue trade_values_3[];
extern TradeValue trade_values_4[];
extern TradeValue trade_values_5[];
extern TradeValue trade_values_6[];
extern u8 trade_stock_1[];
extern u8 trade_stock_2[];
extern u8 trade_stock_3[];
extern u8 trade_stock_4[];
extern u8 trade_stock_5[];

// match: only a named section keeps a table of eight bytes out of small data; retail has these in .rodata
// and reaches them without the global pointer
#define IN_RODATA __attribute__((section(".rodata")))

/* the functions of the trade screens: the two steps of Trade_Run's loop, the steps of the screen itself, and
   the handlers of its two lists */
static int (*const trade_run_steps[2])(s32) IN_RODATA = {
    Trade_CopyFrameToTexture,
    Trade_RunStep,
};
static int (*const trade_steps[4])(HandlerContext *) IN_RODATA = {
    Trade_Start,
    Trade_RunHandler,
    Trade_CutModelChain,
    NULL,
};
static void (*const trade_list_handlers[2])(HandlerContext *) IN_RODATA = {
    Trade_UpdateStockList,
    Trade_UpdateOwnedList,
};
static const TradeValueTables trade_value_tables = { { trade_values_1, trade_values_2, trade_values_3, trade_values_4, trade_values_5, trade_values_6 } };
static const TradeStockTables trade_stock_tables = { { trade_stock_1, trade_stock_2, trade_stock_3, trade_stock_4, trade_stock_5 } };
static const TradeGroupBases item_group_bases = { { 150, 0, 220, 220, 220, 220, 220, 220, 220, 220, 0, 0, 0 } };
static const TradeValueSlots trade_value_slots = {
    { &menu_number_87, &menu_number_88, &menu_number_89, &menu_number_90, &menu_number_91, &menu_number_92, &menu_number_93 }
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

    item_list_top[0] = NULL;
    item_list_page_mark[0] = NULL;
    handover_list_first = NULL;
    handover_list_last_page = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 150; i++) {
        if (item_counts[i] != 0 && !(item_name_defs[i].state & 2) && values.values[table][i].give_value != 0) {
            LINK_ITEM(&common_item_entries[i], item_list_page_mark)
        }
    }
    LINK_OWNED_GROUP(weapon_entries, weapon_level_records, 0, 150, 70)
    LINK_OWNED_GROUP(shield_entries, worn_item_level_records, 0, 220, 30)
    LINK_OWNED_GROUP(helm_entries, worn_item_level_records, 30, 250, 25)
    LINK_OWNED_GROUP(armor_entries, worn_item_level_records, 55, 275, 25)
    LINK_OWNED_GROUP(gauntlet_entries, worn_item_level_records, 80, 300, 25)
    LINK_OWNED_GROUP(boot_entries, worn_item_level_records, 105, 325, 25)
    LINK_OWNED_GROUP(ring_entries, worn_item_level_records, 130, 350, 10)
    LINK_OWNED_GROUP(bracelet_entries, worn_item_level_records, 140, 360, 10)
    LINK_OWNED_GROUP(amulet_entries, worn_item_level_records, 150, 370, 10)
    handover_list_count = count;
    if (first != NULL) {
        last->prev = first;
        item_list_cursor[0] = item_list_top[0] = first;
        first->next = last;
        if (item_list_page_mark[0] == NULL) {
            item_list_page_mark[0] = last->next;
        } else {
            item_list_page_mark[0] = item_list_page_mark[0]->next;
        }
        handover_list_first = first;
        handover_list_last_page = last;
        for (i = 5; i >= 0; i--) {
            handover_list_last_page = handover_list_last_page->next;
        }
    } else {
        item_list_cursor[0] = NULL;
    }

    trade_stock_list_top[0] = NULL;
    trade_stock_list_page_mark[0] = NULL;
    owned_list_first = NULL;
    owned_list_last_page = NULL;
    stock_counts = stock.counts[table];
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 0x17C; i++) {
        if (stock_counts[i] != 0 && values.values[table][i].take_value != 0 && !(item_name_defs[i].state & 2)) {
            LINK_ITEM(&all_item_entries[i], trade_stock_list_page_mark)
        }
    }
    owned_list_count = count;
    if (first != NULL) {
        last->prev = first;
        trade_stock_list_cursor[0] = trade_stock_list_top[0] = first;
        first->next = last;
        if (trade_stock_list_page_mark[0] == NULL) {
            trade_stock_list_page_mark[0] = last->next;
        } else {
            trade_stock_list_page_mark[0] = trade_stock_list_page_mark[0]->next;
        }
        owned_list_first = first;
        owned_list_last_page = last;
        for (i = 5; i >= 0; i--) {
            owned_list_last_page = owned_list_last_page->next;
        }
    } else {
        trade_stock_list_cursor[0] = NULL;
    }
}

/* Runs the screen loop of the handler trade_run_step selects until it returns 0, watching the pad type for changes. */
void Trade_Run(s32 value) {
    modal_input_locked = 1;
    trade_amount_down_locked = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    Vram_Clear(0x3D00, 0x3F00);
    Sound_MutePositional();
    Sound_UpdateTagged();
    menu_mode = 0;
    modal_pad_mode = modal_pad_mode_last = Pad_GetMode(0, 0);
    while (trade_run_steps[trade_run_step](value) != 0) {
        sceGsSyncPath(0, 0);
        modal_even_field = Menu_EndFrame(1);
        Input_Update();
        modal_pad_mode = Pad_GetMode(0, 0);
        if (modal_pad_mode != modal_pad_mode_last) {
            trade_pad_change_frames = 1;
        }
        modal_pad_mode_last = modal_pad_mode;
        MenuPad_Update();
        if (trade_pad_change_frames > 0) {
            trade_pad_change_frames++;
            if (trade_pad_change_frames >= 21) {
                trade_pad_change_frames = 0;
            } else {
                menu_buttons = 0;
            }
        }
    }
    sceGsSyncPath(0, 0);
    while (sceGsSyncV(0) != 0) {
    }
    Sound_UnmutePositional();
    MenuHeap_FreeToMark();
    trade_run_step = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}

/* Copies the shown frame buffer into a work buffer and draws that back into both frame buffers, then steps the screen counter. */
static s32 Trade_CopyFrameToTexture(s32 unused_step) {
    f32 unused[28]; // match: 0x70 bytes of locals nothing uses
    u32 texture_base; /* where the shown buffer is as a texture: 0x1180 when it is the second one */
    u32 *packet;

    packet = MenuDraw_NextPacket();
    texture_base = 0x1180;
    if (context1_buffer_half == 0) {
        texture_base = 0;
    }
    packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
    packet[2] = 0xE;
    packet[3] = 0;
    *(u64 *)&packet[4] = (menu_first_draw_env[0] & ~0x1FFL) | 0x192;
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
    *(u64 *)&packet[48] = menu_first_draw_env[0];
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
    *(u64 *)&packet[72] = menu_first_draw_env[46];
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
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 26;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    sceGsSyncPath(0, 0);
    trade_run_step++;
    return 1;
}

/* Calls the handler selected by trade_step with the context at trade_context, storing value in it first. */
static int Trade_RunStep(int value) {
    int (*handler)(HandlerContext *);

    Menu_StepModelLoads(0);
    handler = trade_steps[trade_step];
    trade_context.arg = value;
    if (handler != NULL) {
        return handler(&trade_context);
    }
    return 1;
}

/* Start-up step of the trade screens: loads the textures and blocks, builds the item lists of table context->arg and
   picks the screen's handler (table 4 goes straight to the handler in trade_list_handlers with the five link nodes set up). */
static s32 Trade_Start(HandlerContext *context) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;

    Packet_SwapBuffers();
    Packet_OnFrameStart();
    TextureTable_QueueAll((TextureTableHeader *)Message_GetFontTexture(menu_heap));
    GsDraw_QueueTexFlush();
    Packet_OnFrameEnd();
    Packet_SendChain();
    ((s32 (*)(void))Packet_WaitForSend)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < menu_file_block_count; i++) {
        block = &menu_file_blocks[i];
        MenuTexture_Upload(block->data, block);
    }
    MenuArena_Init(trade_menu_arena);
    packet = MenuDraw_NextPacket();
    packet[0] = 0x8001;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 0x3F;
    packet[7] = 0;
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 2;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    Trade_BuildItemRings(context->arg);
    Menu_InitLoadBuffers();
    pulse_sprite_falling = 0;
    pulse_value_falling = 0;
    if (context->arg != 4) {
        trade_handler = Trade_UpdateSelectScreen;
    } else {
        trade_handler = trade_list_handlers[0];
        item_list_first = owned_list_first;
        item_list_last_page = owned_list_last_page;
        item_list_count = owned_list_count;
        Trade_ChainModels(0);
        context->active_list = 0;
    }
    memset(screen_part_alphas, 0, 0x400);
    memset(panel_part_alphas, 0, 0x400);
    memset(message_part_alphas, 0, 0x400);
    menu_draw_color.blue = 0x80;
    menu_draw_color.green = 0x80;
    menu_draw_color.red = 0x80;
    menu_draw_color.alpha = 0;
    model_draw_color.blue = 0x80;
    model_draw_color.green = 0x80;
    model_draw_color.red = 0x80;
    model_draw_color.alpha = 0;
    trade_step++;
    return TRUE;
}

/* Runs the handler in trade_handler with the context this step was given. */
static int Trade_RunHandler(HandlerContext *context) {
    trade_handler(context);
    return 1;
}

/* Cuts the chain after the first link node (and after the node it points at); with no chain, resets trade_step. */
static int Trade_CutModelChain(HandlerContext *context) {
    LinkNode *node = model_load_chain;
    LinkNode *other;

    if (node == NULL) {
        trade_step = 0;
        return 0;
    }
    other = node->target_node;
    node->next = NULL;
    if (other != NULL) {
        other->next = NULL;
    }
    return 1;
}

/* Rebuilds the five link nodes for the selected item and chains them, when there is one. */
static void Trade_ChainModels(s32 unused_arg) {
    if (trade_stock_list_cursor[0] != NULL) {
        Trade_SetModelNames();
        Menu_TakeLoadBuffer(&model_load_shown);
        Menu_TakeLoadBuffer(&model_load_up);
        Menu_TakeLoadBuffer(&model_load_down);
        Menu_TakeLoadBuffer(&model_load_left);
        Menu_TakeLoadBuffer(&model_load_right);
        model_load_shown.next = &model_load_up;
        model_load_shown.callback = Menu_StepModelLoad;
        model_load_shown.done = 0;
        model_load_up.next = &model_load_down;
        model_load_up.callback = Menu_StepModelLoad;
        model_load_up.done = 0;
        model_load_up.stage = 0;
        model_load_down.next = &model_load_left;
        model_load_down.callback = Menu_StepModelLoad;
        model_load_down.done = 0;
        model_load_down.stage = 0;
        model_load_left.next = &model_load_right;
        model_load_left.callback = Menu_StepModelLoad;
        model_load_left.done = 0;
        model_load_left.stage = 0;
        model_load_right.next = NULL;
        model_load_right.callback = Menu_StepModelLoad;
        model_load_right.done = 0;
        model_load_shown.stage = model_load_right.stage = 0;
        model_load_chain = &model_load_shown;
    }
}

/* Moves value back and forth between low and high, one sixtieth of the span per call; pulse_value_falling is the direction. */
static f32 Trade_PulseValue(s32 low, s32 high, f32 value) {
    if (pulse_value_falling == 0) {
        value += (f32)(high - low) / 60.0f;
        if (value > high) {
            value = high;
            pulse_value_falling = 1;
        }
    } else {
        value -= (f32)(high - low) / 60.0f;
        if (value < low) {
            pulse_value_falling = 0;
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

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_UP) {
            Sound_PlayMenu(1);
            context->active_list--;
        } else if (menu_buttons & PAD_DOWN) {
            Sound_PlayMenu(1);
            context->active_list++;
        }
    }
    context->active_list = (context->active_list + 2) % 2;

    bar_drawn = 0;
    for (i = 0; i < preshop_part_count; i++) {
        if (bar_drawn == 0 && preshop_parts[i].has_cursor_bar == 1) {
            bar.first.f.lo16 = 0x76C0;
            bar.first.f.mid16 = context->active_list * 0xE0 + 0x7F50;
            bar.first.f.mid24 = 900000;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x12C0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xD0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar_drawn = 1;
            if (trade_screen_state == 1) {
                screen_part_alphas[0xFD] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[0xFD]);
            } else {
                screen_part_alphas[0xFD] = 0.0f;
            }
            bar.alpha = (s32)screen_part_alphas[0xFD];
            MenuDraw_Bar(&bar);
        }
        kind = preshop_parts[i].handler_index;
        SELECT_PART((s8)preshop_parts[i].blend_mode);
        MenuSprite_Draw((int)&preshop_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
        SELECT_PART(0);
    }

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            if (context->active_list == 0) {
                if (trade_stock_list_cursor[0] != NULL) {
                    Sound_PlayMenu(0);
                    trade_screen_state = 2;
                    modal_input_locked = 1;
                } else {
                    Sound_PlayMenu(3);
                }
            } else if (context->active_list == 1) {
                if (item_list_cursor[0] != NULL) {
                    Sound_PlayMenu(0);
                    modal_input_locked = 1;
                    trade_screen_state = 3;
                } else {
                    Sound_PlayMenu(3);
                }
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            trade_screen_state = 4;
            modal_input_locked = 1;
        }
    }

    switch (trade_screen_state) {
    case 0:
        screen_part_alphas[0xFD] = 16.0f;
        for (i = 0; i < preshop_part_count; i++) {
            screen_part_alphas[i] += (f32)preshop_parts[i].alpha / 10.0f;
            if ((f32)preshop_parts[i].alpha < screen_part_alphas[i]) {
                for (i = 0; i < preshop_part_count; i++) {
                    full_alpha = preshop_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                modal_input_locked = 0;
                trade_screen_state = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 4:
        screen_part_alphas[0xFD] -= 1.6f;
        if (screen_part_alphas[0xFD] < 0.0f) {
            screen_part_alphas[0xFD] = 0.0f;
        }
        for (i = 0; i < preshop_part_count; i++) {
            screen_part_alphas[i] -= (f32)preshop_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                trade_handler = trade_list_handlers[context->active_list];
                if (trade_screen_state == 2) {
                    item_list_first = owned_list_first;
                    item_list_last_page = owned_list_last_page;
                    item_list_count = owned_list_count;
                    Trade_ChainModels(0);
                } else if (trade_screen_state == 3) {
                    item_list_first = handover_list_first;
                    item_list_last_page = handover_list_last_page;
                    item_list_count = handover_list_count;
                    Menu_ChainItemListModels(0);
                } else {
                    trade_step++;
                }
                trade_screen_state = 0;
                context->active_list = 0;
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

    if (trade_stock_list_cursor[0] == NULL) {
        trade_handler = Trade_UpdateSelectScreen;
        return;
    }
    if (trade_screen_state < 2) {
        Trade_MoveListCursor(context);
    }
    record = trade_stock_list_cursor[0]->item_number - bases.first_item[trade_stock_list_cursor[0]->group];
    if (context->arg == 4) {
        switch (item_name_defs[trade_stock_list_cursor[0]->item_number].upgrade_material_kind & ITEM_UPGRADE_MATERIAL_MASK) {
        case 1:
            menu_number_94 = item_counts[ITEM_BONE];
            break;
        case 2:
            menu_number_94 = item_counts[ITEM_SCALE];
            break;
        case 3:
            menu_number_94 = item_counts[ITEM_CLAW];
            break;
        }
    }
    if (trade_stock_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[record].level_index + 1;
    }
    for (i = 0; i < shop_part_count; i++) {
        kind = shop_parts[i].handler_index;
        SELECT_PART((s8)shop_parts[i].blend_mode);
        if (kind == 3) {
            switch (shop_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
                break;
            case 1:
                break;
            case 2:
                if (context->arg != 4) {
                    MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 2, (int)screen_part_alphas[i]);
                }
                break;
            case 3:
                if (trade_stock_list_cursor[0]->group != 12) {
                    MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 3, (int)screen_part_alphas[i]);
                }
                break;
            case 4:
                if (context->arg == 4) {
                    MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 4, (int)screen_part_alphas[i]);
                }
                break;
            }
        } else if (context->arg != 4) {
            MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
        } else {
            MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 5, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }

    bar_node = trade_stock_list_top[0];
    for (bar_row = 0; bar_row < 7; bar_row++, bar_node = bar_node->prev) {
        if (bar_node == trade_stock_list_cursor[0]) {
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
            if (trade_screen_state == 1) {
                screen_part_alphas[0xFD] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[0xFD]);
            }
            bar.alpha = (s32)screen_part_alphas[0xFD];
            MenuDraw_Bar(&bar);
            break;
        }
    }

    if (context->arg == 4) {
        position.raw = 0x000DBBA079E08A30L;
        switch (item_name_defs[trade_stock_list_cursor[0]->item_number].upgrade_material_kind & ITEM_UPGRADE_MATERIAL_MASK) {
        case 1:
            label = bones_label;
            break;
        case 2:
            position.raw = 0x000DBBA079E089C0L;
            label = scales_label;
            break;
        case 3:
            label = claws_label;
            break;
        default:
            position.f.lo16 = 0x89C0;
            label = scales_label;
            break;
        }
        MenuText_SetPen(position);
        MenuText_DrawWrapped(label, 10);
    }

    MenuText_SetColor(menu_draw_color);
    row_position.raw = 0x0C7A12007C307080L;
    for (row = 0, node = trade_stock_list_top[0]; row < 7; node = node->prev, row++) {
        item = &item_name_defs[node->item_number];
        MenuText_SetPen(row_position);
        MenuText_DrawLine((u16 *)item, 0x1A);
        row_position.f.mid16 += 0xB0;
        if (trade_stock_list_page_mark[0]->prev == node) {
            break;
        }
    }

    /* the values of the (up to seven) listed items; -1 for the rows left empty */
    {
        // match: the list head is read before the table is copied, and the loop reads it again
        MenuItem *first_node = trade_stock_list_top[0];
        TradeValueSlots slots = trade_value_slots;
        MenuItem *next_node;

        i = 0;
        *slots.shown_value[i] = values.values[context->arg][first_node->item_number].take_value;
        value_node = first_node->prev;
        if (first_node != value_node) {
            do {
                i++;
                if (i >= 7) {
                    break;
                }
                *slots.shown_value[i] = values.values[context->arg][value_node->item_number].take_value;
                next_node = value_node->prev;
                value_node = next_node;
            } while (trade_stock_list_top[0] != next_node);
        }
        for (i++; i < 7; i++) {
            *slots.shown_value[i] = -1;
        }
    }

    /* the chosen item's model: scaled to 0.8, turned by trade_stock_list_model_angle (kept within -pi..pi) and moved to trade_stock_list_model_position */
    {
        // match: declared in this block, after the one above, so they come after `slots` in the frame
        f32 local_matrix[4][4];
        f32 view_matrix[4][4];

        memset(view_matrix, 0, 0x40);
        view_matrix[2][2] = view_matrix[1][1] = view_matrix[0][0] = 0.8f;
        view_matrix[3][3] = 1.0f;
        sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        trade_stock_list_model_angle += PI;
        trade_stock_list_model_angle -= (s32)(trade_stock_list_model_angle / TWO_PI) * TWO_PI;
        trade_stock_list_model_angle -= PI;
        sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)view_matrix, trade_stock_list_model_angle);
        sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, trade_stock_list_model_position);
        Menu_DrawItemModel((f32 *)local_matrix);
    }

    if (modal_input_locked == 0) {
        if ((menu_buttons & PAD_TRIANGLE) && Menu_IsEquipSlotFilled(record, trade_stock_list_cursor[0]->group)) {
            trade_stock_list_panel_sound_pending = 1;
            if (trade_panel_shown == 0) {
                trade_panel_shown = 1;
            } else {
                trade_panel_state = 2;
            }
        }
    }

    if (trade_panel_shown != 0) {
        // match: groups 1 and 10 have no case, so packets and count are read unset by the fade below
        switch (trade_stock_list_cursor[0]->group) {
        case 11:
        case 12:
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (trade_stock_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            trade_stock_list_panel_sound_pending = 0;
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_71 = worn_item_level_records[record].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[trade_stock_list_cursor[0]->item_number].weight_tenths_of_pound;
            glow = player_weapon[trade_stock_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_g_parts, shop_g_part_count)
            }
            DRAW_PACKETS(shop_g_parts, shop_g_part_count, glow)
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (trade_stock_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            trade_stock_list_panel_sound_pending = 0;
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_54 = item_name_defs[trade_stock_list_cursor[0]->item_number].weight_tenths_of_pound;
            glow = player_weapon[trade_stock_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_g2_parts, shop_g2_part_count)
            }
            DRAW_PACKETS(shop_g2_parts, shop_g2_part_count, glow)
            packets = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (trade_stock_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            trade_stock_list_panel_sound_pending = 0;
            menu_number_44 = ScaleValue(weapon_level_records[record].gained_values[0] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[0], weapon_level_records[record].level, 100);
            menu_number_45 = ScaleValue(weapon_level_records[record].gained_values[1] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[1], weapon_level_records[record].level, 100);
            menu_number_46 = ScaleValue(weapon_level_records[record].gained_values[2] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[2], weapon_level_records[record].level, 100);
            menu_number_47 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[3], weapon_level_records[record].level, 100);
            menu_number_48 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[4], weapon_level_records[record].level, 100);
            menu_number_49 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[5], weapon_level_records[record].level, 100);
            menu_number_50 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[6], weapon_level_records[record].level, 100);
            menu_number_51 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[7], weapon_level_records[record].level, 100);
            menu_number_52 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[8], weapon_level_records[record].level, 100);
            menu_number_70 = (s32)(weapon_defs[record].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[record].level;
            menu_number_54 = item_name_defs[trade_stock_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[trade_stock_list_cursor[0]->group];
            menu_number_73 = 100;
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_a_parts, shop_a_part_count)
            }
            DRAW_PACKETS(shop_a_parts, shop_a_part_count, 0)
            packets = shop_a_parts;
            count = shop_a_part_count;
            break;
        }

        switch (trade_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += packets[i].alpha / 10.0f;
                if (packets[i].alpha < panel_part_alphas[i]) {
                    FILL_FADE(panel_part_alphas, packets, count)
                    trade_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= packets[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    if (trade_panel_state == 2) {
                        trade_panel_shown = 0;
                    }
                    trade_panel_state = 0;
                    break;
                }
            }
            break;
        }
    }

    current = trade_stock_list_cursor[0];
    texts = Message_GetFontText(menu_heap);
    text_number = current->item_number + MSG_ITEM_TEXT_FIRST + 1; // match: a variable, so the offset is added before the index is scaled
    text_offset = ((s32 *)texts)[text_number];
    MenuText_SetColor(menu_draw_color);
    position.raw = 0x000A875083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            Sound_PlayMenu(0);
            trade_screen_state = 2;
            trade_panel_state = 3;
            modal_input_locked = 1;
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            if (context->arg != 4) {
                trade_screen_state = 3;
            } else {
                trade_screen_state = 4;
            }
            trade_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        }
    }
    if (trade_screen_state == 2 && model_load_chain == NULL) {
        trade_screen_state = 5;
    }
    if (trade_screen_state == 3 && model_load_chain == NULL) {
        trade_screen_state = 6;
    }
    if (trade_screen_state == 4 && model_load_chain == NULL) {
        trade_screen_state = 7;
    }

    switch (trade_screen_state) {
    case 0:
        screen_part_alphas[0xFF] += 12.8f;
        if (screen_part_alphas[0xFF] > 128.0f) {
            screen_part_alphas[0xFF] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        screen_part_alphas[0xFD] += 1.6f;
        if (screen_part_alphas[0xFD] > 16.0f) {
            screen_part_alphas[0xFD] = 16.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] += shop_parts[i].alpha / 10.0f;
            if (shop_parts[i].alpha < screen_part_alphas[i]) {
                modal_input_locked = 0;
                trade_screen_state = 1;
                FILL_FADE(screen_part_alphas, shop_parts, shop_part_count)
                return;
            }
        }
        break;
    case 5:
    case 6:
    case 7:
        screen_part_alphas[0xFF] -= 12.8f;
        if (screen_part_alphas[0xFF] < 0.0f) {
            screen_part_alphas[0xFF] = 0.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        screen_part_alphas[0xFE] -= 12.8f;
        if (screen_part_alphas[0xFE] < 0.0f) {
            screen_part_alphas[0xFE] = 0.0f;
        }
        model_draw_color.alpha = (s32)screen_part_alphas[0xFE];
        screen_part_alphas[0xFD] -= 1.6f;
        if (screen_part_alphas[0xFD] < 0.0f) {
            screen_part_alphas[0xFD] = 0.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] -= shop_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (trade_screen_state == 5) {
                    trade_handler = Trade_UpdateTakeAmount;
                    context->active_list = 1;
                    trade_amount_down_locked = 0;
                } else if (trade_screen_state == 6) {
                    context->active_list = 0;
                    trade_handler = Trade_UpdateSelectScreen;
                    Menu_ResetLoadBuffers(0);
                } else if (trade_screen_state == 7) {
                    context->active_list = 0;
                    trade_step++;
                    Menu_ResetLoadBuffers(0);
                }
                trade_screen_state = 0;
                trade_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
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

    record = trade_stock_list_cursor[0]->item_number - bases.first_item[trade_stock_list_cursor[0]->group];
    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_RIGHT) {
            Sound_PlayMenu(1);
            context->active_list++;
        } else if ((menu_buttons & PAD_LEFT) && trade_amount_down_locked == 0) {
            Sound_PlayMenu(1);
            context->active_list--;
        }
    }
    if (context->active_list + item_counts[trade_stock_list_cursor[0]->item_number] >= 100) {
        if (item_counts[trade_stock_list_cursor[0]->item_number] != 99) {
            context->active_list = 1;
        } else {
            context->active_list = 0;
        }
    }
    if (stock.counts[context->arg][trade_stock_list_cursor[0]->item_number] < context->active_list) {
        context->active_list = 1;
    } else if (context->active_list <= 0) {
        context->active_list = stock.counts[context->arg][trade_stock_list_cursor[0]->item_number];
        if (context->active_list + item_counts[trade_stock_list_cursor[0]->item_number] >= 100) {
            context->active_list = 99 - item_counts[trade_stock_list_cursor[0]->item_number];
        }
    }
    if (context->arg != 4) {
        menu_number_79 = item_counts[trade_stock_list_cursor[0]->item_number];
        menu_number_80 = stock.counts[context->arg][trade_stock_list_cursor[0]->item_number];
        menu_number_81 = context->active_list;
        menu_number_82 = context->active_list * values.values[context->arg][trade_stock_list_cursor[0]->item_number].take_value;
    } else {
        menu_number_79 = item_counts[trade_stock_list_cursor[0]->item_number];
        menu_number_80 = stock.counts[4][trade_stock_list_cursor[0]->item_number];
        menu_number_81 = context->active_list;
        menu_number_82 = context->active_list * values.values[4][trade_stock_list_cursor[0]->item_number].take_value;
        switch (item_name_defs[trade_stock_list_cursor[0]->item_number].upgrade_material_kind & ITEM_UPGRADE_MATERIAL_MASK) {
        case 1:
            menu_number_74 = item_counts[ITEM_BONE];
            break;
        case 2:
            menu_number_74 = item_counts[ITEM_SCALE];
            break;
        case 3:
            menu_number_74 = item_counts[ITEM_CLAW];
            break;
        }
    }
    if (menu_number_80 >= 100) {
        menu_number_80 = 99;
    }
    if (trade_stock_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[record].level_index + 1;
    }
    for (i = 0; i < shop_part_count; i++) {
        kind = shop_parts[i].handler_index;
        SELECT_PART((s8)shop_parts[i].blend_mode);
        if (kind == 3) {
            switch (shop_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
                break;
            case 1:
                MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
                break;
            case 2:
                break;
            case 3:
                if (trade_stock_list_cursor[0]->group != 12) {
                    MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 3, (int)screen_part_alphas[i]);
                }
                break;
            }
        } else if (context->arg != 4) {
            MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 2, (int)screen_part_alphas[i]);
        } else {
            MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 6, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }

    MenuText_SetColor(menu_draw_color);
    memset(view_matrix, 0, 0x40);
    view_matrix[2][2] = view_matrix[1][1] = view_matrix[0][0] = 0.8f;
    view_matrix[3][3] = 1.0f;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = trade_take_amount_model_angle + PI;
    trade_take_amount_model_angle = angle - (s32)(angle / TWO_PI) * TWO_PI - PI;
    sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)view_matrix, trade_take_amount_model_angle);
    sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, trade_take_amount_model_position);
    Menu_DrawItemModel((f32 *)local_matrix);

    if (modal_input_locked == 0 && (menu_buttons & PAD_TRIANGLE) && Menu_IsEquipSlotFilled(record, trade_stock_list_cursor[0]->group)) {
        trade_take_amount_panel_sound_pending = 1;
        if (trade_panel_shown == 0) {
            trade_panel_shown = 1;
        } else {
            trade_panel_state = 2;
        }
    }
    if (trade_panel_shown != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
        switch (trade_stock_list_cursor[0]->group) {
        case 11:
        case 12:
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (trade_take_amount_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            trade_take_amount_panel_sound_pending = 0;
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_71 = worn_item_level_records[record].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[trade_stock_list_cursor[0]->item_number].weight_tenths_of_pound;
            kind = player_weapon[trade_stock_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_g_parts, shop_g_part_count)
            }
            DRAW_PACKETS(shop_g_parts, shop_g_part_count, kind)
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (trade_take_amount_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            trade_take_amount_panel_sound_pending = 0;
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_54 = item_name_defs[trade_stock_list_cursor[0]->item_number].weight_tenths_of_pound;
            kind = player_weapon[trade_stock_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_g2_parts, shop_g2_part_count)
            }
            DRAW_PACKETS(shop_g2_parts, shop_g2_part_count, kind)
            packets = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (trade_take_amount_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            trade_take_amount_panel_sound_pending = 0;
            menu_number_44 = ScaleValue(weapon_level_records[record].gained_values[0] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[0], weapon_level_records[record].level, 100);
            menu_number_45 = ScaleValue(weapon_level_records[record].gained_values[1] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[1], weapon_level_records[record].level, 100);
            menu_number_46 = ScaleValue(weapon_level_records[record].gained_values[2] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[2], weapon_level_records[record].level, 100);
            menu_number_47 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[3], weapon_level_records[record].level, 100);
            menu_number_48 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[4], weapon_level_records[record].level, 100);
            menu_number_49 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[5], weapon_level_records[record].level, 100);
            menu_number_50 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[6], weapon_level_records[record].level, 100);
            menu_number_51 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[7], weapon_level_records[record].level, 100);
            menu_number_52 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[8], weapon_level_records[record].level, 100);
            menu_number_70 = (s32)(weapon_defs[record].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[record].level;
            menu_number_54 = item_name_defs[trade_stock_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[trade_stock_list_cursor[0]->group];
            menu_number_73 = 100;
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_a_parts, shop_a_part_count)
            }
            DRAW_PACKETS(shop_a_parts, shop_a_part_count, 0)
            packets = shop_a_parts;
            count = shop_a_part_count;
            break;
        }

        switch (trade_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += (f32)packets[i].alpha / 10.0f;
                if ((f32)packets[i].alpha < panel_part_alphas[i]) {
                    FILL_FADE(panel_part_alphas, packets, count)
                    trade_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= (f32)packets[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    trade_panel_state = 0;
                    trade_panel_shown = 0;
                    break;
                }
            }
            break;
        }
    }

    node = trade_stock_list_top[0];
    MenuText_SetColor(menu_draw_color);
    position.raw = 0x0C7A12007C307080L;
    for (row = 0; row < 7; row++) {
        item = &item_name_defs[node->item_number];
        MenuText_SetPen(position);
        MenuText_DrawLine((u16 *)item, 0x1A);
        position.f.mid16 += 0xB0;
        if (trade_stock_list_page_mark[0]->prev == node) {
            break;
        }
        node = node->prev;
    }

    bar_drawn = 0;
    for (i = 0; i < shop_mess_part_count; i++) {
        if (bar_drawn == 0 && shop_mess_parts[i].has_cursor_bar == 1) {
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
            if (trade_screen_state == 1) {
                screen_part_alphas[0xFD] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[0xFD]);
            }
            bar.alpha = (s32)screen_part_alphas[0xFD];
            MenuDraw_Bar(&bar);
        }
        SELECT_PART((s8)shop_mess_parts[i].blend_mode);
        kind = shop_mess_parts[i].handler_index;
        if (context->arg != 4) {
            MenuSprite_Draw((int)&shop_mess_parts[i], kind + 1000, 2, (int)message_part_alphas[i]);
        } else {
            MenuSprite_Draw((int)&shop_mess_parts[i], kind + 1000, 6, (int)message_part_alphas[i]);
        }
        SELECT_PART(0);
    }

    chosen = &item_name_defs[trade_stock_list_cursor[0]->item_number];
    MenuText_SetColor(menu_draw_color);
    position.raw = 0x0A2DC6C07DB078C0L;
    MenuText_SetPen(position);
    MenuText_DrawLineCentered(chosen, 0x18);
    if (context->arg == 4) {
        position.raw = 0x000DBBA079E08A30L;
        label_position.raw = 0x000DBBA081807C60L;
        MenuText_SetPen(position);
        switch (item_name_defs[trade_stock_list_cursor[0]->item_number].upgrade_material_kind & ITEM_UPGRADE_MATERIAL_MASK) {
        case 1:
            label = bones_label;
            break;
        case 2:
            label = scales_label;
            label_position.raw = 0x000DBBA081807C00L;
            break;
        case 3:
            label = claws_label;
            break;
        default: // match: a default that repeats case 3
            label = claws_label;
            break;
        }
        MenuText_DrawWrapped(label, 10);
        MenuText_SetPen(label_position);
        MenuText_DrawWrapped(label, 10); // match: the same label a second time
    }

    current = trade_stock_list_cursor[0];
    texts = Message_GetFontText(menu_heap);
    number = current->item_number + MSG_ITEM_TEXT_FIRST + 1; // match: its own local, or the offset is folded into the load
    text_offset = ((s32 *)texts)[number];
    position.raw = 0x000F424083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            if ((player_state.gold >= menu_number_82 && context->arg != 4) || (menu_number_74 >= menu_number_82 && context->arg == 4)) {
                Sound_PlayMenu(0);
                trade_screen_state = 3;
                trade_panel_state = 2;
                modal_input_locked = 1;
                if (model_load_chain != NULL) {
                    model_load_chain->next = NULL;
                }
            } else {
                Sound_PlayMenu(3);
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            trade_screen_state = 4;
            trade_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        }
    }
    if (trade_screen_state == 3 && model_load_chain == NULL) {
        trade_screen_state = 5;
    }
    if (trade_screen_state == 4 && model_load_chain == NULL) {
        trade_screen_state = 6;
    }

    switch (trade_screen_state) {
    case 0:
        screen_part_alphas[0xFF] += 12.8f;
        if (screen_part_alphas[0xFF] > 128.0f) {
            screen_part_alphas[0xFF] = 128.0f;
        }
        screen_part_alphas[0xFD] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        if (screen_part_alphas[0xFD] > 16.0f) {
            screen_part_alphas[0xFD] = 16.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] += (f32)shop_parts[i].alpha / 10.0f;
            if ((f32)shop_parts[i].alpha < screen_part_alphas[i]) {
                modal_input_locked = 0;
                trade_screen_state = 1;
                FILL_FADE(screen_part_alphas, shop_parts, shop_part_count)
                break;
            }
        }
        for (i = 0; i < shop_mess_part_count; i++) {
            message_part_alphas[i] += (f32)shop_mess_parts[i].alpha / 10.0f;
            if ((f32)shop_mess_parts[i].alpha < message_part_alphas[i]) {
                FILL_FADE(message_part_alphas, shop_mess_parts, shop_mess_part_count)
                break;
            }
        }
        break;
    case 5:
    case 6:
        screen_part_alphas[0xFF] -= 12.8f;
        if (screen_part_alphas[0xFF] < 0.0f) {
            screen_part_alphas[0xFF] = 0.0f;
        }
        screen_part_alphas[0xFE] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        if (screen_part_alphas[0xFE] < 0.0f) {
            screen_part_alphas[0xFE] = 0.0f;
        }
        screen_part_alphas[0xFD] -= 1.6f;
        model_draw_color.alpha = (s32)screen_part_alphas[0xFE];
        if (screen_part_alphas[0xFD] < 0.0f) {
            screen_part_alphas[0xFD] = 0.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] -= (f32)shop_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (trade_screen_state == 5) {
                    if (context->arg != 4) {
                        player_state.gold -= menu_number_82;
                        trade_handler = Trade_UpdateSelectScreen;
                        Menu_ResetLoadBuffers(0);
                    } else {
                        switch (item_name_defs[trade_stock_list_cursor[0]->item_number].upgrade_material_kind & ITEM_UPGRADE_MATERIAL_MASK) {
                        case 1:
                            item_counts[ITEM_BONE] -= menu_number_82;
                            break;
                        case 2:
                            item_counts[ITEM_SCALE] -= menu_number_82;
                            break;
                        case 3:
                            item_counts[ITEM_CLAW] -= menu_number_82;
                            break;
                        }
                        trade_handler = trade_list_handlers[0];
                    }
                    item_counts[trade_stock_list_cursor[0]->item_number] += context->active_list;
                    if (item_counts[trade_stock_list_cursor[0]->item_number] >= 100) {
                        item_counts[trade_stock_list_cursor[0]->item_number] = 99;
                    }
                    if (stock.counts[context->arg][trade_stock_list_cursor[0]->item_number] != 0xFF) {
                        stock.counts[context->arg][trade_stock_list_cursor[0]->item_number] -= context->active_list;
                    }
                    context->active_list = 0;
                    Trade_BuildItemRings(context->arg);
                    if (context->arg == 4) {
                        ((int (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing; an int result orders the loads after it
                        item_list_first = owned_list_first;
                        item_list_last_page = owned_list_last_page;
                        item_list_count = owned_list_count;
                        Trade_ChainModels(0);
                        trade_screen_state = 0;
                        context->active_list = 0;
                    }
                } else if (trade_screen_state == 6) {
                    trade_handler = trade_list_handlers[0];
                }
                trade_panel_state = 0;
                trade_screen_state = 0;
                memset(screen_part_alphas, 0, 0x400);
                break;
            }
        }
        for (i = 0; i < shop_mess_part_count; i++) {
            message_part_alphas[i] -= (f32)shop_mess_parts[i].alpha / 10.0f;
            if (message_part_alphas[i] < 0.0f) {
                memset(message_part_alphas, 0, 0x400);
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

    if (item_list_cursor[0] == NULL) {
        trade_handler = Trade_UpdateSelectScreen;
        return;
    }
    if (trade_screen_state < 2) {
        Menu_MoveItemListCursor(context, modal_input_locked);
    }
    record = item_list_cursor[0]->item_number - bases.first_item[item_list_cursor[0]->group];
    if (item_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[record].level_index + 1;
    }
    for (i = 0; i < shop_part_count; i++) {
        kind = shop_parts[i].handler_index;
        SELECT_PART((s8)shop_parts[i].blend_mode);
        if (kind == 3) {
            switch (shop_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
                break;
            case 1: // match: an empty case of its own
                break;
            case 2:
                MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 2, (int)screen_part_alphas[i]);
                break;
            case 3:
                if (item_list_cursor[0]->group != 11) {
                    MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 3, (int)screen_part_alphas[i]);
                }
                break;
            }
        } else {
            MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 3, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }

    for (bar_row = 0, bar_node = item_list_top[0]; bar_row < 7; bar_row++, bar_node = bar_node->prev) {
        if (bar_node == item_list_cursor[0]) {
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
            if (trade_screen_state == 1) {
                screen_part_alphas[0xFD] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[0xFD]);
            }
            bar.alpha = (s32)screen_part_alphas[0xFD];
            MenuDraw_Bar(&bar);
            break;
        }
    }

    MenuText_SetColor(menu_draw_color);
    position.raw = 0x0C7A12007C307080L;
    row = 0;
    memset(&view_matrix, 0, 0x40);
    view_matrix.rows[2][2] = view_matrix.rows[1][1] = view_matrix.rows[0][0] = 0.8f;
    view_matrix.rows[3][3] = 1.0f;
    node = item_list_top[0];
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    trade_owned_list_model_angle += PI;
    trade_owned_list_model_angle -= (s32)(trade_owned_list_model_angle / TWO_PI) * TWO_PI;
    trade_owned_list_model_angle -= PI;
    sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)&view_matrix, trade_owned_list_model_angle);
    sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, trade_owned_list_model_position);
    Menu_DrawItemModel((f32 *)local_matrix);

    for (; row < 7; node = node->prev, row++) {
        item = &item_name_defs[node->item_number];
        MenuText_SetPen(position);
        MenuText_DrawLine((u16 *)item, 0x1A);
        position.f.mid16 += 0xB0;
        if (item_list_page_mark[0]->prev == node) {
            break;
        }
    }

    slot_node = item_list_top[0];
    {
        TradeValueSlots slots = trade_value_slots;

        i = 0;
        do { // match: a for loop here gets its first pass peeled off
            *slots.shown_value[i] = values.values[context->arg][slot_node->item_number].give_value;
            slot_node = slot_node->prev;
            if (item_list_top[0] == slot_node) {
                break;
            }
            i++;
        } while (i < 7);
        for (i++; i < 7; i++) {
            *slots.shown_value[i] = -1;
        }
    }

    if (modal_input_locked == 0) {
        group = item_list_cursor[0]->group;
        if (group == 11) {
            group = 12;
        }
        if ((menu_buttons & PAD_TRIANGLE) && Menu_IsEquipSlotFilled(record, group)) {
            details_opened = 1;
            if (trade_panel_shown == 0) {
                trade_panel_shown = 1;
            } else {
                trade_panel_state = 2;
            }
        }
    }

    if (trade_panel_shown != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
        switch (item_list_cursor[0]->group) {
        case 11:
        case 12:
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (details_opened != 0) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_71 = worn_item_level_records[record].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            worn = player_weapon[item_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE_2(panel_part_alphas, shop_g_parts, shop_g_part_count)
            }
            DRAW_PACKETS(shop_g_parts, shop_g_part_count, worn)
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (details_opened != 0) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            worn = player_weapon[item_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE_2(panel_part_alphas, shop_g2_parts, shop_g2_part_count)
            }
            DRAW_PACKETS(shop_g2_parts, shop_g2_part_count, worn)
            packets = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (details_opened != 0) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = ScaleValue(weapon_level_records[record].gained_values[0] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[0], weapon_level_records[record].level, 100);
            menu_number_45 = ScaleValue(weapon_level_records[record].gained_values[1] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[1], weapon_level_records[record].level, 100);
            menu_number_46 = ScaleValue(weapon_level_records[record].gained_values[2] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[2], weapon_level_records[record].level, 100);
            menu_number_47 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[3], weapon_level_records[record].level, 100);
            menu_number_48 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[4], weapon_level_records[record].level, 100);
            menu_number_49 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[5], weapon_level_records[record].level, 100);
            menu_number_50 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[6], weapon_level_records[record].level, 100);
            menu_number_51 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[7], weapon_level_records[record].level, 100);
            menu_number_52 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[8], weapon_level_records[record].level, 100);
            menu_number_70 = (s32)(weapon_defs[record].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[record].level;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[item_list_cursor[0]->group];
            menu_number_73 = 100;
            if (trade_panel_state == 1) {
                FILL_FADE_2(panel_part_alphas, shop_a_parts, shop_a_part_count)
            }
            DRAW_PACKETS(shop_a_parts, shop_a_part_count, 0)
            packets = shop_a_parts;
            count = shop_a_part_count;
            break;
        }

        switch (trade_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += (f32)packets[i].alpha / 10.0f;
                if ((f32)packets[i].alpha < panel_part_alphas[i]) {
                    FILL_FADE_2(panel_part_alphas, packets, count)
                    trade_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= (f32)packets[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    if (trade_panel_state == 2) {
                        trade_panel_shown = 0;
                    }
                    trade_panel_state = 0;
                    break;
                }
            }
            break;
        }
    }

    current = item_list_cursor[0];
    texts = Message_GetFontText(menu_heap);
    text_index = current->item_number + MSG_ITEM_TEXT_FIRST + 1; // match: the index is worked out apart from the load
    text_offset = ((s32 *)texts)[text_index];
    position.raw = 0x000A875083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            Sound_PlayMenu(0);
            trade_screen_state = 2;
            trade_panel_state = 3;
            modal_input_locked = 1;
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            trade_screen_state = 3;
            trade_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        }
    }
    if (trade_screen_state == 2 && model_load_chain == NULL) {
        trade_screen_state = 4;
    }
    if (trade_screen_state == 3 && model_load_chain == NULL) {
        trade_screen_state = 5;
    }

    switch (trade_screen_state) {
    case 0:
        screen_part_alphas[0xFF] += 12.8f;
        if (screen_part_alphas[0xFF] > 128.0f) {
            screen_part_alphas[0xFF] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        screen_part_alphas[0xFD] += 1.6f;
        if (screen_part_alphas[0xFD] > 16.0f) {
            screen_part_alphas[0xFD] = 16.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] += (f32)shop_parts[i].alpha / 10.0f;
            if ((f32)shop_parts[i].alpha < screen_part_alphas[i]) {
                modal_input_locked = 0;
                trade_screen_state = 1;
                FILL_FADE_2(screen_part_alphas, shop_parts, shop_part_count)
                return;
            }
        }
        break;
    case 4:
    case 5:
        screen_part_alphas[0xFF] -= 12.8f;
        if (screen_part_alphas[0xFF] < 0.0f) {
            screen_part_alphas[0xFF] = 0.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        screen_part_alphas[0xFE] -= 12.8f;
        if (screen_part_alphas[0xFE] < 0.0f) {
            screen_part_alphas[0xFE] = 0.0f;
        }
        model_draw_color.alpha = (s32)screen_part_alphas[0xFE];
        screen_part_alphas[0xFD] -= 1.6f;
        if (screen_part_alphas[0xFD] < 0.0f) {
            screen_part_alphas[0xFD] = 0.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] -= (f32)shop_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (trade_screen_state == 4) {
                    trade_handler = Trade_UpdateGiveAmount;
                    context->active_list = 1;
                    trade_amount_down_locked = 0;
                } else if (trade_screen_state == 5) {
                    context->active_list = 1;
                    trade_handler = Trade_UpdateSelectScreen;
                    Menu_ResetLoadBuffers(0);
                }
                trade_screen_state = 0;
                trade_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
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
    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_RIGHT) {
            Sound_PlayMenu(1);
            context->active_list++;
        } else if (menu_buttons & PAD_LEFT) {
            Sound_PlayMenu(1);
            context->active_list--;
        }
    }
    if (item_counts[item_list_cursor[0]->item_number] < context->active_list) {
        context->active_list = 1;
    } else if (context->active_list <= 0) {
        context->active_list = item_counts[item_list_cursor[0]->item_number];
    }
    menu_number_79 = item_counts[item_list_cursor[0]->item_number];
    menu_number_80 = stock.counts[context->arg][item_list_cursor[0]->item_number];
    menu_number_81 = context->active_list;
    menu_number_82 = context->active_list * values.values[context->arg][item_list_cursor[0]->item_number].give_value;
    if (menu_number_80 >= 100) {
        menu_number_80 = 99;
    }
    record = item_list_cursor[0]->item_number - bases.first_item[item_list_cursor[0]->group];
    if (item_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[record].level_index + 1;
    }
    for (i = 0; i < shop_part_count; i++) {
        kind = shop_parts[i].handler_index;
        SELECT_PART((s8)shop_parts[i].blend_mode);
        if (kind == 3) {
            switch (shop_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
                break;
            case 1:
                MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
                break;
            case 2:
                break;
            case 3:
                if (item_list_cursor[0]->group != 11) {
                    MenuSprite_Draw((int)&shop_parts[i], kind + 1000, 3, (int)screen_part_alphas[i]);
                }
                break;
            }
        } else {
            // match: 10000 where the other screens add 1000 (the callee takes it modulo 1000, so it draws the same)
            MenuSprite_Draw((int)&shop_parts[i], kind + 10000, 4, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }

    MenuText_SetColor(menu_draw_color);
    memset(view_matrix, 0, 0x40);
    view_matrix[2][2] = view_matrix[1][1] = view_matrix[0][0] = 0.8f;
    view_matrix[3][3] = 1.0f;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = trade_give_amount_model_angle + PI;
    trade_give_amount_model_angle = angle - (s32)(angle / TWO_PI) * TWO_PI - PI;
    sceVu0RotMatrixY((f32 *)local_matrix, (f32 *)view_matrix, trade_give_amount_model_angle);
    sceVu0TransMatrix((f32 *)local_matrix, (f32 *)local_matrix, trade_give_amount_model_position);
    Menu_DrawItemModel((f32 *)local_matrix);

    if (modal_input_locked == 0) {
        group = item_list_cursor[0]->group;
        if (group == 11) {
            group = 12;
        }
        if ((menu_buttons & PAD_TRIANGLE) && Menu_IsEquipSlotFilled(record, group)) {
            info_pressed = 1;
            if (trade_panel_shown == 0) {
                trade_panel_shown = 1;
            } else {
                trade_panel_state = 2;
            }
        }
    }
    if (trade_panel_shown != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
        switch (item_list_cursor[0]->group) {
        case 11:
        case 12:
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (info_pressed != 0) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_71 = worn_item_level_records[record].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            glow = player_weapon[item_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_g_parts, shop_g_part_count)
            }
            DRAW_PACKETS(shop_g_parts, shop_g_part_count, glow)
            packets = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (info_pressed != 0) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            menu_number_45 = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            menu_number_46 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            menu_number_47 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            menu_number_48 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            menu_number_49 = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            menu_number_50 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            menu_number_51 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            menu_number_52 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            glow = player_weapon[item_list_cursor[0]->group];
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_g2_parts, shop_g2_part_count)
            }
            DRAW_PACKETS(shop_g2_parts, shop_g2_part_count, glow)
            packets = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (info_pressed != 0) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = ScaleValue(weapon_level_records[record].gained_values[0] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[0], weapon_level_records[record].level, 100);
            menu_number_45 = ScaleValue(weapon_level_records[record].gained_values[1] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[1], weapon_level_records[record].level, 100);
            menu_number_46 = ScaleValue(weapon_level_records[record].gained_values[2] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[2], weapon_level_records[record].level, 100);
            menu_number_47 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[3], weapon_level_records[record].level, 100);
            menu_number_48 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[4], weapon_level_records[record].level, 100);
            menu_number_49 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[5], weapon_level_records[record].level, 100);
            menu_number_50 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[6], weapon_level_records[record].level, 100);
            menu_number_51 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[7], weapon_level_records[record].level, 100);
            menu_number_52 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[8], weapon_level_records[record].level, 100);
            menu_number_70 = (s32)(weapon_defs[record].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[record].level;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[item_list_cursor[0]->group];
            menu_number_73 = 100;
            if (trade_panel_state == 1) {
                FILL_FADE(panel_part_alphas, shop_a_parts, shop_a_part_count)
            }
            DRAW_PACKETS(shop_a_parts, shop_a_part_count, 0)
            packets = shop_a_parts;
            count = shop_a_part_count;
            break;
        }

        switch (trade_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += (f32)packets[i].alpha / 10.0f;
                if ((f32)packets[i].alpha < panel_part_alphas[i]) {
                    FILL_FADE(panel_part_alphas, packets, count)
                    trade_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= (f32)packets[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    trade_panel_state = 0;
                    trade_panel_shown = 0;
                    break;
                }
            }
            break;
        }
    }

    node = item_list_top[0];
    position.raw = 0x0C0975E07C307080L;
    MenuText_SetPen(position);
    for (row = 0; row < 7; row++) {
        MenuText_DrawLine((u16 *)&item_name_defs[node->item_number], 0x1A);
        if (item_list_page_mark[0]->prev == node) {
            break;
        }
        node = node->prev;
    }

    bar_drawn = 0;
    for (i = 0; i < shop_mess_part_count; i++) {
        if (bar_drawn == 0 && shop_mess_parts[i].has_cursor_bar == 1) {
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
            if (trade_screen_state == 1) {
                screen_part_alphas[0xFD] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[0xFD]);
            }
            bar.alpha = (s32)screen_part_alphas[0xFD];
            MenuDraw_Bar(&bar);
        }
        kind = shop_mess_parts[i].handler_index;
        SELECT_PART((s8)shop_mess_parts[i].blend_mode);
        MenuSprite_Draw((int)&shop_mess_parts[i], kind + 1000, 4, (int)message_part_alphas[i]);
        SELECT_PART(0);
    }

    chosen = &item_name_defs[item_list_cursor[0]->item_number];
    MenuText_SetColor(menu_draw_color);
    position.raw = 0x0A2DC6C07DB078C0L;
    MenuText_SetPen(position);
    MenuText_DrawLineCentered(chosen, 0x18);

    current = item_list_cursor[0];
    texts = Message_GetFontText(menu_heap);
    number = current->item_number + MSG_ITEM_TEXT_FIRST + 1; // match: its own local, or the offset is folded into the load
    text_offset = ((s32 *)texts)[number];
    position.raw = 0x000A875083207080L;
    MenuText_SetPen(position);
    if (text_offset != 0) {
        MenuText_DrawWordWrappedLong(texts + text_offset, 0x33);
    }

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            Sound_PlayMenu(0);
            player_state.gold += menu_number_82;
            if (player_state.gold > 999999) {
                player_state.gold = 999999;
            }
            item_counts[item_list_cursor[0]->item_number] -= context->active_list;
            if (item_list_cursor[0]->item_number == player_state.weapon + ITEM_WEAPON_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST ||
                item_list_cursor[0]->item_number == player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST ||
                item_list_cursor[0]->item_number == player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST ||
                item_list_cursor[0]->item_number == player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST ||
                item_list_cursor[0]->item_number == player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.shortcut) {
                if (item_counts[item_list_cursor[0]->item_number] == 0) {
                    Menu_ClearEquipSlot(item_list_cursor[0]->group);
                }
            }
            if (stock.counts[context->arg][item_list_cursor[0]->item_number] != 0xFF) {
                stock.counts[context->arg][item_list_cursor[0]->item_number] += context->active_list;
                if (stock.counts[context->arg][item_list_cursor[0]->item_number] >= 100) {
                    stock.counts[context->arg][item_list_cursor[0]->item_number] = 99;
                }
            }
            trade_screen_state = 3;
            trade_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            trade_screen_state = 4;
            trade_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        }
    }
    if (trade_screen_state == 3 && model_load_chain == NULL) {
        trade_screen_state = 5;
    }
    if (trade_screen_state == 4 && model_load_chain == NULL) {
        trade_screen_state = 6;
    }

    switch (trade_screen_state) {
    case 0:
        screen_part_alphas[0xFF] += 12.8f;
        if (screen_part_alphas[0xFF] > 128.0f) {
            screen_part_alphas[0xFF] = 128.0f;
        }
        screen_part_alphas[0xFD] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        if (screen_part_alphas[0xFD] > 16.0f) {
            screen_part_alphas[0xFD] = 16.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] += (f32)shop_parts[i].alpha / 10.0f;
            if ((f32)shop_parts[i].alpha < screen_part_alphas[i]) {
                modal_input_locked = 0;
                trade_screen_state = 1;
                FILL_FADE(screen_part_alphas, shop_parts, shop_part_count)
                break;
            }
        }
        for (i = 0; i < shop_mess_part_count; i++) {
            message_part_alphas[i] += (f32)shop_mess_parts[i].alpha / 10.0f;
            if ((f32)shop_mess_parts[i].alpha < message_part_alphas[i]) {
                FILL_FADE(message_part_alphas, shop_mess_parts, shop_mess_part_count)
                break;
            }
        }
        break;
    case 5:
    case 6:
        screen_part_alphas[0xFF] -= 12.8f;
        if (screen_part_alphas[0xFF] < 0.0f) {
            screen_part_alphas[0xFF] = 0.0f;
        }
        screen_part_alphas[0xFE] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        if (screen_part_alphas[0xFE] < 0.0f) {
            screen_part_alphas[0xFE] = 0.0f;
        }
        screen_part_alphas[0xFD] -= 1.6f;
        model_draw_color.alpha = (s32)screen_part_alphas[0xFE];
        if (screen_part_alphas[0xFD] < 0.0f) {
            screen_part_alphas[0xFD] = 0.0f;
        }
        for (i = 0; i < shop_part_count; i++) {
            screen_part_alphas[i] -= (f32)shop_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (trade_screen_state == 5) {
                    context->active_list = 1;
                    trade_handler = Trade_UpdateSelectScreen;
                    Trade_BuildItemRings(context->arg);
                    Menu_ResetLoadBuffers(0);
                } else if (trade_screen_state == 6) {
                    trade_handler = trade_list_handlers[1];
                }
                trade_screen_state = 0;
                trade_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
                break;
            }
        }
        for (i = 0; i < shop_mess_part_count; i++) {
            message_part_alphas[i] -= (f32)shop_mess_parts[i].alpha / 10.0f;
            if (message_part_alphas[i] < 0.0f) {
                memset(message_part_alphas, 0, 0x400);
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

    forward = backward = trade_stock_list_top[0];
    if (item_list_count >= 8) {
        for (i = 0; i < 7; i++) {
            forward = forward->next;
            backward = backward->prev;
            if (forward == item_list_first) {
                while (++i < 7 && backward != item_list_last_page) {
                    backward = backward->prev;
                }
                break;
            }
            if (backward == item_list_last_page) {
                while (++i < 7 && forward != item_list_first) {
                    forward = forward->next;
                }
                break;
            }
        }
    } else {
        forward = backward = item_list_first;
    }
    sprintf(model_name_shown, "menu/select2/i%04d.mod", item_name_defs[trade_stock_list_cursor[0]->item_number].file_number);
    sprintf(model_name_down, "menu/select2/i%04d.mod", item_name_defs[trade_stock_list_cursor[0]->prev->item_number].file_number);
    sprintf(model_name_up, "menu/select2/i%04d.mod", item_name_defs[trade_stock_list_cursor[0]->next->item_number].file_number);
    sprintf(model_name_left, "menu/select2/i%04d.mod", item_name_defs[forward->item_number].file_number);
    sprintf(model_name_right, "menu/select2/i%04d.mod", item_name_defs[backward->item_number].file_number);
}

/* Moves the list cursor one row (0x10, 0x40) or one page (0x20, 0x80, lists of 8 and more), scrolling the window. */
static s32 Trade_MoveListCursor(HandlerContext *context) {
    MenuItem *cursor = trade_stock_list_cursor[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if (menu_buttons & PAD_UP) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == trade_stock_list_top[0] && next != trade_stock_list_page_mark[0]->prev) {
                    trade_stock_list_top[0] = next;
                    trade_stock_list_page_mark[0] = trade_stock_list_page_mark[0]->next;
                }
                Sound_PlayMenu(1);
                trade_stock_list_cursor[0] = cursor->next;
            }
            Menu_ShiftListModelsNext(0);
        } else if (menu_buttons & PAD_DOWN) {
            if (cursor->prev != NULL) {
                if (trade_stock_list_page_mark[0]->prev == cursor && trade_stock_list_top[0]->next != cursor) {
                    trade_stock_list_page_mark[0] = cursor;
                    trade_stock_list_top[0] = trade_stock_list_top[0]->prev;
                }
                Sound_PlayMenu(1);
                trade_stock_list_cursor[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((menu_buttons & PAD_RIGHT) && item_list_count >= 8) {
            if (cursor->prev != NULL) {
                if (trade_stock_list_top[0]->next != trade_stock_list_page_mark[0]->prev) {
                    rows = 0;
                    while (Trade_StockListScrollUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageUp(0);
        } else if ((menu_buttons & PAD_LEFT) && item_list_count >= 8) {
            if (cursor->next != NULL) {
                if (trade_stock_list_top[0]->next != trade_stock_list_page_mark[0]->prev) {
                    rows = 0;
                    while (Trade_StockListScrollDown()) {
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

/* Builds the two item rings of the smith's screens: the items that can be handed in (mode 0: those that can still
   gain a level, mode 1: any idle one) at item_list_cursor, and at smith_collect_list_top the items handed in earlier whose time is
   up. Each ring's count goes to handover_list_count / owned_list_count. */
static void Smith_BuildItemRings(s32 mode) {
    MenuItem *last = NULL;
    MenuItem *first = NULL;
    s32 count = 0;
    s32 i;

    item_list_top[0] = NULL;
    item_list_page_mark[0] = NULL;
    handover_list_first = NULL;
    handover_list_last_page = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    if (mode == 0) {
        APPEND_UPGRADABLE(weapon_entries, weapon_level_records, 0, 150, 70)
        APPEND_UPGRADABLE(shield_entries, worn_item_level_records, 0, 220, 30)
        APPEND_UPGRADABLE(helm_entries, worn_item_level_records, 30, 250, 25)
        APPEND_UPGRADABLE(armor_entries, worn_item_level_records, 55, 275, 25)
        APPEND_UPGRADABLE(gauntlet_entries, worn_item_level_records, 80, 300, 25)
        APPEND_UPGRADABLE(boot_entries, worn_item_level_records, 105, 325, 25)
    } else if (mode == 1) {
        APPEND_IDLE(weapon_entries, weapon_level_records, 0, 150, 70)
        APPEND_IDLE(shield_entries, worn_item_level_records, 0, 220, 30)
        APPEND_IDLE(helm_entries, worn_item_level_records, 30, 250, 25)
        APPEND_IDLE(armor_entries, worn_item_level_records, 55, 275, 25)
        APPEND_IDLE(gauntlet_entries, worn_item_level_records, 80, 300, 25)
        APPEND_IDLE(boot_entries, worn_item_level_records, 105, 325, 25)
        APPEND_IDLE(ring_entries, worn_item_level_records, 130, 350, 10)
        APPEND_IDLE(bracelet_entries, worn_item_level_records, 140, 360, 10)
        APPEND_IDLE(amulet_entries, worn_item_level_records, 150, 370, 10)
    }
    handover_list_count = count;
    if (first != NULL) {
        last->prev = first;
        item_list_top[0] = first;
        item_list_cursor[0] = first;
        first->next = last;
        if (item_list_page_mark[0] == NULL) {
            item_list_page_mark[0] = last->next;
        } else {
            item_list_page_mark[0] = item_list_page_mark[0]->next;
        }
        handover_list_first = first;
        handover_list_last_page = last;
        for (i = 5; i >= 0; i--) {
            handover_list_last_page = handover_list_last_page->next;
        }
    } else {
        item_list_cursor[0] = NULL;
    }

    smith_collect_list_top[0] = NULL;
    smith_collect_list_page_mark[0] = NULL;
    last = NULL;
    first = NULL;
    count = 0;
    owned_list_first = NULL;
    owned_list_last_page = NULL;
    PlayTime_Update();
    for (i = 1; i < 380; i++) {
        if (i >= 150) {
            if (i < 220) {
                if (i != 150 && weapon_level_records[i - 150].in_smith_rings != 0 && !(item_name_defs[i].state & 2) &&
                    PlayTime_SecondsBetween(weapon_level_records[i - 150].last_upgraded.time, current_play_time) >= item_name_defs[i].upgrade_wait_minutes * 60) {
                    // match: written out; through APPEND's `(node)->next` the item pointer is reduced differently
                    count++;
                    if (count == 7) {
                        smith_collect_list_page_mark[0] = &all_item_entries[i];
                    }
                    if (last != NULL) {
                        all_item_entries[i].next = last;
                        last->prev = &all_item_entries[i];
                    } else {
                        first = &all_item_entries[i];
                    }
                    last = &all_item_entries[i];
                }
            } else {
                if (i != 220 && worn_item_level_records[i - 220].in_smith_rings != 0 && !(item_name_defs[i].state & 2) &&
                    PlayTime_SecondsBetween(worn_item_level_records[i - 220].last_upgraded.time, current_play_time) >= item_name_defs[i].upgrade_wait_minutes * 60) {
                    count++;
                    if (count == 7) {
                        smith_collect_list_page_mark[0] = &all_item_entries[i];
                    }
                    if (last != NULL) {
                        all_item_entries[i].next = last;
                        last->prev = &all_item_entries[i];
                    } else {
                        first = &all_item_entries[i];
                    }
                    last = &all_item_entries[i];
                }
            }
        }
    }
    owned_list_count = count;
    if (first != NULL) {
        last->prev = first;
        smith_collect_list_top[0] = first;
        smith_collect_list_cursor[0] = first;
        first->next = last;
        if (smith_collect_list_page_mark[0] == NULL) {
            smith_collect_list_page_mark[0] = last->next;
        } else {
            smith_collect_list_page_mark[0] = smith_collect_list_page_mark[0]->next;
        }
        owned_list_first = first;
        owned_list_last_page = last;
        for (i = 5; i >= 0; i--) {
            owned_list_last_page = owned_list_last_page->next;
        }
    } else {
        smith_collect_list_cursor[0] = NULL;
    }
}

/* the same for the smith's screens */
static int (*const smith_run_steps[2])(s32) IN_RODATA = {
    Smith_CopyFrameToTexture,
    Smith_RunStep,
};
static int (*const smith_steps[4])(HandlerContext *) IN_RODATA = {
    Smith_Start,
    Smith_RunHandler,
    Smith_CutModelChain,
    NULL,
};
static void (*const smith_list_handlers[3])(HandlerContext *) IN_RODATA = {
    Smith_UpdateUpgradeList,
    Smith_UpdateListWithConfirm,
    Smith_UpdateCollectList,
};

/* Runs the screen loop of the handler smith_run_step selects until it returns 0, watching the pad type for changes. */
void Smith_Run(void) {
    modal_input_locked = 1;
    smith_confirm_shown = 0;
    smith_unread_flag = 0;
    smith_upgrade_choice = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    Vram_Clear(0x3D00, 0x3F00);
    Sound_MutePositional();
    Sound_UpdateTagged();
    menu_mode = 0;
    modal_pad_mode = modal_pad_mode_last = Pad_GetMode(0, 0);
    while (smith_run_steps[smith_run_step](0) != 0) {
        sceGsSyncPath(0, 0);
        modal_even_field = Menu_EndFrame(1);
        Input_Update();
        modal_pad_mode = Pad_GetMode(0, 0);
        if (modal_pad_mode != modal_pad_mode_last) {
            smith_pad_change_frames = 1;
        }
        modal_pad_mode_last = modal_pad_mode;
        MenuPad_Update();
        if (smith_pad_change_frames > 0) {
            smith_pad_change_frames++;
            if (smith_pad_change_frames >= 21) {
                smith_pad_change_frames = 0;
            } else {
                menu_buttons = 0;
            }
        }
    }
    sceGsSyncPath(0, 0);
    while (sceGsSyncV(0) != 0) {
    }
    Sound_UnmutePositional();
    MenuHeap_FreeToMark();
    smith_run_step = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}

/* Start-up step of the screen Smith_UpdateSelectScreen runs: loads its textures and blocks, clears the alpha tables and moves on. */
static s32 Smith_Start(HandlerContext *context) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;

    Packet_SwapBuffers();
    Packet_OnFrameStart();
    TextureTable_QueueAll((TextureTableHeader *)Message_GetFontTexture(menu_heap));
    GsDraw_QueueTexFlush();
    Packet_OnFrameEnd();
    Packet_SendChain();
    ((s32 (*)(void))Packet_WaitForSend)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < menu_file_block_count; i++) {
        block = &menu_file_blocks[i];
        MenuTexture_Upload(block->data, block);
    }
    MenuArena_Init(trade_menu_arena);
    packet = MenuDraw_NextPacket();
    packet[0] = 0x8001;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 0x3F;
    packet[7] = 0;
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 2;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    memset(screen_part_alphas, 0, 0x400);
    memset(panel_part_alphas, 0, 0x400);
    memset(message_part_alphas, 0, 0x400);
    menu_draw_color.blue = 0x80;
    menu_draw_color.green = 0x80;
    menu_draw_color.red = 0x80;
    menu_draw_color.alpha = 0;
    model_draw_color.blue = 0x80;
    model_draw_color.green = 0x80;
    model_draw_color.red = 0x80;
    model_draw_color.alpha = 0;
    Menu_InitLoadBuffers();
    smith_handler = Smith_UpdateSelectScreen;
    smith_step++;
    return TRUE;
}

/* Copies the shown frame buffer into a work buffer and draws that back into both frame buffers, then steps the screen counter. */
static s32 Smith_CopyFrameToTexture(s32 unused_step) {
    f32 unused[28]; // match: 0x70 bytes of locals nothing uses
    u32 texture_base; /* where the shown buffer is as a texture: 0x1180 when it is the second one */
    u32 *packet;

    packet = MenuDraw_NextPacket();
    texture_base = 0x1180;
    if (context1_buffer_half == 0) {
        texture_base = 0;
    }
    packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
    packet[2] = 0xE;
    packet[3] = 0;
    *(u64 *)&packet[4] = (menu_first_draw_env[0] & ~0x1FFL) | 0x192;
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
    *(u64 *)&packet[48] = menu_first_draw_env[0];
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
    *(u64 *)&packet[72] = menu_first_draw_env[46];
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
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 26;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    sceGsSyncPath(0, 0);
    smith_run_step++;
    return 1;
}

/* Calls the handler selected by smith_step with the context at smith_context, storing value in it first. */
static int Smith_RunStep(int value) {
    int (*handler)(HandlerContext *);

    Menu_StepModelLoads(0);
    handler = smith_steps[smith_step];
    smith_context.arg = value;
    if (handler != NULL) {
        return handler(&smith_context);
    }
    return 1;
}

/* Runs the handler in smith_handler with the context this step was given. */
static int Smith_RunHandler(HandlerContext *context) {
    smith_handler(context);
    return 1;
}

/* Cuts the chain after the first link node (and after the node it points at); with no chain, resets smith_step. */
static int Smith_CutModelChain(HandlerContext *context) {
    LinkNode *node = model_load_chain;
    LinkNode *other;

    if (node == NULL) {
        smith_step = 0;
        return 0;
    }
    other = node->target_node;
    node->next = NULL;
    if (other != NULL) {
        other->next = NULL;
    }
    return 1;
}

/* Rebuilds the five link nodes for the current smith_collect_list_cursor item and chains them (the Menu_ChainItemListModels of this list). */
static void Smith_ChainModels(s32 unused_arg) {
    if (smith_collect_list_cursor[0] != NULL) {
        Smith_SetModelNames();
        Menu_TakeLoadBuffer(&model_load_shown);
        Menu_TakeLoadBuffer(&model_load_up);
        Menu_TakeLoadBuffer(&model_load_down);
        Menu_TakeLoadBuffer(&model_load_left);
        Menu_TakeLoadBuffer(&model_load_right);
        model_load_shown.next = &model_load_up;
        model_load_shown.callback = Menu_StepModelLoad;
        model_load_shown.done = 0;
        model_load_up.next = &model_load_down;
        model_load_up.callback = Menu_StepModelLoad;
        model_load_up.done = 0;
        model_load_up.stage = 0;
        model_load_down.next = &model_load_left;
        model_load_down.callback = Menu_StepModelLoad;
        model_load_down.done = 0;
        model_load_down.stage = 0;
        model_load_left.next = &model_load_right;
        model_load_left.callback = Menu_StepModelLoad;
        model_load_left.done = 0;
        model_load_left.stage = 0;
        model_load_right.next = NULL;
        model_load_right.callback = Menu_StepModelLoad;
        model_load_right.done = 0;
        model_load_shown.stage = model_load_right.stage = 0;
        model_load_chain = &model_load_shown;
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

    if (modal_input_locked == 0 && smith_choices_active != 0) {
        if (menu_buttons & PAD_UP) {
            Sound_PlayMenu(1);
            context->active_list--;
        } else if (menu_buttons & PAD_DOWN) {
            Sound_PlayMenu(1);
            context->active_list++;
        }
    }
    line_count = 3;
    context->active_list = (context->active_list + 3) % line_count;

    bar_drawn = FALSE;
    for (i = 0; i < pretune_part_count; i++) {
        if (!bar_drawn && pretune_parts[i].has_cursor_bar == 1) {
            bar.first.f.lo16 = 0x76C0;
            bar.first.f.mid16 = context->active_list * 0xD0 + 0x7EF0;
            bar.first.f.mid24 = 0xDBBA0;
            bar.green = bar.red = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x12C0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xC0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar_drawn = TRUE;
            if (smith_screen_state == 1) {
                screen_part_alphas[253] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[253]);
            } else {
                screen_part_alphas[253] = 0.0f;
            }
            bar.alpha = (s32)screen_part_alphas[253];
            MenuDraw_Bar(&bar);
        }
        ((int (*)(s8))Menu_SetBlendMode)(pretune_parts[i].blend_mode); // match: the byte is passed sign-extended here
        MenuSprite_Draw((int)&pretune_parts[i], 1000, 0, (int)screen_part_alphas[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if (smith_choices_active == 0) {
        for (i = 0; i < tune_mess_part_count; i++) {
            kind = tune_mess_parts[i].handler_index;
            MenuSprite_DrawPlain((int)&tune_mess_parts[i], kind, 1);
        }
    }

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            if (smith_choices_active == 0) {
                Sound_PlayMenu(0);
                smith_choices_active = 1;
            } else {
                detail_panel_open = 0;
                switch (context->active_list) {
                case 0:
                    Smith_BuildItemRings(0);
                    if (item_list_cursor[0] != NULL) {
                        Sound_PlayMenu(0);
                        modal_input_locked = 1;
                        smith_screen_state = 2;
                    } else {
                        Sound_PlayMenu(3);
                    }
                    break;
                case 1:
                    Sound_PlayMenu(0);
                    Smith_BuildItemRings(1);
                    if (item_counts[ITEM_EARTH_STONE] != 0) {
                        modal_input_locked = 1;
                        smith_screen_state = 3;
                    } else {
                        smith_choices_active = 0;
                    }
                    break;
                case 2:
                    Smith_BuildItemRings(2);
                    if (owned_list_count > 0) {
                        Sound_PlayMenu(0);
                        modal_input_locked = 1;
                        smith_screen_state = 4;
                    } else {
                        Sound_PlayMenu(3);
                    }
                    break;
                }
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            if (smith_choices_active == 0) {
                smith_choices_active = 1;
            } else {
                smith_screen_state = 5;
                modal_input_locked = 1;
            }
        }
    }

    switch (smith_screen_state) {
    case 0:
        screen_part_alphas[253] += 1.6f;
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        for (i = 0; i < pretune_part_count; i++) {
            screen_part_alphas[i] += pretune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > pretune_parts[i].alpha) {
                for (i = 0; i < pretune_part_count; i++) {
                    full_alpha = pretune_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                modal_input_locked = 0;
                smith_screen_state = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        screen_part_alphas[253] -= 1.6f;
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < pretune_part_count; i++) {
            screen_part_alphas[i] -= pretune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                // match: the table has two entries; line 2 reads the word after it (trade_value_tables's first pointer)
                trade_handler = trade_list_handlers[context->active_list];
                if (smith_screen_state == 2) {
                    item_list_first = handover_list_first;
                    smith_handler = smith_list_handlers[context->active_list];
                    item_list_last_page = handover_list_last_page;
                    item_list_count = handover_list_count;
                    Menu_ChainItemListModels(0);
                } else if (smith_screen_state == 3) {
                    item_list_first = handover_list_first;
                    smith_handler = smith_list_handlers[context->active_list];
                    item_list_last_page = handover_list_last_page;
                    item_list_count = handover_list_count;
                    smith_confirm_shown = 0;
                    Menu_ChainItemListModels(0);
                } else if (smith_screen_state == 4) {
                    item_list_first = owned_list_first;
                    smith_handler = smith_list_handlers[context->active_list];
                    item_list_last_page = owned_list_last_page;
                    item_list_count = owned_list_count;
                    Smith_ChainModels(0);
                } else if (smith_screen_state == 5) {
                    smith_step++;
                }
                smith_screen_state = 0;
                context->active_list = 0;
                break;
            }
        }
        break;
    }
}

/* Writes the model file names of the selected item, its two neighbours, and the two items just outside the visible
   window (found by walking at most 7 steps out from smith_collect_list_top towards the list's ends) into the five link nodes. */
static void Smith_SetModelNames(void) {
    MenuItem *before = smith_collect_list_top[0];
    MenuItem *after = before;
    int steps;

    if (item_list_count >= 8) {
        for (steps = 0; steps < 7; steps++) {
            after = after->next;
            before = before->prev;
            if (after == item_list_first) {
                for (steps++; steps < 7 && before != item_list_last_page; steps++) {
                    before = before->prev;
                }
                break;
            }
            if (before == item_list_last_page) {
                for (steps++; steps < 7 && after != item_list_first; steps++) {
                    after = after->next;
                }
                break;
            }
        }
    } else {
        after = before = item_list_first;
    }
    sprintf(model_name_shown, "menu/select2/i%04d.mod", item_name_defs[smith_collect_list_cursor[0]->item_number].file_number);
    sprintf(model_name_down, "menu/select2/i%04d.mod", item_name_defs[smith_collect_list_cursor[0]->prev->item_number].file_number);
    sprintf(model_name_up, "menu/select2/i%04d.mod", item_name_defs[smith_collect_list_cursor[0]->next->item_number].file_number);
    sprintf(model_name_left, "menu/select2/i%04d.mod", item_name_defs[after->item_number].file_number);
    sprintf(model_name_right, "menu/select2/i%04d.mod", item_name_defs[before->item_number].file_number);
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

    if (item_list_cursor[0] == NULL) {
        smith_handler = Smith_UpdateSelectScreen;
        return;
    }
    if (smith_screen_state < 2) {
        Menu_MoveItemListCursor(context, modal_input_locked);
    }
    number = item_list_cursor[0]->item_number;
    index = number - bases.first_item[item_list_cursor[0]->group];
    if (item_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[index].level_index + 1;
    }
    menu_number_74 = 100;
    if (item_list_cursor[0]->item_number < 220) {
        menu_number_75 = GetWeaponLevel(item_list_cursor[0]->item_number - 150);
    } else {
        menu_number_75 = GetItemLevel(item_list_cursor[0]->item_number - 220);
    }
    for (i = 0; i < tune_part_count; i++) {
        kind = tune_parts[i].handler_index;
        SELECT_PART((s8)tune_parts[i].blend_mode);
        if (kind == 3) {
            switch (tune_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 0, (int)screen_part_alphas[i]);
                break;
            case 1:
                break;
            case 2:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 2, (int)screen_part_alphas[i]);
                break;
            case 3:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 3, (int)screen_part_alphas[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&tune_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }
    item = item_list_top[0];
    for (row = 0; row < 7; row++, item = item->prev) {
        if (item == item_list_cursor[0]) {
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
            if (smith_screen_state == 1) {
                screen_part_alphas[253] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[253]);
            }
            bar.alpha = (s32)screen_part_alphas[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }
    MenuText_SetColor(menu_draw_color);
    word.raw = 0x0C7A12007C307080;
    for (line = 0, node = item_list_top[0]; line < 7; node = node->prev, line++) {
        def = &item_name_defs[node->item_number];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (item_list_page_mark[0]->prev == node) {
            break;
        }
    }
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = smith_upgrade_list_model_angle + PI;
    smith_upgrade_list_model_angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
    sceVu0RotMatrixY(rotation, scale, smith_upgrade_list_model_angle);
    sceVu0TransMatrix(rotation, rotation, smith_upgrade_list_model_position);
    Menu_DrawItemModel(rotation);
    if ((menu_buttons & PAD_TRIANGLE) && modal_input_locked == 0 && Menu_IsEquipSlotFilled(index, item_list_cursor[0]->group)) {
        smith_upgrade_list_panel_sound_pending = 1;
        if (detail_panel_open == 0) {
            detail_panel_open = 1;
        } else {
            smith_panel_state = 2;
        }
    }
    if (detail_panel_open != 0) {
        switch (item_list_cursor[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (smith_upgrade_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_upgrade_list_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_71 = worn_item_level_records[index].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g_part_count; i++) {
                    full_alpha = shop_g_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g_part_count; i++) {
                SELECT_PART((s8)shop_g_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g_parts[i], shop_g_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (smith_upgrade_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_upgrade_list_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g2_part_count; i++) {
                    full_alpha = shop_g2_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g2_part_count; i++) {
                SELECT_PART((s8)shop_g2_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g2_parts[i], shop_g2_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (smith_upgrade_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_upgrade_list_panel_sound_pending = 0;
            menu_number_44 = (weapon_level_records[index].gained_values[0] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[0]) * weapon_level_records[index].level / 100;
            menu_number_45 = (weapon_level_records[index].gained_values[1] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[1]) * weapon_level_records[index].level / 100;
            menu_number_46 = (weapon_level_records[index].gained_values[2] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[2]) * weapon_level_records[index].level / 100;
            menu_number_47 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[3] * weapon_level_records[index].level / 100;
            menu_number_48 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[4] * weapon_level_records[index].level / 100;
            menu_number_49 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[5] * weapon_level_records[index].level / 100;
            menu_number_50 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[6] * weapon_level_records[index].level / 100;
            menu_number_51 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[7] * weapon_level_records[index].level / 100;
            menu_number_52 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[8] * weapon_level_records[index].level / 100;
            menu_number_70 = (s32)(weapon_defs[index].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[index].level;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[item_list_cursor[0]->group];
            menu_number_73 = 100;
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_a_part_count; i++) {
                    full_alpha = shop_a_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_a_part_count; i++) {
                SELECT_PART((s8)shop_a_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_a_parts[i], shop_a_parts[i].handler_index + 1000, 0, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_a_parts;
            count = shop_a_part_count;
            break;
        }
        switch (smith_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        panel_part_alphas[i] = full_alpha;
                    }
                    smith_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    if (smith_panel_state == 2) {
                        detail_panel_open = 0;
                    }
                    smith_panel_state = 0;
                    break;
                }
            }
            break;
        }
    }
    selected = item_list_cursor[0];
    table = Message_GetFontText(menu_heap);
    text_index = selected->item_number + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong(table + entry, 0x33);
    }
    if ((menu_buttons & PAD_CROSS) && modal_input_locked == 0) {
        if ((item_list_cursor[0]->item_number < 220 ? GetWeaponLevel(item_list_cursor[0]->item_number - 150) < 100
                                        : GetItemLevel(item_list_cursor[0]->item_number - 220) < 100) ||
            item_list_cursor[0]->item_number == 183) {
            Sound_PlayMenu(0);
            smith_screen_state = 2;
            smith_panel_state = 3;
            modal_input_locked = 1;
        }
    } else if ((menu_buttons & PAD_CIRCLE) && modal_input_locked == 0) {
        Sound_PlayMenu(2);
        smith_screen_state = 3;
        smith_panel_state = 2;
        modal_input_locked = 1;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    }
    if (smith_screen_state == 2 && model_load_chain == NULL) {
        smith_screen_state = 4;
    }
    if (smith_screen_state == 3 && model_load_chain == NULL) {
        smith_screen_state = 5;
    }
    switch (smith_screen_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        screen_part_alphas[253] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] += tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > tune_parts[i].alpha) {
                smith_screen_state = 1;
                for (i = 0; i < tune_part_count; i++) {
                    full_alpha = tune_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                modal_input_locked = 0;
                return;
            }
        }
        break;
    case 4:
    case 5:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[254] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[254] < 0.0f) {
            screen_part_alphas[254] = 0.0f;
        }
        screen_part_alphas[253] -= 1.6f;
        model_draw_color.alpha = (s32)screen_part_alphas[254];
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] -= tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (smith_screen_state == 4) {
                    context->active_list = 1;
                    smith_handler = Smith_UpdateUpgradeConfirm;
                    smith_confirm_shown = 0;
                } else if (smith_screen_state == 5) {
                    context->active_list = 0;
                    smith_handler = Smith_UpdateSelectScreen;
                    Menu_ResetLoadBuffers(0);
                }
                smith_screen_state = 0;
                smith_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
                return;
            }
        }
        break;
    }
}

/* The screen Smith_UpdateUpgradeList opens for the item chosen from its list: draws the parts, the list with the item's panel,
   its name and its text, waits for accept or cancel and fades out. On accept the item's record gets level 100, the
   play time of now and the flag that makes that time count (LevelDef.upgrade_wait_minutes minutes, shown through menu_number_76); item
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

    menu_number_74 = 100;
    if (item_list_cursor[0]->item_number < 220) {
        menu_number_75 = GetWeaponLevel(item_list_cursor[0]->item_number - 150);
    } else {
        menu_number_75 = GetItemLevel(item_list_cursor[0]->item_number - 220);
    }
    number = item_list_cursor[0]->item_number;
    index = number - bases.first_item[item_list_cursor[0]->group];
    if (item_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[index].level_index + 1;
    }
    for (i = 0; i < tune_part_count; i++) {
        kind = tune_parts[i].handler_index;
        SELECT_PART((s8)tune_parts[i].blend_mode);
        if (kind == 3) {
            switch (tune_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 0, (int)screen_part_alphas[i]);
                break;
            case 1:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 1, (int)screen_part_alphas[i]);
                break;
            case 2:
                break;
            case 3:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 3, (int)screen_part_alphas[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&tune_parts[i], kind + 1000, 2, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }
    MenuText_SetColor(menu_draw_color);
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = smith_upgrade_confirm_model_angle + PI;
    smith_upgrade_confirm_model_angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
    sceVu0RotMatrixY(rotation, scale, smith_upgrade_confirm_model_angle);
    sceVu0TransMatrix(rotation, rotation, smith_upgrade_confirm_model_position);
    Menu_DrawItemModel(rotation);
    if ((menu_buttons & PAD_TRIANGLE) && modal_input_locked == 0 && Menu_IsEquipSlotFilled(index, item_list_cursor[0]->group)) {
        smith_upgrade_confirm_panel_sound_pending = 1;
        if (detail_panel_open == 0) {
            detail_panel_open = 1;
        } else {
            smith_panel_state = 2;
        }
    }
    if (detail_panel_open != 0) {
        switch (item_list_cursor[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (smith_upgrade_confirm_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_upgrade_confirm_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_71 = worn_item_level_records[index].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g_part_count; i++) {
                    full_alpha = shop_g_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g_part_count; i++) {
                SELECT_PART((s8)shop_g_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g_parts[i], shop_g_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (smith_upgrade_confirm_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_upgrade_confirm_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g2_part_count; i++) {
                    full_alpha = shop_g2_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g2_part_count; i++) {
                SELECT_PART((s8)shop_g2_parts[i].blend_mode);
                // match: called as returning a value, which keeps $2 away from the pointer formed for the next call
                ((int (*)(int, int, int, int))MenuSprite_Draw)((int)&shop_g2_parts[i], shop_g2_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART((s8)shop_g2_parts[i].blend_mode); // match: the other lists hand 0 here
            }
            parts = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (smith_upgrade_confirm_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_upgrade_confirm_panel_sound_pending = 0;
            menu_number_44 = (weapon_level_records[index].gained_values[0] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[0]) * weapon_level_records[index].level / 100;
            menu_number_45 = (weapon_level_records[index].gained_values[1] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[1]) * weapon_level_records[index].level / 100;
            menu_number_46 = (weapon_level_records[index].gained_values[2] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[2]) * weapon_level_records[index].level / 100;
            menu_number_47 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[3] * weapon_level_records[index].level / 100;
            menu_number_48 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[4] * weapon_level_records[index].level / 100;
            menu_number_49 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[5] * weapon_level_records[index].level / 100;
            menu_number_50 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[6] * weapon_level_records[index].level / 100;
            menu_number_51 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[7] * weapon_level_records[index].level / 100;
            menu_number_52 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[8] * weapon_level_records[index].level / 100;
            menu_number_70 = (s32)(weapon_defs[index].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[index].level;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[item_list_cursor[0]->group];
            menu_number_73 = 100;
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_a_part_count; i++) {
                    full_alpha = shop_a_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_a_part_count; i++) {
                SELECT_PART((s8)shop_a_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_a_parts[i], shop_a_parts[i].handler_index + 1000, 0, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_a_parts;
            count = shop_a_part_count;
            break;
        }
        switch (smith_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        panel_part_alphas[i] = full_alpha;
                    }
                    smith_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    smith_panel_state = 0;
                    detail_panel_open = 0;
                    break;
                }
            }
            break;
        }
    }
    word.raw = 0x0C0975E07C307080;
    for (row = 0, item = item_list_top[0]; row < 7; item = item->prev, row++) {
        def = &item_name_defs[item->item_number];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (item_list_page_mark[0]->prev == item) {
            break;
        }
    }
    menu_number_76 = item_name_defs[item_list_cursor[0]->item_number].upgrade_wait_minutes;
    for (i = 0; i < tune_mess_part_count; i++) {
        kind = tune_mess_parts[i].handler_index;
        SELECT_PART((s8)tune_mess_parts[i].blend_mode);
        MenuSprite_Draw((int)&tune_mess_parts[i], kind + 1000, 0, (int)message_part_alphas[i]);
        SELECT_PART(0);
    }
    selected_def = &item_name_defs[item_list_cursor[0]->item_number];
    MenuText_SetColor(menu_draw_color);
    word.raw = 0x0A2DC6C07F5078C0;
    MenuText_SetPen(word);
    MenuText_DrawLineCentered(selected_def, 0x18);
    selected = item_list_cursor[0];
    table = Message_GetFontText(menu_heap);
    text_index = selected->item_number + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong((u16 *)(table + entry), 0x33);
    }
    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            Sound_PlayMenu(0);
            smith_screen_state = 2;
            smith_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            smith_screen_state = 3;
            smith_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        }
    }
    if (smith_screen_state == 2 && model_load_chain == NULL) {
        smith_screen_state = 4;
    }
    if (smith_screen_state == 3 && model_load_chain == NULL) {
        smith_screen_state = 5;
    }
    switch (smith_screen_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        screen_part_alphas[253] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] += tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > tune_parts[i].alpha) {
                smith_screen_state = 1;
                modal_input_locked = 0;
                for (i = 0; i < tune_part_count; i++) {
                    full_alpha = tune_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                break;
            }
        }
        for (i = 0; i < tune_mess_part_count; i++) {
            message_part_alphas[i] += tune_mess_parts[i].alpha / 10.0f;
            if (message_part_alphas[i] > tune_mess_parts[i].alpha) {
                for (i = 0; i < tune_mess_part_count; i++) {
                    full_alpha = tune_mess_parts[i].alpha;
                    message_part_alphas[i] = full_alpha;
                }
                break;
            }
        }
        break;
    case 4:
    case 5:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[254] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[254] < 0.0f) {
            screen_part_alphas[254] = 0.0f;
        }
        screen_part_alphas[253] -= 1.6f;
        model_draw_color.alpha = (s32)screen_part_alphas[254];
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] -= tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (smith_screen_state == 4) {
                    PlayTime_Update();
                    bought = item_list_cursor[0];
                    if (bought->item_number < 220) {
                        if (bought->item_number == ITEM_BROKEN_KATANA) {
                            item_counts[ITEM_BROKEN_KATANA] = 0;
                            item_counts[bought->item_number + 1] = 1;
                            // match: "- 150 + 1" wraps the constant offset around, which keeps the compiler from
                            // forming the table's address out of this one (it does for "- 149")
                            *(ClockTime *)weapon_level_records[bought->item_number - 150 + 1].last_upgraded.bytes = current_play_time;
                            GetWeaponRecord(bought->item_number - 149)->level = 100;
                            GetWeaponRecord(bought->item_number - 149)->in_smith_rings = 1;
                        } else {
                            *(ClockTime *)weapon_level_records[bought->item_number - 150].last_upgraded.bytes = current_play_time;
                            GetWeaponRecord(bought->item_number - 150)->level = 100;
                            GetWeaponRecord(bought->item_number - 150)->in_smith_rings = 1;
                        }
                    } else {
                        *(ClockTime *)worn_item_level_records[bought->item_number - 220].last_upgraded.bytes = current_play_time;
                        GetItemRecord(bought->item_number - 220)->level = 100;
                        GetItemRecord(bought->item_number - 220)->in_smith_rings = 1;
                    }
                    if (item_list_cursor[0]->item_number == player_state.weapon + ITEM_WEAPON_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.shortcut) {
                        Menu_ClearEquipSlot(item_list_cursor[0]->group);
                    }
                    context->active_list = 0;
                    smith_handler = Smith_UpdateSelectScreen;
                    Menu_ResetLoadBuffers(0);
                } else if (smith_screen_state == 5) {
                    smith_handler = smith_list_handlers[0];
                }
                smith_screen_state = 0;
                smith_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
                break;
            }
        }
        for (i = 0; i < tune_mess_part_count; i++) {
            message_part_alphas[i] -= tune_mess_parts[i].alpha / 10.0f;
            if (message_part_alphas[i] < 0.0f) {
                memset(message_part_alphas, 0, 0x400);
                break;
            }
        }
        break;
    }
}

/* The smith's item list screen with its confirmation window, a twin of Smith_UpdateUpgradeList: draws the screen parts, the
   cursor bar, the seven rows, the turning model, the selected item's panel and, with smith_confirm_shown set, the window that
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

    if (item_list_cursor[0] == NULL) {
        smith_handler = Smith_UpdateSelectScreen;
        return;
    }
    if (smith_confirm_shown == 0 && smith_screen_state < 2) {
        Menu_MoveItemListCursor(context, modal_input_locked);
    }
    number = item_list_cursor[0]->item_number;
    index = number - bases.first_item[item_list_cursor[0]->group];
    if (item_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[index].level_index + 1;
    }
    menu_number_74 = 100;
    if (item_list_cursor[0]->item_number < 220) {
        menu_number_75 = GetWeaponLevel(item_list_cursor[0]->item_number - 150);
    } else {
        menu_number_75 = GetItemLevel(item_list_cursor[0]->item_number - 220);
    }
    for (i = 0; i < tune_part_count; i++) {
        kind = tune_parts[i].handler_index;
        SELECT_PART((s8)tune_parts[i].blend_mode);
        if (kind == 3) {
            switch (tune_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 0, (int)screen_part_alphas[i]);
                break;
            case 1:
                if (smith_confirm_shown != 0) {
                    MenuSprite_Draw((int)&tune_parts[i], 1003, 1, (int)screen_part_alphas[i]);
                }
                break;
            case 2:
                if (smith_confirm_shown == 0) {
                    MenuSprite_Draw((int)&tune_parts[i], 1003, 2, (int)screen_part_alphas[i]);
                }
                break;
            case 3:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 3, (int)screen_part_alphas[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&tune_parts[i], kind + 1000, 3, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }
    if (smith_confirm_shown == 0) {
        // match: the bar's corner is a packed word set in front of the loop (as in Menu_UpdateVisibleItem), not literals in
        // the loop body: its three fields then leave the loop in the first loop pass and 0xFF only in the second.
        // The word's top byte is not used and cannot be told from the code.
        corner.raw = 0x000864707C306FE0;
        item = item_list_top[0];
        for (row = 0; row < 7; row++, item = item->prev) {
            if (item == item_list_cursor[0]) {
                bar.first.f.lo16 = corner.f.lo16;
                bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
                bar.first.f.mid24 = corner.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
                bar.second.f.mid24 = bar.first.f.mid24;
                if (smith_screen_state == 1) {
                    if (smith_confirm_shown == 0) {
                        screen_part_alphas[253] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[253]);
                    } else { // match: never taken, the loop only runs with smith_confirm_shown clear
                        screen_part_alphas[253] = 80.0f;
                    }
                }
                bar.alpha = (s32)screen_part_alphas[253];
                MenuDraw_Bar(&bar);
                break;
            }
        }
    }
    MenuText_SetColor(menu_draw_color);
    word.raw = 0x0C0975E07C307080;
    for (line = 0, node = item_list_top[0]; line < 7; node = node->prev, line++) {
        def = &item_name_defs[node->item_number];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (item_list_page_mark[0]->prev == node) {
            break;
        }
    }
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = smith_confirm_list_model_angle + PI;
    smith_confirm_list_model_angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
    sceVu0RotMatrixY(rotation, scale, smith_confirm_list_model_angle);
    sceVu0TransMatrix(rotation, rotation, smith_confirm_list_model_position);
    Menu_DrawItemModel(rotation);
    if ((menu_buttons & PAD_TRIANGLE) && modal_input_locked == 0 && Menu_IsEquipSlotFilled(index, item_list_cursor[0]->group)) {
        if (detail_panel_open == 0) {
            smith_confirm_list_panel_sound_pending = 1;
            detail_panel_open = 1;
        } else if (smith_panel_state != 2) {
            smith_panel_state = 2;
            Sound_PlayMenu(1);
        } else {
            smith_panel_state = 0;
        }
    }
    if (detail_panel_open != 0) {
        switch (item_list_cursor[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (smith_confirm_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_confirm_list_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_71 = worn_item_level_records[index].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g_part_count; i++) {
                    full_alpha = shop_g_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g_part_count; i++) {
                SELECT_PART((s8)shop_g_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g_parts[i], shop_g_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (smith_confirm_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_confirm_list_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g2_part_count; i++) {
                    full_alpha = shop_g2_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g2_part_count; i++) {
                SELECT_PART((s8)shop_g2_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g2_parts[i], shop_g2_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (smith_confirm_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_confirm_list_panel_sound_pending = 0;
            menu_number_44 = (weapon_level_records[index].gained_values[0] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[0]) * weapon_level_records[index].level / 100;
            menu_number_45 = (weapon_level_records[index].gained_values[1] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[1]) * weapon_level_records[index].level / 100;
            menu_number_46 = (weapon_level_records[index].gained_values[2] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[2]) * weapon_level_records[index].level / 100;
            menu_number_47 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[3] * weapon_level_records[index].level / 100;
            menu_number_48 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[4] * weapon_level_records[index].level / 100;
            menu_number_49 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[5] * weapon_level_records[index].level / 100;
            menu_number_50 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[6] * weapon_level_records[index].level / 100;
            menu_number_51 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[7] * weapon_level_records[index].level / 100;
            menu_number_52 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[8] * weapon_level_records[index].level / 100;
            menu_number_70 = (s32)(weapon_defs[index].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[index].level;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[item_list_cursor[0]->group];
            menu_number_73 = 100;
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_a_part_count; i++) {
                    full_alpha = shop_a_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_a_part_count; i++) {
                SELECT_PART((s8)shop_a_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_a_parts[i], shop_a_parts[i].handler_index + 1000, 0, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_a_parts;
            count = shop_a_part_count;
            break;
        }
        switch (smith_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        panel_part_alphas[i] = full_alpha;
                    }
                    smith_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    smith_panel_state = 0;
                    detail_panel_open = 0;
                    break;
                }
            }
            break;
        }
    }
    if (smith_confirm_shown != 0) {
        menu_number_76 = item_name_defs[item_list_cursor[0]->item_number].upgrade_wait_minutes;
        for (i = 0; i < tune_mess_part_count; i++) {
            kind = tune_mess_parts[i].handler_index;
            SELECT_PART((s8)tune_mess_parts[i].blend_mode);
            MenuSprite_Draw((int)&tune_mess_parts[i], kind + 1000, 0, (int)message_part_alphas[i]);
            SELECT_PART(0);
        }
        chosen = &item_name_defs[item_list_cursor[0]->item_number];
        MenuText_SetColor(menu_draw_color);
        word.raw = 0x0A2DC6C07F5078C0;
        MenuText_SetPen(word);
        MenuText_DrawLineCentered((u16 *)chosen, 0x18);
    }
    selected = item_list_cursor[0];
    table = Message_GetFontText(menu_heap);
    text_index = selected->item_number + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong((u16 *)(table + entry), 0x33);
    }
    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            if (smith_confirm_shown == 0) {
                Sound_PlayMenu(0);
                smith_screen_state = 2;
                modal_input_locked = 1;
            } else {
                Sound_PlayMenu(0);
                smith_screen_state = 3;
                modal_input_locked = 1;
                if (model_load_chain != NULL) {
                    model_load_chain->next = NULL;
                }
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            if (smith_confirm_shown == 0) {
                smith_screen_state = 4;
                modal_input_locked = 1;
                if (model_load_chain != NULL) {
                    model_load_chain->next = NULL;
                }
            } else {
                smith_screen_state = 5;
                modal_input_locked = 1;
                if (model_load_chain != NULL) {
                    model_load_chain->next = NULL;
                }
            }
        }
    }
    if (smith_screen_state == 2 && model_load_chain == NULL) {
        smith_screen_state = 6;
    }
    if (smith_screen_state == 3 && model_load_chain == NULL) {
        smith_screen_state = 7;
    }
    if (smith_screen_state == 4 && model_load_chain == NULL) {
        smith_screen_state = 8;
    }
    if (smith_screen_state == 5 && model_load_chain == NULL) {
        smith_screen_state = 9;
    }
    switch (smith_screen_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        screen_part_alphas[253] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        screen_part_alphas[252] += 1.6f;
        if (screen_part_alphas[252] > 16.0f) {
            screen_part_alphas[252] = 16.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] += tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > tune_parts[i].alpha) {
                modal_input_locked = 0;
                smith_screen_state = 1;
                for (i = 0; i < tune_part_count; i++) {
                    full_alpha = tune_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                break;
            }
        }
        for (i = 0; i < tune_mess_part_count; i++) {
            message_part_alphas[i] += tune_mess_parts[i].alpha / 10.0f;
            if (message_part_alphas[i] > tune_mess_parts[i].alpha) {
                for (i = 0; i < tune_mess_part_count; i++) {
                    full_alpha = tune_mess_parts[i].alpha;
                    message_part_alphas[i] = full_alpha;
                }
                break;
            }
        }
        break;
    case 6:
    case 7:
    case 8:
    case 9:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[254] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[254] < 0.0f) {
            screen_part_alphas[254] = 0.0f;
        }
        screen_part_alphas[253] -= 1.6f;
        model_draw_color.alpha = (s32)screen_part_alphas[254];
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        screen_part_alphas[252] -= 1.6f;
        if (screen_part_alphas[252] < 0.0f) {
            screen_part_alphas[252] = 0.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] -= tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (smith_screen_state == 6) {
                    smith_handler = Smith_UpdateItemScreen;
                    context->active_list = 1;
                    smith_upgrade_choice = 0;
                } else if (smith_screen_state == 7) {
                    PlayTime_Update();
                    if (item_list_cursor[0]->item_number < 220) {
                        *(ClockTime *)weapon_level_records[item_list_cursor[0]->item_number - 150].last_upgraded.bytes = current_play_time;
                        switch (smith_upgrade_choice) {
                        case 0:
                            weapon_level_records[item_list_cursor[0]->item_number - 150].gained_values[0]++;
                            weapon_level_records[item_list_cursor[0]->item_number - 150].in_smith_rings = 1;
                            break;
                        case 1:
                            weapon_level_records[item_list_cursor[0]->item_number - 150].gained_values[1]++;
                            weapon_level_records[item_list_cursor[0]->item_number - 150].in_smith_rings = 1;
                            break;
                        case 2:
                            weapon_level_records[item_list_cursor[0]->item_number - 150].gained_values[2]++;
                            weapon_level_records[item_list_cursor[0]->item_number - 150].in_smith_rings = 1;
                            break;
                        }
                    } else {
                        *(ClockTime *)worn_item_level_records[item_list_cursor[0]->item_number - 220].last_upgraded.bytes = current_play_time;
                        switch (smith_upgrade_choice) {
                        case 0:
                            worn_item_level_records[item_list_cursor[0]->item_number - 220].gained_values[0]++;
                            worn_item_level_records[item_list_cursor[0]->item_number - 220].in_smith_rings = 1;
                            break;
                        case 1:
                            worn_item_level_records[item_list_cursor[0]->item_number - 220].gained_values[1]++;
                            worn_item_level_records[item_list_cursor[0]->item_number - 220].in_smith_rings = 1;
                            break;
                        case 2:
                            worn_item_level_records[item_list_cursor[0]->item_number - 220].gained_values[2]++;
                            worn_item_level_records[item_list_cursor[0]->item_number - 220].in_smith_rings = 1;
                            break;
                        }
                    }
                    if (item_list_cursor[0]->item_number == player_state.weapon + ITEM_WEAPON_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST ||
                        item_list_cursor[0]->item_number == player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST || item_list_cursor[0]->item_number == player_state.shortcut) {
                        Menu_ClearEquipSlot(item_list_cursor[0]->group);
                    }
                    smith_handler = Smith_UpdateSelectScreen;
                    item_counts[ITEM_EARTH_STONE]--;
                    context->active_list = 1;
                    Menu_ResetLoadBuffers(0);
                } else if (smith_screen_state == 8) {
                    context->active_list = 1;
                    smith_handler = Smith_UpdateSelectScreen;
                    Menu_ResetLoadBuffers(0);
                } else if (smith_screen_state == 9) {
                    smith_confirm_shown = 0;
                }
                smith_unread_flag = 0;
                smith_screen_state = 0;
                smith_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
                break;
            }
        }
        for (i = 0; i < tune_mess_part_count; i++) {
            message_part_alphas[i] -= tune_mess_parts[i].alpha / 10.0f;
            if (message_part_alphas[i] < 0.0f) {
                memset(message_part_alphas, 0, 0x400);
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

    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_DOWN) {
            Sound_PlayMenu(1);
            smith_upgrade_choice++;
        } else if (menu_buttons & PAD_UP) {
            Sound_PlayMenu(1);
            smith_upgrade_choice--;
        }
    }
    line_count = 3;
    smith_upgrade_choice = (s8)(smith_upgrade_choice + 3) % line_count;
    menu_number_74 = 100;
    if (item_list_cursor[0]->item_number < 220) {
        menu_number_75 = GetWeaponLevel(item_list_cursor[0]->item_number - 150);
    } else {
        menu_number_75 = GetItemLevel(item_list_cursor[0]->item_number - 220);
    }
    number = item_list_cursor[0]->item_number;
    index = number - bases.first_item[item_list_cursor[0]->group];
    if (item_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[index].level_index + 1;
    }
    bar_drawn = FALSE;
    for (i = 0; i < tune_s_part_count; i++) {
        if (!bar_drawn && tune_s_parts[i].has_cursor_bar == 1) {
            word.raw = 0x0E0864707DF07490L;
            bar_drawn = TRUE;
            bar.first.f.lo16 = word.f.lo16;
            line_offset = smith_upgrade_choice * 0xB0; // match: kept in a variable, or the sums below fold to constants
            bar.first.f.mid16 = word.f.mid16 + line_offset;
            if (smith_upgrade_choice == 1) {
                bar.first.f.mid16 = word.f.mid16 + line_offset + 0x30;
            } else if (smith_upgrade_choice == 2) {
                bar.first.f.mid16 = word.f.mid16 + line_offset + 0x60;
            }
            bar.first.f.mid24 = word.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x8B0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (smith_screen_state == 1) {
                screen_part_alphas[253] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[253]);
            }
            bar.alpha = (s32)screen_part_alphas[253];
            MenuDraw_Bar(&bar);
        }
        kind = tune_s_parts[i].handler_index;
        SELECT_PART((s8)tune_s_parts[i].blend_mode);
        MenuSprite_Draw((int)&tune_s_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
        SELECT_PART(0);
    }
    MenuText_SetColor(menu_draw_color);
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = smith_item_screen_model_angle + PI;
    smith_item_screen_model_angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
    sceVu0RotMatrixY(rotation, scale, smith_item_screen_model_angle);
    sceVu0TransMatrix(rotation, rotation, smith_item_screen_model_position);
    Menu_DrawItemModel(rotation);
    if ((menu_buttons & PAD_TRIANGLE) && modal_input_locked == 0 && Menu_IsEquipSlotFilled(index, item_list_cursor[0]->group)) {
        if (detail_panel_open == 0) {
            play_sound = TRUE;
            detail_panel_open = 1;
        } else if (smith_panel_state != 2) { // match: this way round for the branch sense
            smith_panel_state = 2;
            Sound_PlayMenu(1);
        } else {
            smith_panel_state = 0;
        }
    }
    if (detail_panel_open != 0) {
        switch (item_list_cursor[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (play_sound) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_71 = worn_item_level_records[index].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            for (i = 0; i < shop_g_part_count; i++) {
                SELECT_PART((s8)shop_g_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g_parts[i], shop_g_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (play_sound) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[item_list_cursor[0]->group];
            for (i = 0; i < shop_g2_part_count; i++) {
                SELECT_PART((s8)shop_g2_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g2_parts[i], shop_g2_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (play_sound) {
                Sound_PlayMenu(1);
            }
            menu_number_44 = (weapon_level_records[index].gained_values[0] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[0]) * weapon_level_records[index].level / 100;
            menu_number_45 = (weapon_level_records[index].gained_values[1] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[1]) * weapon_level_records[index].level / 100;
            menu_number_46 = (weapon_level_records[index].gained_values[2] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[2]) * weapon_level_records[index].level / 100;
            menu_number_47 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[3] * weapon_level_records[index].level / 100;
            menu_number_48 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[4] * weapon_level_records[index].level / 100;
            menu_number_49 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[5] * weapon_level_records[index].level / 100;
            menu_number_50 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[6] * weapon_level_records[index].level / 100;
            menu_number_51 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[7] * weapon_level_records[index].level / 100;
            menu_number_52 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[8] * weapon_level_records[index].level / 100;
            menu_number_70 = (s32)(weapon_defs[index].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[index].level;
            menu_number_54 = item_name_defs[item_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[item_list_cursor[0]->group];
            menu_number_73 = 100;
            for (i = 0; i < shop_a_part_count; i++) {
                SELECT_PART((s8)shop_a_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_a_parts[i], shop_a_parts[i].handler_index + 1000, 0, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_a_parts;
            count = shop_a_part_count;
            break;
        }
        switch (smith_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        panel_part_alphas[i] = full_alpha;
                    }
                    smith_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    smith_panel_state = 0;
                    detail_panel_open = 0;
                    break;
                }
            }
            break;
        }
    }
    selected = item_list_cursor[0];
    table = Message_GetFontText(menu_heap);
    text_index = selected->item_number + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080L;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong(table + entry, 0x33);
    }
    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            Sound_PlayMenu(0);
            smith_screen_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            smith_screen_state = 3;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        }
    }
    if (smith_screen_state == 2 && model_load_chain == NULL) {
        smith_screen_state = 4;
    }
    if (smith_screen_state == 3 && model_load_chain == NULL) {
        smith_screen_state = 5;
    }
    switch (smith_screen_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        screen_part_alphas[253] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        for (i = 0; i < tune_s_part_count; i++) {
            screen_part_alphas[i] += tune_s_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > tune_s_parts[i].alpha) {
                smith_screen_state = 1;
                modal_input_locked = 0;
                for (i = 0; i < tune_s_part_count; i++) {
                    full_alpha = tune_s_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                return;
            }
        }
        break;
    case 4:
    case 5:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[254] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[254] < 0.0f) {
            screen_part_alphas[254] = 0.0f;
        }
        screen_part_alphas[253] -= 1.6f;
        model_draw_color.alpha = (s32)screen_part_alphas[254];
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < tune_s_part_count; i++) {
            screen_part_alphas[i] -= tune_s_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (smith_screen_state == 4) {
                    smith_confirm_shown = 1;
                    smith_handler = smith_list_handlers[1];
                } else if (smith_screen_state == 5) {
                    smith_confirm_shown = 0;
                    smith_handler = smith_list_handlers[1];
                }
                smith_screen_state = 0;
                smith_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
                return;
            }
        }
        break;
    }
}

/* The item list screen over the smith_collect_list_cursor ring, a twin of Smith_UpdateUpgradeList: draws the seven rows, the turning model
   and the selected item's panel, and fades the screen in and out. Leaving with the 0x4000 button clears the
   chosen item's LevelRecord.in_smith_rings before going back to Smith_UpdateSelectScreen. */
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

    if (smith_collect_list_cursor[0] == NULL) {
        smith_handler = Smith_UpdateSelectScreen;
        return;
    }
    if (smith_screen_state < 2) {
        Smith_MoveListCursor(context);
    }
    number = smith_collect_list_cursor[0]->item_number;
    index = number - bases.first_item[smith_collect_list_cursor[0]->group];
    if (smith_collect_list_cursor[0]->group == 0) {
        menu_number_77 = weapon_level_records[index].level_index + 1;
    }
    menu_number_74 = 100;
    if (smith_collect_list_cursor[0]->item_number < 220) {
        menu_number_75 = GetWeaponLevel(smith_collect_list_cursor[0]->item_number - 150);
    } else {
        menu_number_75 = GetItemLevel(smith_collect_list_cursor[0]->item_number - 220);
    }
    for (i = 0; i < tune_part_count; i++) {
        kind = tune_parts[i].handler_index;
        SELECT_PART((s8)tune_parts[i].blend_mode);
        if (kind == 3) {
            switch (tune_parts[i].slot) {
            case 0:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 0, (int)screen_part_alphas[i]);
                break;
            case 1:
                break;
            case 2:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 2, (int)screen_part_alphas[i]);
                break;
            case 3:
                MenuSprite_Draw((int)&tune_parts[i], 1003, 3, (int)screen_part_alphas[i]);
                break;
            }
        } else {
            MenuSprite_Draw((int)&tune_parts[i], kind + 1000, 4, (int)screen_part_alphas[i]);
        }
        SELECT_PART(0);
    }
    item = smith_collect_list_top[0];
    for (row = 0; row < 7; row++, item = item->prev) {
        if (item == smith_collect_list_cursor[0]) {
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
            if (smith_screen_state == 1) {
                screen_part_alphas[253] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[253]);
            }
            bar.alpha = (s32)screen_part_alphas[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }
    MenuText_SetColor(menu_draw_color);
    word.raw = 0x0C7A12007C307080;
    for (line = 0, node = smith_collect_list_top[0]; line < 7; node = node->prev, line++) {
        def = &item_name_defs[node->item_number];
        MenuText_SetPen(word);
        MenuText_DrawLine((u16 *)def, 0x1A);
        word.f.mid16 += 0xB0;
        if (smith_collect_list_page_mark[0]->prev == node) {
            break;
        }
    }
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = smith_collect_list_model_angle + PI;
    smith_collect_list_model_angle = angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
    sceVu0RotMatrixY(rotation, scale, smith_collect_list_model_angle);
    sceVu0TransMatrix(rotation, rotation, smith_collect_list_model_position);
    Menu_DrawItemModel(rotation);
    if ((menu_buttons & PAD_TRIANGLE) && modal_input_locked == 0 && Menu_IsEquipSlotFilled(index, smith_collect_list_cursor[0]->group)) {
        smith_collect_list_panel_sound_pending = 1;
        if (detail_panel_open == 0) {
            detail_panel_open = 1;
        } else {
            smith_panel_state = 2;
        }
    }
    if (detail_panel_open != 0) {
        switch (smith_collect_list_cursor[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (smith_collect_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_collect_list_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_71 = worn_item_level_records[index].level;
            menu_number_72 = 100;
            menu_number_54 = item_name_defs[smith_collect_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[smith_collect_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g_part_count; i++) {
                    full_alpha = shop_g_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g_part_count; i++) {
                SELECT_PART((s8)shop_g_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g_parts[i], shop_g_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g_parts;
            count = shop_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            if (smith_collect_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_collect_list_panel_sound_pending = 0;
            menu_number_44 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            menu_number_45 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            menu_number_46 = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            menu_number_47 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            menu_number_48 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            menu_number_49 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            menu_number_50 = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            menu_number_51 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            menu_number_52 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            menu_number_54 = item_name_defs[smith_collect_list_cursor[0]->item_number].weight_tenths_of_pound;
            variant = player_weapon[smith_collect_list_cursor[0]->group];
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_g2_part_count; i++) {
                    full_alpha = shop_g2_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_g2_part_count; i++) {
                SELECT_PART((s8)shop_g2_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_g2_parts[i], shop_g2_parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_g2_parts;
            count = shop_g2_part_count;
            break;
        case 0:
            if (smith_collect_list_panel_sound_pending != 0) {
                Sound_PlayMenu(1);
            }
            smith_collect_list_panel_sound_pending = 0;
            menu_number_44 = (weapon_level_records[index].gained_values[0] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[0]) * weapon_level_records[index].level / 100;
            menu_number_45 = (weapon_level_records[index].gained_values[1] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[1]) * weapon_level_records[index].level / 100;
            menu_number_46 = (weapon_level_records[index].gained_values[2] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[2]) * weapon_level_records[index].level / 100;
            menu_number_47 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[3] * weapon_level_records[index].level / 100;
            menu_number_48 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[4] * weapon_level_records[index].level / 100;
            menu_number_49 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[5] * weapon_level_records[index].level / 100;
            menu_number_50 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[6] * weapon_level_records[index].level / 100;
            menu_number_51 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[7] * weapon_level_records[index].level / 100;
            menu_number_52 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[8] * weapon_level_records[index].level / 100;
            menu_number_70 = (s32)(weapon_defs[index].weapon_length_cm * CM_TO_FEET * 10.0f);
            menu_number_71 = weapon_level_records[index].level;
            menu_number_54 = item_name_defs[smith_collect_list_cursor[0]->item_number].weight_tenths_of_pound;
            menu_number_72 = player_weapon[smith_collect_list_cursor[0]->group];
            menu_number_73 = 100;
            if (smith_panel_state == 1) {
                for (i = 0; i < shop_a_part_count; i++) {
                    full_alpha = shop_a_parts[i].alpha;
                    panel_part_alphas[i] = full_alpha;
                }
            }
            for (i = 0; i < shop_a_part_count; i++) {
                SELECT_PART((s8)shop_a_parts[i].blend_mode);
                MenuSprite_Draw((int)&shop_a_parts[i], shop_a_parts[i].handler_index + 1000, 0, (int)panel_part_alphas[i]);
                SELECT_PART(0);
            }
            parts = shop_a_parts;
            count = shop_a_part_count;
            break;
        }
        switch (smith_panel_state) {
        case 0:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        panel_part_alphas[i] = full_alpha;
                    }
                    smith_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    detail_panel_open = 0;
                    smith_panel_state = 0;
                    break;
                }
            }
            break;
        }
    }
    selected = smith_collect_list_cursor[0];
    table = Message_GetFontText(menu_heap);
    text_index = selected->item_number + MSG_ITEM_TEXT_FIRST + 1;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong(table + entry, 0x33);
    }
    if (modal_input_locked == 0) {
        if (menu_buttons & PAD_CROSS) {
            Sound_PlayMenu(0);
            smith_screen_state = 2;
            smith_panel_state = 3;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        } else if (menu_buttons & PAD_CIRCLE) {
            Sound_PlayMenu(2);
            smith_screen_state = 3;
            smith_panel_state = 2;
            modal_input_locked = 1;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        }
    }
    if (smith_screen_state == 2 && model_load_chain == NULL) {
        smith_screen_state = 4;
    }
    if (smith_screen_state == 3 && model_load_chain == NULL) {
        smith_screen_state = 5;
    }
    switch (smith_screen_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        screen_part_alphas[253] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] += tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > tune_parts[i].alpha) {
                smith_screen_state = 1;
                modal_input_locked = 0;
                for (i = 0; i < tune_part_count; i++) {
                    full_alpha = tune_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                return;
            }
        }
        break;
    case 4:
    case 5:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[254] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[254] < 0.0f) {
            screen_part_alphas[254] = 0.0f;
        }
        screen_part_alphas[253] -= 1.6f;
        model_draw_color.alpha = (s32)screen_part_alphas[254];
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < tune_part_count; i++) {
            screen_part_alphas[i] -= tune_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                if (smith_screen_state == 4) {
                    if (smith_collect_list_cursor[0]->item_number < 220) {
                        ClearWeaponMark(smith_collect_list_cursor[0]->item_number - 150);
                    } else {
                        ClearItemMark(smith_collect_list_cursor[0]->item_number - 220);
                    }
                    context->active_list = 2;
                    smith_handler = Smith_UpdateSelectScreen;
                    Menu_ResetLoadBuffers(0);
                } else if (smith_screen_state == 5) {
                    context->active_list = 2;
                    smith_handler = Smith_UpdateSelectScreen;
                    Menu_ResetLoadBuffers(0);
                }
                smith_screen_state = 0;
                smith_panel_state = 0;
                memset(screen_part_alphas, 0, 0x400);
                return;
            }
        }
        break;
    }
}

/* Trade_MoveListCursor for the smith_collect_list_cursor list: moves the cursor one row (0x10, 0x40) or one page (0x20, 0x80, lists of 8
   and more), scrolling the window, then renames the link nodes' models. TRUE when a button was handled. */
static s32 Smith_MoveListCursor(HandlerContext *context) {
    MenuItem *cursor = smith_collect_list_cursor[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if (menu_buttons & PAD_UP) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == smith_collect_list_top[0] && next != smith_collect_list_page_mark[0]->prev) {
                    smith_collect_list_top[0] = next;
                    smith_collect_list_page_mark[0] = smith_collect_list_page_mark[0]->next;
                }
                Sound_PlayMenu(1);
                smith_collect_list_cursor[0] = cursor->next;
            }
            Menu_ShiftListModelsNext(0);
        } else if (menu_buttons & PAD_DOWN) {
            if (cursor->prev != NULL) {
                if (smith_collect_list_page_mark[0]->prev == cursor && smith_collect_list_top[0]->next != cursor) {
                    smith_collect_list_page_mark[0] = cursor;
                    smith_collect_list_top[0] = smith_collect_list_top[0]->prev;
                }
                Sound_PlayMenu(1);
                smith_collect_list_cursor[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((menu_buttons & PAD_RIGHT) && item_list_count >= 8) {
            if (cursor->prev != NULL) {
                if (smith_collect_list_top[0]->next != smith_collect_list_page_mark[0]->prev) {
                    rows = 0;
                    while (Smith_CollectListScrollUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageUp(0);
        } else if ((menu_buttons & PAD_LEFT) && item_list_count >= 8) {
            if (cursor->next != NULL) {
                if (smith_collect_list_top[0]->next != smith_collect_list_page_mark[0]->prev) {
                    rows = 0;
                    while (Smith_CollectListScrollDown()) {
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
    SaveScreen_DrawBackground,
    SaveScreen_RunStep,
};
static int (*const save_screen_steps[4])(HandlerContext *) IN_RODATA = {
    SaveScreen_Start,
    SaveScreen_RunHandler,
    SaveScreen_CutModelChain,
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
    menu_mode = 0;
    modal_pad_mode = modal_pad_mode_last = Pad_GetMode(0, 0);
    while (save_screen_run_steps[save_screen_run_step](0) != 0) {
        sceGsSyncPath(0, 0);
        modal_even_field = Menu_EndFrame(1);
        Input_Update();
        field = Pad_GetMode(0, 0);
        modal_pad_mode = field;
        if (field != modal_pad_mode_last) {
            save_screen_pad_change_frames = 1;
        }
        modal_pad_mode_last = field;
        MenuPad_Update();
        if (save_screen_pad_change_frames > 0) {
            if (++save_screen_pad_change_frames >= 0x15) {
                save_screen_pad_change_frames = 0;
            } else {
                menu_buttons = 0;
            }
        }
    }
    sceGsSyncPath(0, 0);
    while (sceGsSyncV(0) != 0) {
    }
    Sound_UnmutePositional();
    MenuHeap_FreeToMark();
    save_screen_step = 0;
    save_screen_run_step = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}

/* Fills the GIF packet that draws the screen background and sends it over DMA channel 2. */
static s32 SaveScreen_DrawBackground(int unused) {
    char reserved[0x70]; /* unused, but retail reserves the 0x70 bytes */
    GsFillPacket *packet;
    u32 flag;

    packet = MenuDraw_NextPacket();
    flag = context1_buffer_half != 0 ? 0x1180 : 0;
    packet->gif_tag_prim = 0x100B4000;
    packet->gif_tag_register_list = 0xE;
    packet->gif_tag_register_list_hi = 0;
    packet->work_target_frame = (menu_first_draw_env[0] & ~0x1FFLL) | 0x192;
    packet->work_target_frame_register = 0x4C;
    packet->work_target_frame_pad = 0;
    packet->xyoffset = 0;
    packet->xyoffset_hi = 0;
    packet->xyoffset_register = 0x18;
    packet->xyoffset_pad = 0;
    packet->test = 0x30000;
    packet->test_hi = 0;
    packet->test_register = 0x47;
    packet->test_pad = 0;
    packet->scissor_x = 0x02800000;
    packet->scissor_y = 0xE00000;
    packet->scissor_register = 0x40;
    packet->scissor_pad = 0;
    packet->source_tex0 = (s64)(flag | 0x28028000 | ((s64)0xC800 << 19));
    packet->source_tex0_register = 6;
    packet->source_tex0_pad = 0;
    packet->rgbaq_rgba = 0x80808080;
    packet->rgbaq_q = 1.0f;
    packet->rgbaq_register = 1;
    packet->rgbaq_pad = 0;
    packet->source_st_corner0_s = 0;
    packet->source_st_corner0_t = 0;
    packet->source_st_corner0_register = 2;
    packet->source_st_corner0_pad = 0;
    packet->source_xy_corner0 = 0;
    packet->source_z_corner0 = 0;
    packet->source_xy_corner0_register = 5;
    packet->source_xy_corner0_pad = 0;
    packet->source_st_corner1_s = 0.625f;
    packet->source_st_corner1_t = 0.875f;
    packet->source_st_corner1_register = 2;
    packet->source_st_corner1_pad = 0;
    packet->source_xy_corner1 = 0x0E002800;
    packet->source_z_corner1 = 0;
    packet->source_xy_corner1_register = 5;
    packet->source_xy_corner1_pad = 0;
    packet->texflush = 0;
    packet->texflush_hi = 0;
    packet->texflush_register = 0x3F;
    packet->texflush_pad = 0;
    packet->first_frame = menu_first_draw_env[0];
    packet->first_frame_register = 0x4C;
    packet->first_frame_pad = 0;
    packet->work_tex0 = 0x66802B240LL;
    packet->work_tex0_register = 6;
    packet->work_tex0_pad = 0;
    packet->first_frame_st_corner0_s = 0;
    packet->first_frame_st_corner0_t = 0;
    packet->first_frame_st_corner0_register = 2;
    packet->first_frame_st_corner0_pad = 0;
    packet->first_frame_xy_corner0 = 0;
    packet->first_frame_z_corner0 = 0;
    packet->first_frame_xy_corner0_register = 5;
    packet->first_frame_xy_corner0_pad = 0;
    packet->first_frame_st_corner1_s = 0.625f;
    packet->first_frame_st_corner1_t = 0.4375f;
    packet->first_frame_st_corner1_register = 2;
    packet->first_frame_st_corner1_pad = 0;
    packet->first_frame_xy_corner1 = 0x0E002800;
    packet->first_frame_z_corner1 = 0;
    packet->first_frame_xy_corner1_register = 5;
    packet->first_frame_xy_corner1_pad = 0;
    packet->second_frame = menu_first_draw_env[0x2E];
    packet->second_frame_register = 0x4C;
    packet->second_frame_pad = 0;
    packet->second_frame_st_corner0_s = 0;
    packet->second_frame_st_corner0_t = 0;
    packet->second_frame_st_corner0_register = 2;
    packet->second_frame_st_corner0_pad = 0;
    packet->second_frame_xy_corner0 = 0;
    packet->second_frame_z_corner0 = 0;
    packet->second_frame_xy_corner0_register = 5;
    packet->second_frame_xy_corner0_pad = 0;
    packet->second_frame_st_corner1_s = 0.625f;
    packet->second_frame_st_corner1_t = 0.4375f;
    packet->second_frame_st_corner1_register = 2;
    packet->second_frame_st_corner1_pad = 0;
    packet->second_frame_xy_corner1 = 0x0E002800;
    packet->second_frame_z_corner1 = 0;
    packet->second_frame_xy_corner1_register = 5;
    packet->second_frame_xy_corner1_pad = 0;
    packet->test_again = 0x50000;
    packet->test_again_hi = 0;
    packet->test_again_register = 0x47;
    packet->test_again_pad = 0;
    packet->texflush_again = 0;
    packet->texflush_again_hi = 0;
    packet->texflush_again_register = 0x3F;
    packet->texflush_again_pad = 0;
    packet->alpha = 0x44;
    packet->alpha_hi = 0;
    packet->alpha_register = 0x42;
    packet->alpha_pad = 0;
    packet->gif_tag = 0x8019;
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 0x1A;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    sceGsSyncPath(0, 0);
    menu_draw_color.red = menu_draw_color.green = menu_draw_color.blue = 0x80;
    menu_draw_color_alpha = 0;
    model_draw_color.red = model_draw_color.green = model_draw_color.blue = 0x80;
    model_draw_color_alpha = 0;
    save_screen_run_step++;
    return 1;
}

/* Calls the handler selected by save_screen_step with the context at save_screen_context, storing value in it first. */
static int SaveScreen_RunStep(int value) {
    int (*handler)(HandlerContext *);

    Menu_StepModelLoads(0);
    handler = save_screen_steps[save_screen_step];
    save_screen_context.arg = value;
    if (handler != NULL) {
        return handler(&save_screen_context);
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

    Packet_SwapBuffers();
    Packet_OnFrameStart();
    TextureTable_QueueAll((TextureTableHeader *)Message_GetFontTexture(menu_heap));
    GsDraw_QueueTexFlush();
    Packet_OnFrameEnd();
    Packet_SendChain();
    ((s32 (*)(void))Packet_WaitForSend)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < menu_file_block_count; i++) {
        block = &menu_file_blocks[i];
        MenuTexture_Upload(block->data, block);
    }
    MenuArena_Init(trade_menu_arena);
    packet = MenuDraw_NextPacket();
    packet[0] = 0x8001;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 0x3F;
    packet[7] = 0;
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 2;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    Menu_InitLoadBuffers();
    // match: taking the handler first orders its lui before the save screen's
    next_handler = SaveScreen_UpdateCardSelect;
    screen = &save_screen_state;
    screen->dialog_id = -1;
    screen->card_update_result = 0;
    save_screen_handler = next_handler;
    // match: this store order schedules into retail's unk18, unk11, unk14, ... order
    screen->card_op_running = 0;
    screen->selected_card = 0;
    screen->selected_save = 1;
    screen->confirm_choice = 1;
    screen->check_finished = 0;
    screen->save_state_0 = 0;
    screen->save_state_1 = 0;
    screen->save_state_2 = 0;
    screen->input_locked = 1;
    screen->is_leaving = 0;
    SaveCard_Open();
    SaveCard_StartCheck(1);
    memset(screen_part_alphas, 0, 0x400);
    memset(panel_part_alphas, 0, 0x400);
    memset(message_part_alphas, 0, 0x400);
    pulse_sprite_falling = 0;
    pulse_sprite_alpha = 128.0f;
    save_screen_step++;
    return TRUE;
}

/* Runs the handler in save_screen_handler. */
static int SaveScreen_RunHandler(HandlerContext *context) {
    save_screen_handler();
    return 1;
}

/* Cuts the chain after the first link node (and after the node it points at); with no chain, resets save_screen_step. */
static int SaveScreen_CutModelChain(HandlerContext *context) {
    LinkNode *head;
    LinkNode *child;

    head = model_load_chain;
    if (head == NULL) {
        save_screen_step = 0;
        return 0;
    }
    child = head->target_node;
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
    if (SaveCard_HasCard(save_screen_state.selected_card) == 0 && save_screen_state.dialog_id == 14) {
        card_result = -1;
    }
    if (save_screen_state.check_finished == 0 && SaveCard_IsCheckRunning() == 0) {
        save_screen_state.input_locked = 0;
        save_screen_state.check_finished = 1;
        if (SaveCard_IsPresent(0) == 0 && SaveCard_IsPresent(1) != 0) {
            save_screen_state.selected_card = 1;
        }
    }

    for (i = 0; i < slot_part_count; i++) {
        kind = slot_parts[i].handler_index;
        SELECT_PART((s8)slot_parts[i].blend_mode);
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0, screen_part_alphas[i]);
            break;
        case 29:
            slot = slot_parts[i].slot;
            if (slot == 0 || slot == 2) {
                MenuSprite_Draw((int)&slot_parts[i], kind + 1000, slot, screen_part_alphas[i]);
            }
            break;
        case 31:
            if (save_screen_state.check_finished != 0) {
                if (system_screen_state != 1) {
                    MenuSprite_Draw((int)&slot_parts[i], kind + 1000, save_screen_state.selected_card, screen_part_alphas[i]);
                } else {
                    MenuSprite_Draw((int)&slot_parts[i], kind, save_screen_state.selected_card, screen_part_alphas[i]);
                }
            }
            break;
        case 3:
            switch (save_screen_state.dialog_id) {
            case 4:
            case 5:
            case 10:
            case 12:
            case 13:
            case 14:
            case 15:
            case 21:
                MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0, screen_part_alphas[i]);
                break;
            }
            break;
        case 26:
            if (SaveCard_IsPresent(0) == 0) {
                if (system_screen_state == 0) {
                    MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0x20, screen_part_alphas[i]);
                } else if (system_screen_state < 2) {
                    MenuSprite_DrawPlain((int)&slot_parts[i], kind, 0x20);
                }
            } else if (system_screen_state != 1) {
                MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0x80, screen_part_alphas[i]);
            } else {
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 0x80);
            }
            break;
        case 27:
            if (SaveCard_IsPresent(1) == 0) {
                if (system_screen_state == 0) {
                    MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0x20, screen_part_alphas[i]);
                } else if (system_screen_state < 2) {
                    MenuSprite_DrawPlain((int)&slot_parts[i], kind, 0x20);
                }
            } else if (system_screen_state != 1) {
                MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0x80, screen_part_alphas[i]);
            } else {
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 0x80);
            }
            break;
        case 28:
            switch (save_screen_state.dialog_id) {
            case 4:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 4);
                break;
            case 5:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 5);
                break;
            case 12:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 12);
                break;
            case 13:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 13);
                break;
            case 14:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 14);
                break;
            case 15:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 15);
                break;
            case 21:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 21);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 10);
                break;
            case 22:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 22);
                break;
            }
            break;
        default:
            MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0, screen_part_alphas[i]);
            break;
        }
        SELECT_PART(0);
    }

    if (save_screen_state.dialog_id == 12) {
        // match: a depth nothing reads; without it the 64-bit mask of the bar's depth field is loaded after the branch below
        unused_corner.f.mid24 = 0x5F370;
        bar.first.f.lo16 = 0x7580;
        bar.first.f.mid16 = save_screen_state.selected_save * 0xB0 + 0x80E0;
        if (save_screen_state.selected_save != 0) {
            bar.first.f.mid16 = save_screen_state.selected_save * 0xB0 + 0x80F0;
        }
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1540;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        screen_part_alphas[0xFF] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[0xFF]);
        bar.alpha = screen_part_alphas[0xFF];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    if (save_screen_state.confirm_pending != 0) {
        if (SaveCard_IsCheckRunning() == 0) {
            save_screen_state.confirm_pending = 0;
            save_screen_state.input_locked = 0;
            if (save_screen_state.dialog_id == 12) {
                if (save_screen_state.selected_save == 0) {
                    SaveCard_StartFormat(save_screen_state.selected_card);
                    save_screen_state.dialog_id = 13;
                    save_screen_state.card_op_running = 1;
                } else {
                    save_screen_state.dialog_id = -1;
                }
            } else if (save_screen_state.dialog_id == 14) {
                if (SaveCard_StartReadList(save_screen_state.selected_card, 0) == 0) {
                    save_screen_state.card_op_running = 1;
                }
            } else if (save_screen_state.dialog_id == 21) {
                save_screen_state.dialog_id = -1;
            } else if (save_screen_state.dialog_id != -1) {
                save_screen_state.dialog_id = -1;
            } else if (SaveCard_IsPresent(save_screen_state.selected_card) != 0) {
                if (SaveCard_IsFormatted(save_screen_state.selected_card) == 0) {
                    save_screen_state.dialog_id = 12;
                } else if (SaveCard_HasSaveFolder(save_screen_state.selected_card) == 0 && SaveCard_HasRoomForFolder(save_screen_state.selected_card) < 0) {
                    save_screen_state.dialog_id = 21;
                    SaveCard_StartCheck(1);
                } else if (SaveCard_StartReadList(save_screen_state.selected_card, 1) == 0) {
                    save_screen_state.card_op_running = 1;
                }
            } else if (save_screen_state.selected_card == 0) {
                save_screen_state.dialog_id = 4;
            } else if (save_screen_state.selected_card == 1) {
                save_screen_state.dialog_id = 5;
            }
        }
    } else if (save_screen_state.card_op_running != 0) {
        if (SaveCard_CheckBusy() == 0) {
            if (save_screen_state.dialog_id == 13) {
                save_screen_state.dialog_id = 14;
                save_screen_state.selected_save = 1;
                save_screen_state.list_step = -1;
            } else if (SaveCard_HasAnySave() != 0 || SaveCard_HasRoomForFolder(save_screen_state.selected_card) > 0) {
                save_screen_state.input_locked = 1;
                save_screen_state.list_step = -1;
                system_screen_state = 2;
                save_screen_state.selected_save = 0;
            } else {
                save_screen_state.dialog_id = 21;
                SaveCard_StartCheck(1);
            }
            save_screen_state.card_op_running = 0;
        } else if (card_result < 0) {
            if (save_screen_state.dialog_id == 13) {
                save_screen_state.dialog_id = 15;
                save_screen_state.card_op_running = 0;
                save_screen_state.selected_save = 1;
                SaveCard_StartCheck(1);
                card_result = 0;
            } else if (save_screen_state.dialog_id == -1) {
                if (SaveCard_IsDamaged(save_screen_state.selected_card) != 0) {
                    if (SaveCard_HasAnySave() != 0 || SaveCard_HasRoomForFolder(save_screen_state.selected_card) > 0) {
                        save_screen_state.input_locked = 1;
                        system_screen_state = 2;
                        save_screen_state.list_step = -1;
                        save_screen_state.selected_save = 0;
                        SaveCard_Cancel(save_screen_state.selected_card);
                    } else {
                        save_screen_state.dialog_id = 21;
                        SaveCard_StartCheck(1);
                    }
                } else {
                    save_screen_state.dialog_id = 10;
                }
                card_result = 0;
                save_screen_state.card_op_running = 0;
            }
        }
    }

    if (save_screen_state.is_leaving != 0) {
        if (card_result < 0) {
            save_screen_state.input_locked = 1;
            system_screen_state = 3;
        }
    } else if (save_screen_state.dialog_id == 12 && SaveCard_IsPresent(save_screen_state.selected_card) == 0) {
        save_screen_state.dialog_id = 10;
        save_screen_state.selected_save = 1;
    } else if (card_result < 0) {
        save_screen_state.dialog_id = 10;
        save_screen_state.selected_save = 1;
        save_screen_state.card_op_running = 0;
        SaveCard_StartCheck(1);
    } else if (save_screen_state.card_op_running == 0 && save_screen_state.input_locked == 0) {
        if ((menu_buttons & (PAD_LEFT | PAD_RIGHT)) && modal_pad_mode != 0xFF && save_screen_state.dialog_id == -1) {
            Sound_PlayMenu(1);
            save_screen_state.selected_card = (save_screen_state.selected_card + 1) & 1;
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & PAD_CROSS) && modal_pad_mode != 0xFF) {
                Sound_PlayMenu(0);
                save_screen_state.confirm_pending = 1;
                save_screen_state.input_locked = 1;
            } else {
                report = Pad_GetReport(0, 0);
                if ((report->pressed & PAD_CIRCLE) && modal_pad_mode != 0xFF) {
                    Sound_PlayMenu(2);
                    if (save_screen_state.dialog_id == 14) {
                        if (SaveCard_StartReadList(save_screen_state.selected_card, 0) == 0) {
                            save_screen_state.card_op_running = 1;
                        }
                    } else if (save_screen_state.dialog_id != -1) {
                        save_screen_state.dialog_id = -1;
                        save_screen_state.selected_save = 1;
                    } else {
                        save_screen_state.is_leaving = 1;
                        SaveCard_Close();
                    }
                } else if ((menu_buttons & PAD_UP) && modal_pad_mode != 0xFF && save_screen_state.dialog_id == 12) {
                    Sound_PlayMenu(1);
                    save_screen_state.selected_save = (save_screen_state.selected_save + 1) & 1;
                } else if ((menu_buttons & PAD_DOWN) && modal_pad_mode != 0xFF && save_screen_state.dialog_id == 12) {
                    Sound_PlayMenu(1);
                    save_screen_state.selected_save = (save_screen_state.selected_save - 1) & 1;
                }
            }
        }
    }

    switch (system_screen_state) {
    case 0:
        for (i = 0; i < slot_part_count; i++) {
            // match: the cursor bar's level is stepped once for every part, inside the loop
            screen_part_alphas[0xFF] += 1.6f;
            if (screen_part_alphas[0xFF] > 16.0f) {
                screen_part_alphas[0xFF] = 16.0f;
            }
            if (slot_parts[i].handler_index == 26) {
                if (SaveCard_IsPresent(0) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x20;
                }
            } else if (slot_parts[i].handler_index == 27) {
                if (SaveCard_IsPresent(1) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x20;
                }
            }
            screen_part_alphas[i] += SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha < screen_part_alphas[i]) {
                for (i = 0; i < slot_part_count; i++) {
                    screen_part_alphas[i] = SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha;
                }
                system_screen_state = 1;
                if (save_screen_state.check_finished != 0) {
                    save_screen_state.input_locked = 0;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < slot_part_count; i++) {
            screen_part_alphas[0xFF] -= 1.6f; // match: stepped once for every part here too
            if (screen_part_alphas[0xFF] < 0.0f) {
                screen_part_alphas[0xFF] = 0.0f;
            }
            if (slot_parts[i].handler_index == 26) {
                if (SaveCard_IsPresent(0) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x20;
                }
            } else if (slot_parts[i].handler_index == 27) {
                if (SaveCard_IsPresent(1) != 0) {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x80;
                } else {
                    SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha = 0x20;
                }
            }
            screen_part_alphas[i] -= SPRITE_PACKET_COLOR_VIEW(&slot_parts[i])->alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                if (system_screen_state == 2) {
                    save_screen_state.confirm_press_state = 0;
                    save_screen_handler = SaveScreen_UpdateSaveList;
                    save_screen_state.free_clusters = 0;
                    save_screen_state.selected_save = 0;
                    pulse_value_falling = 0;
                    pulse_sprite_falling = 0;
                    card_flags = SaveCard_GetSaveMask();
                    if (card_flags != 0 && (card_flags & SAVE_EXISTS_0)) {
                        save_line_write_slots[0] = 0;
                        if (card_flags & SAVE_EXISTS_1) {
                            save_line_write_slots[1] = 1;
                            save_line_write_slots[2] = 2;
                        } else {
                            save_line_write_slots[1] = 1;
                            save_line_write_slots[2] = 1;
                        }
                    } else {
                        save_line_write_slots[0] = 0;
                        save_line_write_slots[1] = 0;
                        save_line_write_slots[2] = 0;
                    }
                    save_line_slots[0] = 0;
                    save_line_slots[1] = 1;
                    save_line_slots[2] = 2;
                } else if (system_screen_state == 3) {
                    save_screen_state.selected_save = 0;
                    save_screen_step++;
                    MenuHeap_FreeToMark();
                }
                save_screen_state.dialog_id = -1;
                system_screen_state = 0;
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

    info[0] = (CardSaveInfo *)SaveCard_GetSaveInfo(save_line_slots[0]);
    menu_number_44 = info[0]->level;
    menu_number_45 = info[0]->experience;
    menu_number_46 = info[0]->gold;
    menu_number_47 = info[0]->hp_now;
    menu_number_48 = info[0]->hp_max;
    menu_number_49 = info[0]->mp_now;
    menu_number_50 = info[0]->mp_max;
    menu_number_51 = info[0]->power_base;
    menu_number_52 = info[0]->magic_base;
    menu_number_53 = info[0]->play_hours;
    menu_number_54 = info[0]->play_minutes;
    menu_number_70 = info[0]->play_seconds;
    info[1] = (CardSaveInfo *)SaveCard_GetSaveInfo(save_line_slots[1]);
    menu_number_71 = info[1]->level;
    menu_number_72 = info[1]->experience;
    menu_number_73 = info[1]->gold;
    menu_number_74 = info[1]->hp_now;
    menu_number_75 = info[1]->hp_max;
    menu_number_76 = info[1]->mp_now;
    menu_number_77 = info[1]->mp_max;
    menu_number_78 = info[1]->power_base;
    menu_number_79 = info[1]->magic_base;
    menu_number_80 = info[1]->play_hours;
    menu_number_81 = info[1]->play_minutes;
    menu_number_82 = info[1]->play_seconds;
    info[2] = (CardSaveInfo *)SaveCard_GetSaveInfo(save_line_slots[2]);
    menu_number_83 = info[2]->level;
    menu_number_84 = info[2]->experience;
    menu_number_85 = info[2]->gold;
    menu_number_86 = info[2]->hp_now;
    menu_number_87 = info[2]->hp_max;
    menu_number_88 = info[2]->mp_now;
    menu_number_89 = info[2]->mp_max;
    menu_number_90 = info[2]->power_base;
    menu_number_91 = info[2]->magic_base;
    menu_number_92 = info[2]->play_hours;
    menu_number_93 = info[2]->play_minutes;
    menu_number_94 = info[2]->play_seconds;

    if ((save_screen_state.list_step == -1 && (save_screen_state.free_clusters == 0 || save_screen_state.free_clusters != SaveCard_GetFreeClusters(save_screen_state.selected_card))) ||
        (save_screen_state.list_step == 2 && save_screen_state.free_clusters != SaveCard_GetFreeClusters(save_screen_state.selected_card))) {
        save_screen_state.free_clusters = 0;
        for (i = 0; i < 3; i++) {
            MENU_SCREEN_SAVES(&save_screen_state)->save_state[i] = SaveCard_GetSaveState(i);
        }
        for (i = 0; i < 3; i++) {
            if (MENU_SCREEN_SAVES(&save_screen_state)->save_state[i] != 1) {
                switch (i) {
                case 0:
                    if (save_screen_state.save_state_1 > 0 || save_screen_state.save_state_2 > 0) {
                        first_arg = 1;
                        second_arg = 0;
                    } else {
                        first_arg = 0;
                        second_arg = 1;
                    }
                    break;
                case 1:
                    if (save_screen_state.save_state_0 > 0 && save_screen_state.save_state_2 > 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (save_screen_state.save_state_0 > 0 && save_screen_state.save_state_2 == 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (save_screen_state.save_state_0 == 0 && save_screen_state.save_state_2 > 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (save_screen_state.save_state_0 == 0 && save_screen_state.save_state_2 == 0) { // match: no else: a negative byte sets neither argument
                        second_arg = 1;
                        first_arg = 1;
                    }
                    break;
                case 2:
                    if (save_screen_state.save_state_0 > 0 && save_screen_state.save_state_1 > 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (save_screen_state.save_state_0 > 0 && save_screen_state.save_state_1 == 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (save_screen_state.save_state_0 == 0 && save_screen_state.save_state_1 > 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (save_screen_state.save_state_0 == 0 && save_screen_state.save_state_1 == 0) { // match: a last test, not an else: retail keeps a path that sets neither
                        second_arg = 1;
                        first_arg = 2;
                    }
                    break;
                }
                if (SaveCard_HasRoomFor(save_screen_state.selected_card, first_arg, second_arg) > 0) {
                    MENU_SCREEN_SAVES(&save_screen_state)->save_state[i] = 0;
                } else {
                    MENU_SCREEN_SAVES(&save_screen_state)->save_state[i] = -1;
                }
            }
        }
    }

    for (i = 0; i < data_part_count; i++) {
        kind = data_parts[i].handler_index;
        slot = data_parts[i].slot;
        ((int (*)(s8))Menu_SetBlendMode)((s8)data_parts[i].blend_mode); // match: a signed byte, see SET_PART_MODE_OF
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&data_parts[i], 0x3EA, 0, screen_part_alphas[i]);
            break;
        case 3:
            group = slot / 3;
            if (MENU_SCREEN_SAVES(&save_screen_state)->save_state[save_line_slots[group]] == 1) {
                MenuSprite_Draw((int)&data_parts[i], 0x3EB, group * 3, screen_part_alphas[i]);
            } else if (MENU_SCREEN_SAVES(&save_screen_state)->save_state[save_line_slots[group]] == 0) {
                MenuSprite_Draw((int)&data_parts[i], 0x3EB, group * 3 + 1, screen_part_alphas[i]);
            }
            break;
        case 25:
            if (MENU_SCREEN_SAVES(&save_screen_state)->save_state[save_line_slots[slot]] != -1) {
                MenuSprite_Draw((int)&data_parts[i], 0x401, save_screen_state.selected_save, screen_part_alphas[i]);
            }
            break;
        default:
            MenuSprite_Draw((int)&data_parts[i], kind + 1000, save_screen_state.selected_save, screen_part_alphas[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    table = (s32 *)Message_GetFontText(menu_heap);
    word.raw = 0x000A87507C107120;
    for (i = 0; i < 3; i++) {
        if (MENU_SCREEN_SAVES(&save_screen_state)->save_state[save_line_slots[i]] == 1) {
            kind = table[info[i]->save_area + 1]; // match: the text's offset in the kind variable: retail keeps both in one register
            if (kind != 0) {
                text = (char *)table + kind;
                MenuText_SetPen(word);
                MenuText_SetColor(menu_draw_color);
                MenuText_DrawWrapped((TradeValue *)text, 0x18);
            }
        }
        word.f.mid16 += 0x340;
    }

    for (i = 0; i < mc_part_count; i++) {
        kind = mc_parts[i].handler_index;
        SET_PART_MODE_OF(&mc_parts[i]);
        if (kind == 3) {
            switch (save_screen_state.list_step) {
            case 6:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 6);
                break;
            case 7:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 7);
                break;
            case 0:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 0);
                break;
            case 2:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 2);
                break;
            case 20:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 20);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 10);
                break;
            case 11:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 11);
                break;
            }
        } else {
            switch (save_screen_state.list_step) {
            case 0:
            case 2:
            case 6:
            case 7:
            case 10:
            case 11:
            case 20:
                MenuSprite_Draw((int)&mc_parts[i], kind + 1000, 0, screen_part_alphas[i]);
                break;
            }
        }
        SET_PART_MODE(0);
    }

    if (save_screen_state.list_step == 6 || save_screen_state.list_step == 7) {
        bar.first.f.lo16 = 0x7490;
        bar.first.f.mid16 = save_screen_state.confirm_choice * 0xC0 + 0x80A0;
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1720;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        screen_part_alphas[0xFF] = Trade_PulseValue(0x10, 0x20, screen_part_alphas[0xFF]);
        bar.alpha = screen_part_alphas[0xFF];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    save_screen_state.card_update_result = SaveCard_Update();
    if (SaveCard_HasCard(save_screen_state.selected_card) == 0) {
        save_screen_state.card_update_result = -1;
    }
    if (save_screen_state.card_update_result < 0) {
        if (save_screen_state.list_step == 0 || save_screen_state.list_step == 11) {
            save_screen_state.list_step = 11;
        } else {
            save_screen_state.list_step = 10;
        }
    }
    if (save_screen_state.list_step == 0 && SaveCard_CheckBusy() == 0) {
        save_screen_state.list_step = 2;
        if (save_lines_reorder_pending != 0) {
            save_lines_reorder_pending = 0;
            same_count = 0;
            if (save_line_write_slots[0] == save_line_slots[0]) {
                same_count++;
            }
            if (save_line_write_slots[1] == save_line_slots[1]) {
                same_count++;
            }
            if (save_line_write_slots[2] == save_line_slots[2]) {
                same_count++;
            }
            for (i = 0; i < 3; i++) {
                if (i != save_screen_state.selected_save && save_line_write_slots[i] >= save_line_write_slots[save_screen_state.selected_save]) {
                    save_line_write_slots[i]++;
                }
            }
            if (save_line_slots[save_screen_state.selected_save] != 0) {
                if (same_count == 1) {
                    swapped = save_line_slots[save_line_write_slots[save_screen_state.selected_save]];
                    save_line_slots[save_line_write_slots[save_screen_state.selected_save]] = save_line_slots[save_screen_state.selected_save];
                    save_line_slots[save_screen_state.selected_save] = swapped;
                } else if (same_count == 2) {
                    if (save_line_slots[save_screen_state.selected_save] != 1) {
                        for (i = 0; i < 3; i++) {
                            if (save_line_slots[i] == 1) {
                                break;
                            }
                        }
                        line = i;
                        // match: the group variable as the second counter: retail keeps both in one register
                        for (group = 0; group < 3; group++) {
                            if (save_line_slots[group] == 2) {
                                break;
                            }
                        }
                        save_line_slots[group] = 1;
                        save_line_slots[line] = 2;
                    }
                }
            }
        }
    }

    report = Pad_GetReport(0, 0);
    if ((report->pressed & PAD_CROSS) && modal_pad_mode != 0xFF) {
        if (save_screen_state.list_step != 0) {
            report = Pad_GetReport(0, 0);
            if (report->held & (PAD_UP | PAD_DOWN)) {
                save_screen_state.confirm_press_state = 1;
            } else {
                save_screen_state.confirm_press_state = 3;
            }
        }
    }
    if (save_screen_state.confirm_press_state == 2) {
        save_screen_state.confirm_press_state = 3;
    }
    if (save_screen_state.confirm_press_state == 1) {
        report = Pad_GetReport(0, 0);
        if (report->held & (PAD_UP | PAD_DOWN)) {
            if (menu_buttons & (PAD_UP | PAD_DOWN)) {
                save_screen_state.confirm_press_state = 2;
            }
        } else {
            save_screen_state.confirm_press_state = 3;
        }
    }

    if (save_screen_state.list_step != 0 && save_screen_state.input_locked == 0) {
        if (save_screen_state.confirm_press_state == 3) {
            save_screen_state.confirm_press_state = 0;
            switch (save_screen_state.list_step) {
            case -1:
                Sound_PlayMenu(0);
                save_state = SaveCard_GetSaveState(save_line_write_slots[save_screen_state.selected_save]);
                if (save_state == 0) {
                    if (SaveCard_HasRoomFor(save_screen_state.selected_card, 1, 0) > 0 &&
                        MENU_SCREEN_SAVES(&save_screen_state)->save_state[save_screen_state.selected_save] != -1) {
                        save_lines_reorder_pending = 1;
                        save_screen_state.list_step = 6;
                        save_screen_state.free_clusters = SaveCard_GetFreeClusters(save_screen_state.selected_card);
                    } else {
                        save_screen_state.list_step = 20;
                        save_lines_reorder_pending = 0;
                    }
                } else if (save_state == 1) {
                    save_screen_state.list_step = 7;
                }
                break;
            case 6:
            case 7:
                if (save_screen_state.confirm_choice == 0) {
                    Sound_PlayMenu(0);
                    save_screen_state.list_step = 0;
                    PlayTime_Update();
                    Game_WriteSaveImage();
                    SaveCard_StartSave(save_screen_state.selected_card, save_line_write_slots[save_screen_state.selected_save], (s32 *)save_image);
                } else {
                    Sound_PlayMenu(2);
                    save_screen_state.list_step = -1;
                }
                save_screen_state.confirm_choice = 1;
                break;
            case 2:
            case 20:
                Sound_PlayMenu(0);
                save_screen_state.list_step = -1;
                break;
            case 10:
            case 11:
                Sound_PlayMenu(0);
                system_screen_state = 3;
                save_screen_state.input_locked = 1;
                break;
            }
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & PAD_CIRCLE) && modal_pad_mode != 0xFF && save_screen_state.confirm_press_state == 0) {
                Sound_PlayMenu(2);
                switch (save_screen_state.list_step) {
                case -1:
                    system_screen_state = 2;
                    save_screen_state.input_locked = 1;
                    break;
                case 10:
                case 11:
                    system_screen_state = 3;
                    save_screen_state.input_locked = 1;
                    break;
                case 6:
                case 7:
                    save_screen_state.confirm_choice = 1;
                case 2:
                case 20:
                    save_screen_state.list_step = -1;
                    break;
                }
            } else if (menu_buttons & PAD_DOWN) {
                switch (save_screen_state.list_step) {
                case -1:
                    for (i = 0; i < 3; i++) {
                        save_screen_state.selected_save = ((s8)(save_screen_state.selected_save + 1) + 3) % 3;
                        if (MENU_SCREEN_SAVES(&save_screen_state)->save_state[save_screen_state.selected_save] >= 0) {
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
                    save_screen_state.confirm_choice = (save_screen_state.confirm_choice + 1) & 1;
                    break;
                }
            } else if (menu_buttons & PAD_UP) {
                switch (save_screen_state.list_step) {
                case -1:
                    for (i = 0; i < 3; i++) {
                        save_screen_state.selected_save = ((s8)(save_screen_state.selected_save - 1) + 3) % 3;
                        if (MENU_SCREEN_SAVES(&save_screen_state)->save_state[save_screen_state.selected_save] >= 0) {
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
                    save_screen_state.confirm_choice = (save_screen_state.confirm_choice + 1) & 1;
                    break;
                }
            }
        }
    }

    switch (system_screen_state) {
    case 0:
        screen_part_alphas[0xFF] += 12.8f;
        if (screen_part_alphas[0xFF] > 128.0f) {
            screen_part_alphas[0xFF] = 128.0f;
        }
        menu_draw_color.alpha = screen_part_alphas[0xFF];
        for (i = 0; i < data_part_count; i++) {
            screen_part_alphas[i] += SPRITE_PACKET_COLOR_VIEW(&data_parts[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&data_parts[i])->alpha < screen_part_alphas[i]) {
                for (i = 0; i < data_part_count; i++) {
                    screen_part_alphas[i] = SPRITE_PACKET_COLOR_VIEW(&data_parts[i])->alpha;
                }
                system_screen_state = 1;
                save_screen_state.input_locked = 0;
                break;
            }
        }
        break;
    case 2:
    case 3:
        screen_part_alphas[0xFF] -= 12.8f;
        if (screen_part_alphas[0xFF] < 0.0f) {
            screen_part_alphas[0xFF] = 0.0f;
        }
        menu_draw_color.alpha = screen_part_alphas[0xFF];
        for (i = 0; i < data_part_count; i++) {
            screen_part_alphas[i] -= SPRITE_PACKET_COLOR_VIEW(&data_parts[i])->alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                pulse_sprite_alpha = 128.0f;
                save_screen_handler = SaveScreen_UpdateCardSelect;
                pulse_sprite_falling = 0;
                save_screen_state.selected_save = 1;
                save_screen_state.confirm_choice = 1;
                if (system_screen_state == 2) {
                    SaveCard_StartCheck(0);
                } else {
                    SaveCard_StartCheck(1);
                }
                system_screen_state = 0;
                break;
            }
        }
        break;
    }
}
