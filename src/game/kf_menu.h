/* Menus: the doubly linked item lists the cursor walks (D_005CE880...). */
#ifndef KF_MENU_H
#define KF_MENU_H

#include "common.h"
#include "kf_gs.h"

/* the colour the menus draw with (D_00446C78), eight bytes passed by value to MenuText_SetColor and MenuModel_DrawTinted */
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
    int unk00;                      /* 0x00 */
    struct MenuItem *next;          /* 0x04 */
    struct MenuItem *prev;          /* 0x08 */
    s16 unk0C;                      /* 0x0C the number in its model's file name */
    s8 group;                       /* 0x0E which run of LevelDefs it is in (0: 150 on, 1: the first 150, 2 to 9: 220 on), 0 to 12 */
} MenuItem;

/* a node of the list D_005CE840[0] points into: each one has its own ring of items */
typedef struct MenuCategory {
    MenuItem *item;                 /* 0x00 the item the cursor is on */
    struct MenuCategory *next;      /* 0x04 */
    struct MenuCategory *prev;      /* 0x08 */
} MenuCategory;

/* one of the menu's item lists, a MenuCategory that knows its group: the twelve at D_005CE760, linked in
   a ring by Menu_Init, and the eleven at D_005CE6A0 that their items are first taken from */
typedef struct MenuItemList {
    MenuItem *item;                 /* 0x00 the item the cursor is on */
    struct MenuItemList *next;      /* 0x04 */
    struct MenuItemList *prev;      /* 0x08 */
    char pad0C[0x02];
    s8 group;                       /* 0x0E MenuItem.group of its items */
    char pad0F[0x01];
} MenuItemList;

/* D_0043F780: per item group, the function that writes the item's model file name */
typedef struct {
    void (*handler[12])(char *buffer, MenuItem *item);
} MenuNameHandlers;

/* a node of the menu free list D_004472B0 (15 of them at D_005CE900) */
typedef struct MenuFreeNode {
    struct MenuFreeNode *next;      /* 0x00 */
    int unk04;                      /* 0x04 */
} MenuFreeNode;

/* what Menu_FreeLoadBuffer is handed: the node to give back at 0x0C */
typedef struct {
    char pad00[0x0C];
    MenuFreeNode *node;             /* 0x0C */
} MenuFreeRequest;

/* a node of the list *D_005CE840[0] points into (Menu_MoveEquipCursor follows next/prev) */
typedef struct DataNode {
    int unk00;                      /* 0x00 */
    struct DataNode *next;          /* 0x04 */
    struct DataNode *prev;          /* 0x08 */
    s16 unk0C;                      /* 0x0C table row, plus one */
} DataNode;

/* one of the three 0x180-byte records SaveCard_GetSaveInfo hands out (SaveInfo in kf_save_card.h), as the save list of
   Menu_UpdateSaveList reads it: what a save on the card says about itself */
typedef struct {
    char pad00[0x04];
    s32 unk04;                      /* 0x04 which line of the text table names it */
    s32 unk08;                      /* 0x08 unk08 to unk28: numbers put into the text (D_00446DF4...) */
    s32 unk0C;                      /* 0x0C */
    s32 unk10;                      /* 0x10 */
    s32 unk14;                      /* 0x14 */
    s32 unk18;                      /* 0x18 */
    s32 unk1C;                      /* 0x1C */
    s32 unk20;                      /* 0x20 */
    s32 unk24;                      /* 0x24 */
    s32 unk28;                      /* 0x28 */
    char pad2C[0x14D];
    u8 unk179;                      /* 0x179 unk179 to unk17B: three more numbers for the text */
    u8 unk17A;                      /* 0x17A */
    u8 unk17B;                      /* 0x17B */
    char pad17C[0x04];
} CardSaveInfo;

/* MenuScreenState's three bytes at 0x1E as the array Menu_UpdateSaveList fills: what SaveCard_GetSaveState says of each save
   (1: there is one), then 0 or -1 for the others by what SaveCard_HasRoomFor answers */
typedef struct {
    char pad00[0x1E];
    s8 save_state[3];               /* 0x1E */
} MenuScreenSaves;

#define MENU_SCREEN_SAVES(state) ((MenuScreenSaves *)(state))

/* what the screen handler in D_00446B20 is handed */
typedef struct {
    s32 unk00;                      /* 0x00 */
    s32 unk04;                      /* 0x04 cleared when a screen is left */
} MenuScreenContext;

