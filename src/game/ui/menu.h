/* Menus: the doubly linked item lists the cursor walks (castable_spell_cursor...). */
#ifndef KF_MENU_H
#define KF_MENU_H

#include "common.h"
#include "gfx/gs.h"

/* the colour the menus draw with (menu_draw_color), eight bytes passed by value to MenuText_SetColor and MenuModel_DrawTinted */
typedef struct {
    u8 red;                         /* 0x00 */
    u8 green;                       /* 0x01 */
    u8 blue;                        /* 0x02 */
    u8 alpha;                       /* 0x03 */
    char pad04[0x04];
} MenuColor;

/* the flat rectangle MenuDraw_Bar draws: a SpritePacket whose corners are filled as GsPackedWord */
typedef struct {
    GsPackedWord first;             /* 0x00 x, y and z of the top left corner */
    char pad08[0x08];
    GsPackedWord second;            /* 0x10 the bottom right corner */
    char pad18[0x08];
    u8 red;                         /* 0x20 */
    u8 green;                       /* 0x21 */
    u8 blue;                        /* 0x22 */
    u8 alpha;                       /* 0x23 */
    char pad24[0x3C];
} MenuCursorBar;

typedef struct MenuItem {
    int definition;                 /* 0x00 the item's definition: an item, weapon, worn or spell (fx_table) entry */
    struct MenuItem *next;          /* 0x04 */
    struct MenuItem *prev;          /* 0x08 */
    s16 item_number;                /* 0x0C the item's number: its index in item_name_defs and item_counts */
    s8 group;                       /* 0x0E which run of LevelDefs it is in (0: 150 on, 1: the first 150, 2 to 9: 220 on), 0 to 12 */
} MenuItem;

/* MenuItem.group and MenuItemList.group: what kind of item it is (Menu_Init) */
#define MENU_ITEM_GROUP_WEAPON      0
#define MENU_ITEM_GROUP_SPELL       1       /* an fx_table entry */
#define MENU_ITEM_GROUP_SHIELD      2
#define MENU_ITEM_GROUP_HELM        3
#define MENU_ITEM_GROUP_ARMOR       4
#define MENU_ITEM_GROUP_GAUNTLET    5
#define MENU_ITEM_GROUP_BOOT        6
#define MENU_ITEM_GROUP_RING        7
#define MENU_ITEM_GROUP_BRACELET    8
#define MENU_ITEM_GROUP_AMULET      9
#define MENU_ITEM_GROUP_SHORTCUT    10      /* the equipped shortcut list */
#define MENU_ITEM_GROUP_ITEM        11      /* a common item, in the use and shortcut lists */
#define MENU_ITEM_GROUP_STOCK_ITEM  12      /* a common item in all_item_entries, the shop and smith lists */

/* a node of the list equip_list_shown[0] points into: each one has its own ring of items */
typedef struct MenuCategory {
    MenuItem *item;                 /* 0x00 the item the cursor is on */
    struct MenuCategory *next;      /* 0x04 */
    struct MenuCategory *prev;      /* 0x08 */
} MenuCategory;

/* one of the menu's item lists, a MenuCategory that knows its group: the twelve at use_item_list, linked in
   a ring by Menu_Init, and the eleven at equipped_shortcut that their items are first taken from */
typedef struct MenuItemList {
    MenuItem *item;                 /* 0x00 the item the cursor is on */
    struct MenuItemList *next;      /* 0x04 */
    struct MenuItemList *prev;      /* 0x08 */
    char pad0C[0x02];
    s8 group;                       /* 0x0E MenuItem.group of its items */
    char pad0F[0x01];
} MenuItemList;

/* item_model_name_writers: per item group, the function that writes the item's model file name */
typedef struct {
    void (*handler[12])(char *buffer, MenuItem *item);
} MenuNameHandlers;

/* a node of the menu free list free_load_buffers (15 of them at load_buffer_nodes) */
typedef struct MenuFreeNode {
    struct MenuFreeNode *next;      /* 0x00 */
    int pad04;                      /* 0x04 */
} MenuFreeNode;

/* what Menu_FreeLoadBuffer is handed: the node to give back at 0x0C */
typedef struct {
    char pad00[0x0C];
    MenuFreeNode *node;             /* 0x0C */
} MenuFreeRequest;

/* a node of the list *equip_list_shown[0] points into (Menu_MoveEquipCursor follows next/prev) */
typedef struct DataNode {
    int pad00;                      /* 0x00 */
    struct DataNode *next;          /* 0x04 */
    struct DataNode *prev;          /* 0x08 */
    s16 table_row;                  /* 0x0C row of the item's text table, plus one */
} DataNode;

