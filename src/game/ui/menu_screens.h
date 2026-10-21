/* The menu screens and the loader of their models (ui/menu_screens.c). */
#ifndef KF_MENU_SCREENS_H
#define KF_MENU_SCREENS_H

#include "common.h"
#include "ui/link.h"
#include "core/state.h"
#include "core/game.h"
#include "core/load.h"
#include "gfx/gs.h"
#include "gfx/packet.h"
#include "ui/menu.h"

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
void Menu_ResetLoadBuffers(s32 unused);

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
void Menu_ChainItemListModels(s32 unused);

/* The five load nodes follow a list cursor one row on or back, or a page up or down. */
void Menu_ShiftListModelsNext(s32 value);
void Menu_ShiftListModelsPrev(s32 value);
void Menu_ShiftListModelsPageUp(s32 value);
void Menu_ShiftListModelsPageDown(s32 value);

/* TRUE when the card check found a save on the card of either slot. */
s32 Menu_CardsHaveSaves(void);

/* ui/menu_screens.c */
int Menu_SetBlendMode(u8 mode);
int Menu_IsEquipSlotFilled(s32 value, u32 step);
void Menu_ClearEquipSlot(s8 slot);
void Menu_DrawBrightness(s32 level);
int Menu_IsEquipped(u32 slot, s32 kind, s32 value);