/* D_005CF2F0: state of the menu screen Menu_UpdateCardSlot runs; Menu_OpenCardSlot resets it */
typedef struct {
    s32 unk00;                      /* 0x00 */
    s32 unk04;                      /* 0x04 */
    char pad08[0x04];
    s8 unk0C;                       /* 0x0C */
    s8 unk0D;                       /* 0x0D */
    s8 unk0E;                       /* 0x0E starts as 1 */
    char pad0F[0x01];
    s8 unk10;                       /* 0x10 */
    s8 unk11;                       /* 0x11 */
    s8 unk12;                       /* 0x12 starts as -1 */
    s8 unk13;                       /* 0x13 step of the screen Menu_UpdateSaveList runs, -1: waiting for input */
    s8 unk14;                       /* 0x14 */
    char pad15[0x01];
    s8 unk16;                       /* 0x16 */
    s8 unk17;                       /* 0x17 */
    s8 unk18;                       /* 0x18 starts as 1 */
    s8 unk19;                       /* 0x19 what SaveCard_GetSavesState said of the first slot's card, unk1A of the second's (Menu_CardsHaveSaves) */
    s8 unk1A;                       /* 0x1A */
    s8 unk1B;                       /* 0x1B */
    s8 unk1C;                       /* 0x1C */
    s8 unk1D;                       /* 0x1D */
    s8 unk1E;                       /* 0x1E */
    s8 unk1F;                       /* 0x1F */
    s8 unk20;                       /* 0x20 */
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

/* D_00440230: the pad button bit of each of the twelve choices of the button screen; Menu_UpdateButtonConfig copies it whole */
typedef struct {
    u16 bits[12];                   /* 0x00 */
} MenuPadBitTable;

/* trade_stock_tables: per trade table, how many of each item it has in stock (D_003699B0...); copied whole like TradeValueTables */
typedef struct {
    u8 *counts[5];                  /* 0x00 */
} TradeStockTables;

/* item_group_bases: per MenuItem.group, the LevelDef number its records start at; copied whole like TradeValueTables */
typedef struct {
    s32 first_item[13];             /* 0x00 */
} TradeGroupBases;

/* D_0043FC08: the seven weekday names (D_00446AE0...); Menu_Run copies the table whole and never reads it */
typedef struct {
    char *names[7];                 /* 0x00 */
} MenuWeekdayNames;

/* trade_value_slots: where the values of the seven listed items go (D_00446E64...); copied whole like TradeValueTables */
typedef struct {
    s32 *shown_value[7];            /* 0x00 */
} TradeValueSlots;

/* D_0043FD28: per MenuItem.group, the LevelDef number its records start at, as the equipment screen
   has it (twelve groups); copied whole to a local */
typedef struct {
    s32 first_item[12];             /* 0x00 */
} EquipGroupBases;

/* D_0043FD58: twelve pointers the equipment screen copies whole to a local and never reads */
typedef struct {
    void *unk00[12];                /* 0x00 */
} EquipUnusedTable;

/* D_0043FDC0, D_0043FDD0: one byte per MenuItem.group (the first holds the glyph that names the group);
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

/* D_00446D58: how much of a font texture one glyph cell covers across and down (1/25.6 each); the sprite
   drawers MenuDraw_Word and MenuDraw_Number copy it whole to a local */
typedef struct {
    f32 step[2];
} MenuGlyphCell;

/* what a D_005CEE60 entry points to: a motion of the weapon model on the equipment screen */
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

/* D_0043F7C8: the LevelDef numbers of the twelve key items (Menu_AddKeyItem maps them to 0..11); copied whole to a local */
typedef struct {
    s32 number[12];                 /* 0x00 */
} KeyItemNumbers;

/* D_0043FE48, D_0043FE60: the GS texture and CLUT base pointers of the six key item models on screen; copied whole */
typedef struct {
    u32 base[6];                    /* 0x00 */
} KeyItemTexturePages;

/* D_0043FE80: where the six key item models go; copied whole */
typedef struct {
    f32 position[6][4] __attribute__((aligned(16))); /* 0x00 */
} KeyItemPositions;

/* D_0043FEE0: where the six key item names go, as MenuText_SetPen takes it; copied whole */
typedef struct {
    GsPackedWord word[6];           /* 0x00 */
} KeyItemNameWords;

#endif