/* one of the three 0x180-byte records SaveCard_GetSaveInfo hands out (SaveInfo in save/save_card.h), as the save list of
   Menu_UpdateSaveList reads it: what a save on the card says about itself */
typedef struct {
    char pad00[0x04];
    s32 save_area;                  /* 0x04 GameStatus.save_area: the message id of its area name, from 400 */
    s32 experience;                 /* 0x08 experience, to gold: numbers put into the text (menu_number_44...) */
    s32 level;                      /* 0x0C the player's level */
    s32 hp_now;                     /* 0x10 the HP the player had */
    s32 hp_max;                     /* 0x14 the most HP the player could have */
    s32 mp_now;                     /* 0x18 the MP the player had */
    s32 mp_max;                     /* 0x1C the most MP the player could have */
    s32 power_base;                 /* 0x20 the player's power base */
    s32 magic_base;                 /* 0x24 the player's magic base */
    s32 gold;                       /* 0x28 the gold the player had */
    char pad2C[0x14D];
    u8 play_seconds;                /* 0x179 play_seconds, to play_hours: three more numbers for the text */
    u8 play_minutes;                /* 0x17A the play time's minutes */
    u8 play_hours;                  /* 0x17B the play time's hours */
    char pad17C[0x04];
} CardSaveInfo;

/* MenuScreenState's three bytes at 0x1E as the array Menu_UpdateSaveList fills: what SaveCard_GetSaveState says of each save
   (1: there is one), then 0 or -1 for the others by what SaveCard_HasRoomFor answers */
typedef struct {
    char pad00[0x1E];
    s8 save_state[3];               /* 0x1E */
} MenuScreenSaves;

#define MENU_SCREEN_SAVES(state) ((MenuScreenSaves *)(state))

/* what the screen handler in menu_screen_handler is handed */
typedef struct {
    s32 pad00;                      /* 0x00 */
    s32 selection;                  /* 0x04 the selected line or text number of the screen; cleared when a screen is left */
} MenuScreenContext;

/* the bits of one card slot's four in card_screen_state.card_problem_codes */
#define CARD_SLOT_USABLE 0x1
#define CARD_SLOT_FORMAT 0x2
#define CARD_SLOT_PROBLEM 0x4

/* card_screen_state: state of the menu screen Menu_UpdateCardSlot runs; Menu_OpenCardSlot resets it */
typedef struct {
    s32 card_update_result;         /* 0x00 SaveCard_Update's result: negative marks a card failure */
    s32 card_problem_codes;         /* 0x04 four bits per card slot: 1 usable, 2 format it, 4 another problem */
    char pad08[0x04];
    s8 selected_card;               /* 0x0C the card the cursor is on (0 or 1) */
    s8 selected_save;               /* 0x0D the save line the cursor is on (0 to 2) */
    s8 confirm_choice;              /* 0x0E starts as 1 */
    char pad0F[0x01];
    s8 card_saves_found;            /* 0x10 1: the card's save list was read and has saves */
    s8 is_leaving;                  /* 0x11 1: the screen is being left (the cards were closed) */
    s8 dialog_id;                   /* 0x12 starts as -1 */
    s8 list_step;                   /* 0x13 step of the screen Menu_UpdateSaveList runs, -1: waiting for input */
    s8 card_op_running;             /* 0x14 1 while a card operation the screen started runs */
    char pad15[0x01];
    s8 save_loaded;                 /* 0x16 1: a save was loaded, Menu_Run gives its caller 1 for it */
    s8 check_step;                  /* 0x17 the step of the card check screen (0 to 4) */
    s8 input_locked;                /* 0x18 starts as 1 */
    s8 card0_saves_state;           /* 0x19 what SaveCard_GetSavesState said of the first slot's card, card1_saves_state of the second's (Menu_CardsHaveSaves) */
    s8 card1_saves_state;           /* 0x1A what SaveCard_GetSavesState said of the second card */
    s8 check_finished;              /* 0x1B 0 until the first card check has finished */
    s8 skip_drawing;                /* 0x1C 1: the screen draws nothing and leaves at once */
    s8 confirm_press_state;         /* 0x1D how the confirm press is stepped: 0 idle, 3 fires, 1 and 2 wait */
    s8 save_state_0;                /* 0x1E what SaveCard_GetSaveState says of save 0 (1: there is one) */
    s8 save_state_1;                /* 0x1F what SaveCard_GetSaveState says of save 1 */
    s8 save_state_2;                /* 0x20 what SaveCard_GetSaveState says of save 2 */
} MenuScreenState;