/* data still in asm */
extern MenuItem **equipped_item_refs[11];
extern MenuItem **equip_list_refs[11];
extern LinkNode *equip_model_loads[];
extern PoolPartSource *pool_part_sources[13];
extern f32 use_item_model_position[4];
extern f32 cast_spell_model_position[4];
extern f32 equip_item_model_position[4];
extern f32 equip_weapon_model_position[4];
extern f32 recollection_model_position[4];
extern f32 recollection_page_model_position[4];
extern f32 visible_item_model_position[4];
extern s8 button_choices[12];
extern s8 button_choices_shown[12];
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
extern MenuItem spell_entries[];
extern MenuItem shortcut_entries[];
extern f32 screen_part_alphas[0x100];
extern f32 panel_part_alphas[0x100];
extern MenuItem *item_list_cursor[4];
extern MenuItem *item_list_top[4];
extern MenuItem *item_list_page_mark[4];
extern GsDoubleBufferDc menu_frame_buffers;
extern u64 menu_first_draw_env[47];  /* the frame register of the first buffer ([0]) and of the second ([46]) */
extern char menu_first_draw_env2[];
extern u64 menu_second_draw_env[];  /* inside menu_frame_buffers: the second buffer's draw environments */
extern char menu_second_draw_env2[];
extern LinkNode model_load_shown;
extern char model_name_shown[];  /* the names of the five link nodes at model_load_shown */
extern LinkNode model_load_up;
extern char model_name_up[];
extern LinkNode model_load_down;
extern char model_name_down[];
extern LinkNode model_load_left;
extern char model_name_left[];
extern LinkNode model_load_right;
extern char model_name_right[];
extern u32 main_part_count;
extern u32 item_part_count;
extern u32 magic_part_count;
extern u32 equip_part_count;
extern u32 equip_a_part_count;
extern u32 equip_g_part_count;
extern u32 equip_m_part_count;
extern u32 item_sel_part_count;
extern u32 status_part_count;
extern u32 image_part_count;
extern u32 recol_part_count;
extern u32 system_part_count;
extern u32 slot_part_count;
extern u32 data_part_count;
extern u32 option_part_count;
extern u32 keylist_part_count;
extern u32 keyconf_part_count;
extern u32 preshop_part_count;
extern u32 shop_part_count;
extern u32 equip_g2_part_count;
extern u32 equip_s_part_count;
extern u32 recol_a_part_count;
extern u32 recol_g_part_count;
extern u32 recol_g2_part_count;
extern u32 shop_g2_part_count;
extern u32 shop_a_part_count;
extern u32 shop_g_part_count;
extern u32 pretune_part_count;  /* how many parts the screen has */
extern u32 tune_part_count;  /* how many parts the screen has */
extern u32 tune_s_part_count;  /* how many parts the screen has */
extern u32 mc_part_count;
extern u32 bright_part_count;
extern u32 shop_mess_part_count;
extern u32 tune_mess_part_count;
extern u32 mc_mess_part_count;
extern u32 equip_m2_part_count;
extern s32 menu_run_step;
extern s32 menu_step;
extern s32 equip_weapon_draw_index;
extern int model_read_file;
extern u8 equipped_list_open;
extern char sunday_name[8];
extern char monday_name[8];
extern char tuesday_name[8];
extern char wednesday_name[8];
extern char thursday_name[8];
extern char friday_name[8];
extern char saturday_name[8];
extern ColorBytes backdrop_shade_color[];
extern void *menu_screen_handler;
extern void *save_list_opener;
extern char equip_screen_name[8];
extern u8 menu_blink_step;
extern f32 use_item_model_angle;  /* the shown item model's turn about y */
extern s32 use_item_frames;
extern s8 detail_panel_sound_pending;
extern f32 equip_item_model_angle;
extern f32 equip_weapon_model_angle;
extern u32 equip_weapon_motion_frame;
extern char group_name_weapons[8];
extern char group_name_magic[8];
extern char group_name_shield[8];
extern char group_name_helm[8];
extern char group_name_armor[8];
extern char group_name_gauntlet[8];
extern char group_name_boot[8];
extern char group_name_ring[8];
extern char group_name_bracelet[8];
extern char group_name_amulet[8];
extern char group_name_item[8];
extern s32 key_item_row;  /* the row of three the list starts at, 0 to 2 */
extern s32 key_items_frames;
extern s32 recollection_page;  /* the page shown, 0 to 49 */
extern s32 recollection_page_before;  /* the page shown the frame before */
extern f32 visible_item_model_angle;  /* the turning model's angle */
extern s32 quit_choice;
extern s8 card_slot_leave_steps;
extern s8 save_list_leave_steps;
extern s32 sound_mode_before;  /* option 0 as it was before it was first toggled */
extern s8 sound_mode_change;  /* 0: option 0 untouched, 1: toggled and not applied yet, 2 to 4: applied */
extern s8 options_leave_steps;  /* counts the two steps of leaving when the screen is a start-up step */
extern s8 buttons_leave_steps;
extern s32 button_line;
extern s32 button_prompt_choice;
extern s8 button_prompt;
extern SpritePacket *slot_parts;
extern SpritePacket *data_parts;
extern SpritePacket *preshop_parts;
extern SpritePacket *shop_parts;
extern s8 pulse_value_falling;
extern SpritePacket *shop_g2_parts;
extern SpritePacket *shop_a_parts;
extern SpritePacket *shop_g_parts;
/* the menu's ring of everything owned */
extern MenuItem *owned_list_first;
extern MenuItem *owned_list_last_page;
extern s32 owned_list_count;
extern MenuItem *item_list_first;
extern MenuItem *item_list_last_page;
extern s32 item_list_count;
extern SpritePacket *pretune_parts;
extern SpritePacket *tune_parts;
extern SpritePacket *tune_s_parts;
extern SpritePacket *mc_parts;
extern SpritePacket *bright_parts;
extern SpritePacket *shop_mess_parts;
extern SpritePacket *tune_mess_parts;
extern SpritePacket *mc_mess_parts;
extern SpritePacket *equip_m2_parts;
extern s8 system_screen_state;
/* system_screen_state: the first two states every screen that uses it shares */
#define SYSTEM_SCREEN_STATE_FADE_IN     0
#define SYSTEM_SCREEN_STATE_OPEN        1
extern ColorBytes menu_draw_color;
extern ColorBytes panel_text_color;
extern ColorBytes model_draw_color;
extern LinkNode *model_load_chain;
extern s8 menu_mode;
extern s32 menu_frame_ticks;
extern s32 menu_even_field;
extern s32 pad_mode_change_frames;
extern u8 menu_l2_cycle_step;
extern MenuItem *visible_bar_top;
extern MenuItem *visible_bar_cursor;
extern MenuItem *visible_names_top;
extern MenuItem *visible_names_cursor;
extern MenuItem *visible_text_item;
extern u8 card_save_mask;
extern f32 brightness_slider;
extern SpritePacket *main_parts;
extern SpritePacket *item_parts;
extern SpritePacket *magic_parts;
extern SpritePacket *equip_parts;
extern SpritePacket *equip_a_parts;
extern SpritePacket *equip_g_parts;
extern SpritePacket *equip_m_parts;
extern SpritePacket *item_sel_parts;
extern SpritePacket *status_parts;
extern SpritePacket *image_parts;
extern SpritePacket *recol_parts;
extern SpritePacket *system_parts;
extern SpritePacket *option_parts;
extern SpritePacket *keylist_parts;
extern SpritePacket *keyconf_parts;
extern SpritePacket *equip_g2_parts;
extern SpritePacket *equip_s_parts;
extern SpritePacket *recol_a_parts;
extern SpritePacket *recol_g_parts;
extern SpritePacket *recol_g2_parts;
extern ChunkPoolNode *free_load_buffers;
extern MenuFreeNode equip_weapon_pool_node;
extern s8 main_menu_state;  /* 0: fading in, 1: open, 2: fading out to the chosen screen, 3: fading out to leave */
extern s8 use_item_state;  /* 0: fading in, 1: open, 2 to 4: waiting for the models, 5 to 7: fading out */
extern s8 equip_state;
extern s8 cast_spell_state;  /* 0: fading in, 1: open, 2: waiting for the models, 3: fading out */
extern s8 key_items_state;  /* 0: fading in, 1: open, 2: fading out to Menu_UpdateKeyItems, 3: fading out to leave */
extern s8 recollection_state;  /* 0: fading in, 1: open, 2: fading out */
extern s8 visible_item_state;  /* 0: fading in, 1: open, 2: waiting for the loads, 3: fading out */
extern s8 visible_panel_state;  /* the detail panel's: 0: fading in, 1: open, 2: fading out */
extern s8 main_menu_line;
extern char equip_model_shown;
extern u8 brightness_before;
extern int pad_mode;
extern int pad_mode_before;
extern s8 recollection_cursor;
extern s8 recollection_top;  /* the counter at the top of the list */
extern s8 menu_input_enabled;
extern s32 equip_group_shown;
extern s32 system_menu_line;
extern int worn_model_loading;
extern char menu_step_context[];
extern MenuItemList equipped_shortcut;
extern MenuItemList equipped_weapon;
extern MenuItemList equipped_shield;
extern MenuItemList equipped_helm;
extern MenuItemList equipped_armor;
extern MenuItemList equipped_gauntlet;
extern MenuItemList equipped_boot;
extern MenuItemList equipped_ring;
extern MenuItemList equipped_bracelet;
extern MenuItemList equipped_amulet;
extern MenuItemList equipped_spell;
extern MenuItemList use_item_list;
extern MenuItemList equip_weapon_list;
extern MenuItemList equip_shield_list;
extern MenuItemList equip_helm_list;
extern MenuItemList equip_armor_list;
extern MenuItemList equip_gauntlet_list;
extern MenuItemList equip_boot_list;
extern MenuItemList equip_ring_list;
extern MenuItemList equip_bracelet_list;
extern MenuItemList equip_amulet_list;
extern MenuItemList equip_spell_list;
extern MenuItemList equip_shortcut_list;
extern MenuItem *castable_spell_first[4];
extern MenuItemList *equip_list_shown[];
extern MenuItem *owned_list_current[];
extern MenuItem *owned_list_top[];
extern MenuItem *owned_list_page_mark[];
extern MenuItem *castable_spell_cursor[];
extern MenuItem *castable_spell_top[];
extern MenuItem *castable_spell_page_mark[];
extern LinkNode model_load_spare;
extern ChunkPoolNode load_buffer_nodes[];
extern LinkNode weapon_model_load;
extern char weapon_model_name[];
extern LinkNode shield_model_load;
extern char shield_model_name[];
extern LinkNode armor_model_load;
extern char armor_model_name[];
extern LinkNode helm_model_load;
extern char helm_model_name[];
extern LinkNode gauntlet_model_load;
extern char gauntlet_model_name[];
extern LinkNode boot_model_load;
extern char boot_model_name[];
extern LinkNode ring_model_load;
extern char ring_model_name[];
extern LinkNode bracelet_model_load;
extern char bracelet_model_name[];
extern LinkNode amulet_model_load;
extern char amulet_model_name[];
extern LinkNode worn_model_loader;
extern MenuModelMotion *equip_weapon_motions[];
extern ChunkUpload npc_package_load;
extern LinkNode key_item_model_loads[12];
extern MenuScreenState card_screen_state;
extern u16 button_settings_copy[13];
extern s8 equip_panel_state;

/* data still in asm */
extern u8 detail_panel_open;
extern s32 detail_panel_open_word;  /* detail_panel_open as ui/trade.c reads and writes it, a word (an alias there) */

#endif
