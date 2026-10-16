/* The menu screens and the loader of their models (ui/menu_screens.c). */
#ifndef KF_MENU_SCREENS_H
#define KF_MENU_SCREENS_H

#include "common.h"
#include "ui/link.h"
#include "core/state.h"

/* Sets the menus up once: the frame buffers, the part files, the menu and font textures, the weapon motions, and
   every list of menu items. */
void Menu_Init(void);

/* Runs a menu from opening to closing. mode 0: the game's menu, 1: the memory card screens, 2: the options,
   3: the buttons, 4: the card check, 5: the brightness. With mode 1 the result is 1 when the save list set
   MenuScreenState.save_loaded (a save was loaded, by the look of the callers); otherwise 0. */
int Menu_Run(s8 mode);

/* Ends a menu frame: brightness, vertical blank, buffer swap and (when asked) the backdrop of the next frame.
   Returns whether the field just shown was the even one. */
s32 Menu_EndFrame(s32 run_handler);

/* Draws the model of the first load node (model_load_shown) with the matrix given. Returns nothing. */
int Menu_DrawItemModel(f32 *matrix); // match: callers use an int result

/* Runs the callback of the load node at the head of the chain, or goes on to the next node. */
void Menu_StepModelLoads(int unused);

/* The callback of a menu model's load node: runs the stage the node is at, until it is done. */
void Menu_StepModelLoad(LinkNode *node);

/* Gives a load node a buffer from the free list, if one is left (declared in ui/link.h too). Returns nothing. */
int Menu_TakeLoadBuffer(LinkNode *node);

/* Puts every load buffer back on the free list, keeping a load still in progress. */
void Menu_ResetLoadBuffers(void);

/* Allocates the load buffers, builds their free list and sets up the package load node. */
void Menu_InitLoadBuffers(void);

/* Empties the twelve key item slots. */
void Menu_ClearKeyItems(void);

/* Puts the key item that goes with LevelDef number `key` into the first empty key item slot. */
void Menu_AddKeyItem(s32 key);

/* Moves the cursor of the item list (item_list_cursor) a row or a page, unless the list is locked. Returns TRUE when a
   move button was pressed. */
s32 Menu_MoveItemListCursor(HandlerContext *context, s32 locked);

/* If the item list has anything: names its five models and chains the five load nodes for them. */
void Menu_ChainItemListModels(void);

/* The five load nodes follow a list cursor one row on or back, or a page up or down. */
void Menu_ShiftListModelsNext(s32 value);
void Menu_ShiftListModelsPrev(s32 value);
void Menu_ShiftListModelsPageUp(s32 value);
void Menu_ShiftListModelsPageDown(s32 value);

/* TRUE when the card check found a save on the card of either slot. */
s32 Menu_CardsHaveSaves(void);

#endif