/* one item's two values in a trade table (0x17C entries each, the first at 0x00426650); looks like a shop's prices */
typedef struct {
    u16 take_value;                 /* 0x00 0: the item cannot be taken from the stock list */
    u16 give_value;                 /* 0x02 0: the item cannot be given from what is owned */
} TradeValue;

/* trade_value_tables: the trade tables, a local table Trade_BuildItemRings copies whole from read-only data */
typedef struct {
    TradeValue *values[6];          /* 0x00 */
} TradeValueTables;

/* pad_bit_table: the pad button bit of each of the twelve choices of the button screen; Menu_UpdateButtonConfig copies it whole */
typedef struct {
    u16 bits[12];                   /* 0x00 */
} MenuPadBitTable;

/* trade_stock_tables: per trade table, how many of each item it has in stock (trade_stock_1...); copied whole like TradeValueTables */
typedef struct {
    u8 *counts[5];                  /* 0x00 */
} TradeStockTables;

/* item_group_bases: per MenuItem.group, the LevelDef number its records start at; copied whole like TradeValueTables */
typedef struct {
    s32 first_item[13];             /* 0x00 */
} TradeGroupBases;

/* weekday_names_table: the seven weekday names (sunday_name...); Menu_Run copies the table whole and never reads it */
typedef struct {
    char *names[7];                 /* 0x00 */
} MenuWeekdayNames;

/* trade_value_slots: where the values of the seven listed items go (menu_number_87...); copied whole like TradeValueTables */
typedef struct {
    s32 *shown_value[7];            /* 0x00 */
} TradeValueSlots;

/* equip_group_bases: per MenuItem.group, the LevelDef number its records start at, as the equipment screen
   has it (twelve groups); copied whole to a local */
typedef struct {
    s32 first_item[12];             /* 0x00 */
} EquipGroupBases;

/* equip_group_names: twelve pointers the equipment screen copies whole to a local and never reads */
typedef struct {
    void *unk00[12];                /* 0x00 */
} EquipUnusedTable;

/* equip_group_glyphs, equip_group_slot_bytes: one byte per MenuItem.group (the first holds the glyph that names the group);
   copied whole to a local */
typedef struct {
    s8 per_group[11];               /* 0x00 */
} EquipGroupBytes;

/* the glyph sprite MenuDraw_Word draws: a KindObject of kind 1 */
typedef struct {
    GsPackedWord first;             /* 0x00 x, y and z of the top left corner */
    char pad08[0x08];
    GsPackedWord second;            /* 0x10 the bottom right corner */
    char pad18[0x08];
    u8 red;                         /* 0x20 */
    u8 green;                       /* 0x21 */
    u8 blue;                        /* 0x22 */
    u8 alpha;                       /* 0x23 */
    char pad24[0x0D];
    u8 kind;                        /* 0x31 */
    s8 glyph;                       /* 0x32 */
    char pad33[0x2D];
} MenuGlyphSprite;

/* how much of a font texture one glyph cell covers across and down (1/25.6 each); MenuDraw_Word and
   MenuDraw_Number start a local array `cell` with these two floats (the 8 bytes at 0x446D58) */
typedef struct {
    f32 step[2];
} MenuGlyphCell;

/* what an equip_weapon_motions entry points to: a motion of the weapon model on the equipment screen */
typedef struct {
    char pad00[0x0C];
    u32 frame_count;                /* 0x0C */
    char frames[1];                 /* 0x10 handed to PoolNode_Pose */
} MenuModelMotion;

/* a 4x4 matrix inside a union: a store through it may alias any later load, so it stays ahead of one
   (Trade_UpdateOwnedList only matches with its scale matrix filled this way) */
typedef union {
    f32 rows[4][4];
    s32 words[16];
} MenuMatrix;

/* key_item_numbers: the LevelDef numbers of the twelve key items (Menu_AddKeyItem maps them to 0..11); copied whole to a local */
typedef struct {
    s32 number[12];                 /* 0x00 */
} KeyItemNumbers;

/* key_item_texture_pages, key_item_clut_pages: the GS texture and CLUT base pointers of the six key item models on screen; copied whole */
typedef struct {
    u32 base[6];                    /* 0x00 */
} KeyItemTexturePages;

/* key_item_positions: where the six key item models go; copied whole */
typedef struct {
    f32 position[6][4] __attribute__((aligned(16))); /* 0x00 */
} KeyItemPositions;

/* key_item_name_pens: where the six key item names go, as MenuText_SetPen takes it; copied whole */
typedef struct {
    GsPackedWord word[6];           /* 0x00 */
} KeyItemNameWords;

#define CM_TO_FEET 0.03281f /* feet per centimetre, as retail rounds it */

#endif
