/* The menu screens: the game's menu (Use Item, Cast a Spell, Equip, Status Viewer, Recollection, Visible Item,
   System Config; the names are the game's own, from a table nothing reads) with the memory card, options, button
   and brightness screens, which Menu_Run also runs on their own. The file starts with the rings of items the lists
   walk and with the loader of the models the screens show: load nodes chained at model_load_chain, each loading one
   file in stages. Every screen is a handler in menu_screen_handler that is called once a frame. */
#include "ui/menu_screens.h"
#include "ui/menu.h"
#include "map/map.h"
#include "player/player.h"
#include "effect/effect.h"
#include "ui/link.h"
#include "core/load.h"
#include "core/file.h"
#include "core/game.h"
#include "common.h"
#include "gfx/texture.h"
#include "gfx/packet.h"
#include "input/pad.h"
#include <eeregs.h>
#include "gfx/gs.h"
#include "ui/message.h"
#include "core/state.h"
#include "gfx/screen.h"
#include "save/save_card.h"
#include "item/item.h"

extern LevelRecord weapon_level_records[70];
extern LevelRecord worn_item_level_records[160];
extern u8 spell_levels[6][5];
extern u8 item_counts[380];
extern FxTableEntry fx_table[];
extern PlayerState player_state;
extern LevelDef item_name_defs[380];
extern ItemCommonDef item_common_defs[];
extern MenuItem **equip_list_refs[11];
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
extern MenuItem *item_list_cursor[4];
extern MenuItem *item_list_top[4];
extern MenuItem *item_list_page_mark[4];
extern MenuItem *owned_list_first;
extern MenuItem *owned_list_last_page;
extern s32 owned_list_count;
extern MenuItem *item_list_first;
extern MenuItem *item_list_last_page;
extern s32 item_list_count;
extern s32 equip_group_shown;
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
extern MenuItem *owned_list_top[];
extern MenuItem *owned_list_current[];
extern MenuItem *owned_list_page_mark[];
extern MenuItem *castable_spell_cursor[];
extern MenuItem *castable_spell_top[];
extern MenuItem *castable_spell_page_mark[];
/* Appends node to the ring being built. */
#define LINK_ITEM(node)                     \
    if (last != NULL) {                     \
        (node)->next = last;                \
        last->prev = (node);                \
    } else {                                \
        first = (node);                     \
    }                                       \
    last = (node);
/* The same, counting: the seventh one becomes the page mark. */
#define LINK_COUNTED_ITEM(node, page_mark)  \
    count++;                                \
    if (count == 7) {                       \
        (page_mark)[0] = (node);            \
    }                                       \
    LINK_ITEM(node)
/* Rings the owned, usable items of one equipment group whose record is not in use, and puts the list on the one worn.
   The ring is closed without a test for an empty one. */
#define LINK_GROUP(nodes, records, record_base, item_base, item_count, list, start_list, worn)            \
    last = NULL;                                                                                         \
    first = NULL;                                                                                        \
    for (i = 0; i < (item_count); i++) {                                                                 \
        if (item_counts[(item_base) + i] != 0 && (item_name_defs[(item_base) + i].state & 1) &&               \
            (records)[(record_base) + i].in_smith_rings == 0) {                                                   \
            LINK_ITEM(&(nodes)[i])                                                                       \
        }                                                                                                \
    }                                                                                                    \
    first->next = last;                                                                                  \
    last->prev = first;                                                                                  \
    (list).item = (start_list).item = &(nodes)[(worn) - (record_base)];
/* Whether the spell is the one its slot of spell_levels holds (three spells to a slot, numbered from 1). */
#define SPELL_IS_SET(spell) (spell_levels[((spell) - 1) / 15][(((spell) - 1) / 3) % 5] == ((spell) - 1) % 3 + 1)
extern LinkNode *model_load_chain;
extern LinkNode model_load_spare;
/* the loads of the model shown and of the four the cursor brings on with up (0x10), down (0x40), left (0x80) and
   right (0x20): in a list a row and a page either way, on the equipment screen the next list and the next item */
extern LinkNode model_load_shown;
extern LinkNode model_load_up;
extern LinkNode model_load_down;
extern LinkNode model_load_left;
extern LinkNode model_load_right;
extern int model_read_file;
int File_Open(char *, int, int);
int File_OpenUnflagged(char *name, int nowait, int pack);
int File_Read(int, void *, int);
int File_Close(int file);
int File_Sync(int file, int mode);
int FlushCache(int);
void PoolPartFile_Relocate(char *);
extern int worn_model_loading;
extern PoolPartSource *pool_part_sources[13];
void PoolSlot_LoadGroup(PoolPartSource *, u32);
extern ChunkPoolNode *free_load_buffers;
extern ChunkPoolNode load_buffer_nodes[];
int sprintf(char *, char *, ...);
extern char model_name_shown[];
extern char model_name_up[];
extern char model_name_down[];
extern char model_name_left[];
extern char model_name_right[];
extern char weapon_model_name[];
extern char shield_model_name[];
extern char armor_model_name[];
extern char helm_model_name[];
extern char gauntlet_model_name[];
extern char boot_model_name[];
extern char ring_model_name[];
extern char bracelet_model_name[];
extern char amulet_model_name[];
extern LinkNode worn_model_loader;
extern LinkNode weapon_model_load;
extern LinkNode shield_model_load;
extern LinkNode armor_model_load;
extern LinkNode helm_model_load;
extern LinkNode gauntlet_model_load;
extern LinkNode boot_model_load;
extern LinkNode ring_model_load;
extern LinkNode bracelet_model_load;
extern LinkNode amulet_model_load;
extern LinkNode key_item_model_loads[12];
extern s32 packet_area;
extern ChunkUpload npc_package_load;
int MenuArena_Init(int); // match: this file only matches with an int result
void *MenuArena_Alloc(s32, s32);
void *MenuHeap_Alloc(int);
int MenuFile_Load(char *, void *);
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
extern u32 pretune_part_count;
extern u32 tune_part_count;
extern u32 tune_s_part_count;
extern u32 mc_part_count;
extern u32 bright_part_count;
extern u32 shop_mess_part_count;
extern u32 tune_mess_part_count;
extern u32 mc_mess_part_count;
extern u32 equip_m2_part_count;
extern SpritePacket *slot_parts;
extern SpritePacket *data_parts;
extern SpritePacket *preshop_parts;
extern SpritePacket *shop_parts;
extern SpritePacket *shop_g2_parts;
extern SpritePacket *shop_a_parts;
extern SpritePacket *shop_g_parts;
extern SpritePacket *pretune_parts;
extern SpritePacket *tune_parts;
extern SpritePacket *tune_s_parts;
extern SpritePacket *mc_parts;
extern SpritePacket *bright_parts;
extern SpritePacket *shop_mess_parts;
extern SpritePacket *tune_mess_parts;
extern SpritePacket *mc_mess_parts;
extern SpritePacket *equip_m2_parts;
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
extern PlayerItemDef weapon_defs[];
extern LevelRecordDef worn_defs[];
extern GsDoubleBufferDc menu_frame_buffers;
extern LoadedTexture menu_font_texture;
extern MenuModelMotion *equip_weapon_motions[];
int sceGsResetGraph(int, int, int, int);
void sceGsSetDefDBuffDc(void *, s16, s16, s16, s16, s16, s16);
void MenuHeap_Reset(void);
void MenuFile_IndexBlocks(char *);
void MenuHeap_Mark(void);
void MenuTexture_GetRegisters(TextureHeader *, u64 *, u64 *);
void MenuHeap_FreeToMark(void);
s32 PoolNodeFile_Relocate(RelocFile *);
struct sceCdCLOCK;
int sceCdReadClock(struct sceCdCLOCK *);
int sceGsSyncV(int);
int mktime(int *);
void Vram_Clear(int, int);
void Sound_MuteTracked(void);
void Sound_UnmuteTracked(void);
void Sound_UpdateTagged(void);
void Sound_MuteTagged(void);
void Sound_UnmuteTagged(void);
void Sound_UnmutePositional(void);
void Sound_MutePositional(void);
void Input_Update(void);
int SoundDriver_FlushCommands(void);
s16 MenuPad_Update(void);
extern GsDrawEnv game_dbuff_draw0;
extern s32 menu_run_step;
extern s8 menu_mode;
extern u16 menu_buttons;
extern int context1_buffer_half;
extern int context2_buffer_half;
extern s32 menu_even_field;
extern s32 pad_mode_change_frames;
extern int pad_mode;
extern int pad_mode_before;
extern MenuScreenState card_screen_state;
extern ColorBytes menu_draw_color;
extern f32 menu_view_screen_matrix[16];
void MenuModel_DrawTinted(ChunkUpload *upload, s32 draw_param, RenderSlotArg slot_arg);
// match: called with the node, a matrix and the colour, not as the definition has it (the colour is loaded unaligned)
#define DRAW_NODE_MODEL(node, matrix, colour) ((void (*)(LinkNode *, f32 *, ColorBytes))MenuModel_DrawTinted)(node, matrix, colour)
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
extern u64 menu_first_draw_env[47];             /* the frame register of the first buffer ([0]) and of the second ([46]) */
extern u8 game_options[0x10];  /* the options block */
extern char menu_first_draw_env2[];
extern char menu_second_draw_env[];           /* inside menu_frame_buffers: the second buffer's draw environments */
extern char menu_second_draw_env2[];
extern s32 menu_frame_ticks;
extern u8 D_00447208;
void Menu_DrawBrightness(s32);
int Timer_GetTicks(void);
int Timer_Restart(void); // match: an int result keeps $2 clear of the pad test after the call
int sceGsSyncPath(int, int);
void sceGsSetHalfOffset(void *, int, int, s16); // match: void, so the test after the call uses $2
int sceGsSetHalfOffset2(void *, int, int, s16);
int sceGsSwapDBuffDc(GsDoubleBufferDc *, int);

// match: called as a void function, so the result register is free at once
#define SET_TEST(value) ((void (*)(int, s64))MenuDraw_SetGsRegister)(GS_REG_TEST_1, value)

extern ColorBytes backdrop_shade_color[]; // match: the initialiser of the local below ({ 0x10, 0x10, 0x10 }); an array so it is not reached through $gp
int Menu_SetBlendMode(u8);
void MenuSprite_DrawPlain(int owner, int index, int param);
extern s32 menu_step;
extern char D_005CE0E0[];
extern u8 brightness_option[];
extern LoadedBlockRef menu_file_blocks[];
extern void *menu_screen_handler;
extern float pulse_sprite_alpha;
extern char pulse_sprite_falling;
extern s8 pulse_value_falling;
extern s8 system_screen_state;
extern ColorBytes panel_text_color;
extern ColorBytes model_draw_color;
extern s32 menu_file_block_count;
extern s32 menu_heap;
extern u8 brightness_before;
void func_001095D0(void);
void func_001095D8(void);
void Packet_SwapBuffers(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void GsDraw_QueueTexFlush(void);
void TextureTable_QueueAll(TextureTableHeader *);
void MenuTexture_Upload(TextureHeader *, LoadedBlockRef *);
extern s8 main_menu_line;
void Sound_LoadMgBank(int);
void *memset(void *, int, u32);
extern f32 screen_part_alphas[0x100];
extern f32 panel_part_alphas[0x100];
extern char equip_model_shown;
extern s8 menu_input_enabled;
extern s8 main_menu_state;               /* 0: fading in, 1: open, 2: fading out to the chosen screen, 3: fading out to leave */
extern s8 recollection_cursor;
s32 Sound_PlayMenu(s32);
int Menu_OwnsAnyItem(void);
int Menu_KnowsCastableSpell(void);
void MenuSprite_Draw(int, int, int, int);
extern u8 D_00446B39;
extern GameStatus game_status;
extern f32 use_item_model_position[4];
extern f32 use_item_model_angle;              /* the shown item model's turn about y */
extern s32 use_item_frames;
extern s32 D_00446DF4;
extern s8 use_item_state;               /* 0: fading in, 1: open, 2 to 4: waiting for the models, 5 to 7: fading out */
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void MenuDraw_Bar(MenuCursorBar *);
s32 MenuText_SetColor(ColorBytes bytes);
s32 MenuText_DrawWrapped(u16 *, s32);
s32 MenuText_DrawWordWrappedLong(u16 *, s32);
/* an angle brought back into -pi..pi (for angles above -pi) */
static inline f32 wrap_angle(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}
extern f32 cast_spell_model_position[4];
extern s32 D_00446DF8;
extern s8 cast_spell_state;               /* 0: fading in, 1: open, 2: waiting for the models, 3: fading out */
int MenuText_SetGlyphSize(GsPackedWord);
s32 MenuText_DrawLine(u16 *text, s32 limit);
extern u8 player_weapon[];
extern MenuItem **equipped_item_refs[11];
extern LinkNode *equip_model_loads[];
extern f32 equip_item_model_position[4];
extern f32 equip_weapon_model_position[4];
extern void (*pool_slot_drawers[])(PoolSlot *, ColorBytes);
extern PoolSlot pool_slots[32];
extern s32 D_00446ACC;
extern u8 detail_panel_open; // match: a byte here; ui/trade.c reads and writes it as a word
extern u8 equipped_list_open;
extern s8 detail_panel_sound_pending;
extern f32 equip_item_model_angle;
extern f32 equip_weapon_model_angle;
extern u32 equip_weapon_motion_frame;
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
extern MenuFreeNode D_004472B8;
extern s8 equip_state;
extern s8 equip_panel_state;
// match: equip_panel_state is the second byte of what the symbol list has as one two-byte object. It has to be a variable of
// its own: a store to `equip_state[1]` is never put in a delay slot, and retail has it there.
__asm__("equip_panel_state = equip_state + 1");
void Player_RecomputeStats(void);
int Menu_IsEquipSlotFilled(s32, u32);
int Menu_IsEquipped(u32, s32, s32);
s8 MenuDraw_GetWordStart(s32);
void MenuDraw_Word(KindObject *);
s32 MenuText_DrawLineCentered(u16 *, s32);
void PoolSlot_Draw(RenderSlot *, RenderSlotArg);
s32 PoolSlot_ClearParts(void);
void PoolNode_Pose(RelocEntry *, PoolSlot *, f32 *, s32);
s32 MenuDraw_SetGsRegister(int, s64);
/* the item the cursor is on in the category shown */
// match: the category is a MenuItem whose first word is a MenuItem pointer; read as an int it cannot pass the GS stores
// match: the list read as a MenuItem (next, prev and group are at the same offsets)
#define EQUIP_LIST (((MenuItem **)equip_list_shown)[0])
#define CURRENT_ITEM (((MenuItemList *)EQUIP_LIST)->item)
// match: called as a void function, so the result register is free at once
#define SET_BLEND(mode) ((void (*)(u8))Menu_SetBlendMode)(mode)
#define DRAW_ITEM_MODEL(matrix) DRAW_NODE_MODEL(&model_load_shown, matrix, model_draw_color)
// match: called with the slot alone
#define DRAW_SLOT(slot) ((void (*)(PoolSlot *))PoolSlot_Draw)(slot)
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
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)); \
    *D2_QWC = count;                                            \
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;                                              \
    *D2_CHCR = D_CHCR_STR
#define RESET_ALPHAS(parts, count)                                                                   \
    for (i = 0; i < count; i++) {                                                                    \
        alpha = parts[i].alpha;                                                                      \
        panel_part_alphas[i] = alpha;                                                                       \
    }
#define DRAW_PARTS(parts, count, variant)                                                            \
    for (i = 0; i < count; i++) {                                                                    \
        SET_BLEND(parts[i].blend_mode);                                                              \
        MenuSprite_Draw((int)&parts[i], parts[i].handler_index + 1000, variant, (int)panel_part_alphas[i]);         \
        SET_BLEND(0);                                                                                \
    }
extern s8 key_items_state;               /* 0: fading in, 1: open, 2: fading out to Menu_UpdateKeyItems, 3: fading out to leave */
extern s32 key_item_slots[12];
extern s32 key_item_row;              /* the row of three the list starts at, 0 to 2 */
extern s32 key_items_frames;
#define LIST_PAGES 50
#define LIST_ROWS 7
extern u8 recollection_flags[LIST_PAGES][8];
extern u8 recollection_pages[50];
extern f32 recollection_model_position[4];
extern s32 recollection_page;              /* the page shown, 0 to 49 */
extern s32 recollection_page_before;              /* the page shown the frame before */
extern s8 recollection_state;               /* 0: fading in, 1: open, 2: fading out */
extern s8 recollection_top;               /* the counter at the top of the list */
int sceCdBreak(void);
s32 MenuText_DrawWrappedLong(u16 *, s32);
s32 MenuText_DrawLineShortened(u16 *, s32);
/* The offset of text `number` in the text table, 0 when it has none. */
static inline s32 GetListTextOffset(s32 table, s32 number) {
    return ((s32 *)table)[number];
}
extern f32 recollection_page_model_position[4];
/* The offset of text `number` in the text table, 0 when it has none. */
static inline s32 GetPageTextOffset(s32 table, s32 number) {
    return ((s32 *)table)[number];
}
extern f32 visible_item_model_position[4];
extern f32 visible_item_model_angle;              /* the turning model's angle */
extern s32 D_00446E2C;
extern MenuItem *visible_bar_top;
extern MenuItem *visible_bar_cursor;
extern MenuItem *visible_names_top;
extern MenuItem *visible_names_cursor;
extern MenuItem *visible_text_item;
extern s8 visible_item_state;               /* 0: fading in, 1: open, 2: waiting for the loads, 3: fading out */
extern s8 visible_panel_state;               /* the detail panel's: 0: fading in, 1: open, 2: fading out */
static inline s32 ScaleValue(s32 value, s32 percent, s32 divisor) {
    return value * percent / divisor;
}
/* Starts every packet of a list at its full alpha. match: the alpha is read as an unsigned bit-field */
#define FILL_FADE(levels, packets, count)                                                            \
    for (i = 0; i < (count); i++) {                                                                  \
        (levels)[i] = SPRITE_PACKET_COLOR(&(packets)[i])->alpha;                                     \
    }
/* Draws a list of packets at their current fade levels. */
#define DRAW_PACKETS(packets, count, param)                                                          \
    for (i = 0; i < (count); i++) {                                                                  \
        Menu_SetBlendMode((packets)[i].blend_mode);                                                  \
        MenuSprite_Draw((int)&(packets)[i], (packets)[i].handler_index + 1000, (param), (int)panel_part_alphas[i]); \
        ((void (*)(u8))Menu_SetBlendMode)(0);                                                        \
    }
extern s32 system_menu_line;
extern s32 quit_choice;
extern u16 button_settings_copy[13];
extern u16 forward_button;
extern u16 back_button;
extern u16 turn_right_button;
extern u16 turn_left_button;
extern u16 step_right_button;
extern u16 step_left_button;
extern u16 look_down_button;
extern u16 look_up_button;
extern u16 attack_button;
extern u16 magic_button;
extern u16 action_button;
extern u16 menu_button;
extern u16 shortcut_button;
// match: both are called as void functions, so the result register is free at once
#define PLAY_MENU_SOUND(sound) ((void (*)(s32))Sound_PlayMenu)(sound)
#define SET_PART_MODE(mode) ((void (*)(u8))Menu_SetBlendMode)(mode)
s32 SaveCard_Open(void);
s32 SaveCard_StartCheck(s8);
#define PAGE_ROWS 7
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 ScrollItemListUp(void) {
    item_list_top[0] = item_list_top[0]->prev;
    item_list_page_mark[0] = item_list_page_mark[0]->prev;
    item_list_cursor[0] = item_list_top[0];
    return item_list_top[0] != item_list_last_page;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 ScrollItemListDown(void) {
    item_list_top[0] = item_list_top[0]->next;
    item_list_page_mark[0] = item_list_page_mark[0]->next;
    item_list_cursor[0] = item_list_top[0];
    return item_list_top[0] != item_list_first;
}
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 ScrollAllItemsUp(void) {
    owned_list_top[0] = owned_list_top[0]->prev;
    owned_list_page_mark[0] = owned_list_page_mark[0]->prev;
    owned_list_current[0] = owned_list_top[0];
    return owned_list_top[0] != owned_list_last_page;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 ScrollAllItemsDown(void) {
    owned_list_top[0] = owned_list_top[0]->next;
    owned_list_page_mark[0] = owned_list_page_mark[0]->next;
    owned_list_current[0] = owned_list_top[0];
    return owned_list_top[0] != owned_list_first;
}
extern s8 card_slot_leave_steps;
extern u8 card_save_mask;
extern void *save_list_opener;
s32 SaveCard_Update(void); // match: the result is used here
s32 SaveCard_StartReadList(s32 card, char must_exist);
void SaveCard_Close(void);
s32 SaveCard_CheckBusy(void);
s32 SaveCard_CheckSavesIntact(void);
s32 SaveCard_GetSaveMask(void);
void SaveCard_ClearLoadFailed(s32);
s32 SaveCard_IsChecking(void);
s8 SaveCard_IsPresent(s32);
extern u8 save_image[];
extern s8 save_list_leave_steps;
extern s32 D_00446E34;
extern s32 D_00446E38;
extern s32 D_00446E3C;
extern s32 D_00446E40;
extern s32 D_00446E44;
extern s32 D_00446E48;
extern s32 D_00446E4C;
extern s32 D_00446E50;
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
extern s32 D_00446E80;
s32 SaveCard_Init(void);
s32 SaveCard_StartLoad(s32, s32, s32 *, s32);
s32 SaveCard_HasCard(s32);
s8 SaveCard_GetSaveState(s32);
s32 SaveCard_HasRoomFor(s32, s32, s32);
void SaveCard_ResetCard(s32);
s8 SaveCard_GetLoadFailed(s32);
extern s32 sound_mode_before;              /* option 0 as it was before it was first toggled */
extern s8 sound_mode_change;               /* 0: option 0 untouched, 1: toggled and not applied yet, 2 to 4: applied */
extern s8 options_leave_steps;               /* counts the two steps of leaving when the screen is a start-up step */
void Sound_SetBgmVolume(s32);
void Sound_SyncBgmLevel(void);
void SoundDriver_SetMono(void);
void SoundDriver_SetStereo(void);
extern s8 button_choices[12];
extern s8 button_choices_shown[12];
extern s8 buttons_leave_steps;
extern s32 button_line;
extern s32 button_prompt_choice;
extern s8 button_prompt;
/* Runs SET with the choice number that goes with a pad button bit. */
#define FOR_PAD_BIT(bit, SET) \
    switch (bit) {            \
    case 0x10:                \
        SET(0);               \
        break;                \
    case 0x40:                \
        SET(1);               \
        break;                \
    case 0x20:                \
        SET(2);               \
        break;                \
    case 0x80:                \
        SET(3);               \
        break;                \
    case 0x400:               \
        SET(4);               \
        break;                \
    case 0x100:               \
        SET(5);               \
        break;                \
    case 0x1000:              \
        SET(6);               \
        break;                \
    case 0x4000:              \
        SET(7);               \
        break;                \
    case 0x2000:              \
        SET(8);               \
        break;                \
    case 0x8000:              \
        SET(9);               \
        break;                \
    case 0x800:               \
        SET(10);              \
        break;                \
    case 0x200:               \
        SET(11);              \
        break;                \
    }
#define SET_CHOICE(n) button_choices_shown[i] = button_choices[i] = (n)
#define SET_SHOWN(n) button_choices_shown[i] = (n)
s32 SaveCard_StartRescan(void);
s32 SaveCard_IsCheckDone(void);
s8 SaveCard_IsFormatted(s32);
int SaveCard_GetCardProblem(int);
s32 SaveCard_GetSavesState(void);
extern f32 brightness_slider;

static void Menu_BuildItemRings(void);
static void Menu_MoveLoadToSpare(void);
static void Menu_CopyLoadToSpare(void);
static void Menu_ShiftEquipModelsNextList(int value, void *unused_data);
static void Menu_ShiftEquipModelsPrevList(int arg, void *unused_data);
static void Menu_ShiftEquipModelsPrevItem(int unused, void *unused_data);
static void Menu_ShiftEquipModelsNextItem(int unused, void *unused_data);
static void Menu_OpenModelFile(ChunkUpload *upload);
static void Menu_WaitModelRead(ChunkUpload *upload);
static void Menu_FinishModelLoad(ChunkUpload *upload);
static void Menu_OpenWornModelFile(LinkNode *node);
static void Menu_WaitWornModelRead(ChunkUpload *upload);
static void Menu_FinishWornModelLoad(LinkNode *node);
static void Menu_StepWornModelLoad(ChunkUpload *upload);
static void Menu_OpenPackageFile(ChunkUpload *upload);
static void Menu_WaitPackageRead(ChunkUpload *upload);
static void Menu_FinishPackageLoad(ChunkUpload *upload);
static void Menu_StepPackageLoad(ChunkUpload *upload);
static void Menu_FreeLoadBuffer(MenuFreeRequest *request);
static void Menu_WriteItemModelName(char *buffer, MenuItem *item);
static void Menu_WriteSpellModelName(char *buffer, MenuItem *item);
static void Menu_WriteGroup10ModelName(char *buffer, MenuItem *item);
static void Menu_SetEquipModelNames(void);
static void Menu_SetWornModelNames(void);
static void Menu_ChainEquipModels(void);
static void Menu_SetAllItemsModelNames(void);
static void Menu_ChainAllItemsModels(void);
static void Menu_SetSpellModelNames(void);
static void Menu_ChainSpellModels(void);
static void Menu_ChainKeyItemModels(void);
static void Menu_ShiftSpellModelsNext(int unused);
static void Menu_ShiftSpellModelsPrev(int unused);
static void Menu_LoadPartFile(char *name, u32 *record_count, SpritePacket **records);
static void Menu_LoadAllParts(void);
static void Menu_SendUnusedFlatSprite(void);
static void Menu_SendUnusedSpriteFirst(void);
static void Menu_SendUnusedSpriteSecond(void);
static s32 Menu_CopyFrameToTexture(void);
static int Menu_ReturnTrue(void);
static void Menu_DrawBackdropPlain(void);
static void Menu_DrawBackdropItemSelParts(void);
static void Menu_DrawBackdropPreshopParts(void);
static int Menu_RunStep(void);
static s32 Menu_Start(void);
static s32 Menu_RunHandler(void);
static s32 Menu_CutModelChain(void);
static void Menu_OpenMain(void);
static void Menu_UpdateMain(HandlerContext *context);
static void Menu_OpenChosenScreen(HandlerContext *context);
static void Menu_StepBlink(void);
static void Menu_MoveItemAfter(MenuItem *item, MenuItem *after);
static f32 Menu_PulseValue(f32 value, s32 start, s32 target);
static void Menu_UpdateUseItem(HandlerContext *context);
static void Menu_UpdateCastSpell(s32 context);
static void Menu_UpdateEquip(void);
static void Menu_UpdateStatus(void);
static void Menu_UpdateKeyItems(void);
static void Menu_UpdateRecollection(HandlerContext *context);
static void Menu_UpdateRecollectionPage(HandlerContext *context);
static void Menu_UpdateVisibleItem(HandlerContext *context);
static void Menu_UpdateSystem(MenuScreenContext *context);
static int Menu_MoveSpellCursor(void);
static int Menu_MoveEquipCursor(void);
static s32 Menu_MoveAllItemsCursor(void);
static void Menu_UpdateCardSlot(void);
static void Menu_UpdateSaveList(void);
static void Menu_UpdateOptions(MenuScreenContext *context);
static void Menu_ReadButtonSettings(void);
static void Menu_UpdateButtonConfig(void);
static void Menu_OpenCardCheck(void);
static void Menu_UpdateCardCheck(void);
static void Menu_UpdateBrightness(void);
static void Menu_WaitForCancel(void);
static int Menu_DrawEquipItemText(s32 index);
static void Menu_SetItemListModelNames(void);
static void Menu_OpenCardSlot(void);

// match: only a named section keeps a table of eight bytes out of small data; retail has these in .rodata
// and reaches them without the global pointer
#define IN_RODATA __attribute__((section(".rodata")))

/* the two steps of Menu_Run's loop: the backdrop, then the screen */
static int (*const menu_run_steps[2])(void) IN_RODATA = {
    Menu_CopyFrameToTexture,
    Menu_RunStep,
};
/* what Menu_EndFrame draws at the end of a frame, by option 8 */
static void (*const frame_end_drawers[4])(void) = {
    Menu_DrawBackdropPlain,
    Menu_DrawBackdropPreshopParts,
    Menu_DrawBackdropItemSelParts,
    NULL,
};

/* Builds every ring the menus walk from what the player owns: the items, everything, the usable items and spells,
   one ring per equipment group, the spells known and the spells that can be cast. */
static void Menu_BuildItemRings(void) {
    MenuItem *last = NULL;
    MenuItem *first = NULL;
    s32 count = 0;
    s32 i;
    s32 spell;

    item_list_top[0] = NULL;
    item_list_page_mark[0] = NULL;
    item_list_first = NULL;
    item_list_last_page = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 150; i++) {
        if (item_counts[i] != 0 && (item_name_defs[i].state & 1)) {
            LINK_COUNTED_ITEM(&common_item_entries[i], item_list_page_mark)
        }
    }
    item_list_count = count;
    if (first != NULL) {
        last->prev = first;
        item_list_cursor[0] = item_list_top[0] = use_item_list.item = first;
        first->next = last;
        if (item_list_page_mark[0] == NULL) {
            item_list_page_mark[0] = last->next;
        } else {
            item_list_page_mark[0] = item_list_page_mark[0]->next;
        }
        item_list_first = first;
        item_list_last_page = last;
        for (i = 5; i >= 0; i--) {
            item_list_last_page = item_list_last_page->next;
        }
    } else {
        item_list_cursor[0] = NULL;
    }

    owned_list_top[0] = NULL;
    owned_list_page_mark[0] = NULL;
    owned_list_first = NULL;
    owned_list_last_page = NULL;
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 380; i++) {
        if (item_counts[i] != 0 && !(item_name_defs[i].state & 2)) {
            if ((i >= 0 && i < 150) || (i >= 150 && i < 220 && weapon_level_records[i - 150].in_smith_rings == 0) ||
                (i >= 220 && i < 250 && worn_item_level_records[i - 220].in_smith_rings == 0) ||
                (i >= 250 && i < 275 && worn_item_level_records[i - 220].in_smith_rings == 0) ||
                (i >= 275 && i < 300 && worn_item_level_records[i - 220].in_smith_rings == 0) ||
                (i >= 300 && i < 325 && worn_item_level_records[i - 220].in_smith_rings == 0) ||
                (i >= 325 && i < 350 && worn_item_level_records[i - 220].in_smith_rings == 0) ||
                (i >= 350 && i < 360 && worn_item_level_records[i - 220].in_smith_rings == 0) ||
                (i >= 360 && i < 370 && worn_item_level_records[i - 220].in_smith_rings == 0) ||
                (i >= 370 && worn_item_level_records[i - 220].in_smith_rings == 0)) {
                count++;
                if (count == 7) {
                    owned_list_page_mark[0] = &all_item_entries[i];
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
    owned_list_count = count;
    if (first != NULL) {
        last->prev = first;
        owned_list_current[0] = owned_list_top[0] = first;
        first->next = last;
        if (owned_list_page_mark[0] == NULL) {
            owned_list_page_mark[0] = last->next;
        } else {
            owned_list_page_mark[0] = owned_list_page_mark[0]->next;
        }
        owned_list_first = first;
        owned_list_last_page = last;
        for (i = 5; i >= 0; i--) {
            owned_list_last_page = owned_list_last_page->next;
        }
    } else {
        owned_list_current[0] = NULL;
    }

    last = NULL;
    first = NULL;
    for (i = 0; i < 150; i++) {
        if (item_counts[i] != 0 && (item_name_defs[i].state & 1) && !((u8)item_common_defs[i].flags & 1) && !(i >= 145 && i < 148)) {
            LINK_ITEM(&shortcut_entries[i])
        }
    }
    for (i = 150, spell = 0; i < 241; i++, spell++) {
        if (SPELL_IS_SET(spell) && fx_table[spell].item_menu_magic == 1) {
            LINK_ITEM(&shortcut_entries[i])
        }
    }
    // match: closed without the `first != NULL` test of the counted rings (so are the rings below); an empty one
    // would be written through NULL
    first->next = last;
    last->prev = first;
    switch (player_state.shortcut_type) {
    case 0:
        equip_shortcut_list.item = equipped_shortcut.item = shortcut_entries;
        // match: no break, case 0 goes on into case 1, which sets both again
    case 1:
        if (item_counts[player_state.shortcut] != 0) {
            equip_shortcut_list.item = equipped_shortcut.item = &shortcut_entries[player_state.shortcut];
        } else {
            player_state.shortcut = 0;
            equip_shortcut_list.item = equipped_shortcut.item = shortcut_entries;
        }
        break;
    case 2:
        equip_shortcut_list.item = equipped_shortcut.item = &shortcut_entries[player_state.shortcut + 150];
        break;
    }

    last = NULL;
    first = NULL;
    for (i = 0; i < 70; i++) {
        if (item_counts[150 + i] != 0 && (item_name_defs[150 + i].state & 1) && weapon_level_records[i].in_smith_rings == 0) {
            LINK_ITEM(&weapon_entries[i])
        }
    }
    first->next = last;
    last->prev = first;
    equip_weapon_list.item = equipped_weapon.item = &weapon_entries[player_state.weapon];

    LINK_GROUP(shield_entries, worn_item_level_records, 0, 220, 30, equip_shield_list, equipped_shield, player_state.equipment[EQUIP_SHIELD])
    LINK_GROUP(helm_entries, worn_item_level_records, 30, 250, 25, equip_helm_list, equipped_helm, player_state.equipment[EQUIP_HELM])
    LINK_GROUP(armor_entries, worn_item_level_records, 55, 275, 25, equip_armor_list, equipped_armor, player_state.equipment[EQUIP_ARMOR])
    LINK_GROUP(gauntlet_entries, worn_item_level_records, 80, 300, 25, equip_gauntlet_list, equipped_gauntlet, player_state.equipment[EQUIP_GAUNTLET])
    LINK_GROUP(boot_entries, worn_item_level_records, 105, 325, 25, equip_boot_list, equipped_boot, player_state.equipment[EQUIP_BOOT])
    LINK_GROUP(ring_entries, worn_item_level_records, 130, 350, 10, equip_ring_list, equipped_ring, player_state.equipment[EQUIP_RING])
    LINK_GROUP(bracelet_entries, worn_item_level_records, 140, 360, 10, equip_bracelet_list, equipped_bracelet, player_state.equipment[EQUIP_BRACELET])
    LINK_GROUP(amulet_entries, worn_item_level_records, 150, 370, 10, equip_amulet_list, equipped_amulet, player_state.equipment[EQUIP_AMULET])

    last = NULL;
    first = NULL;
    for (i = 0; i < 91; i++) {
        if (i == 0 || (SPELL_IS_SET(i) && fx_table[i].item_menu_magic == 0)) {
            LINK_ITEM(&spell_entries[i])
        }
    }
    first->next = last;
    last->prev = first;
    castable_spell_top[0] = NULL;
    equip_spell_list.item = equipped_spell.item = &spell_entries[player_state.equipped_magic];
    castable_spell_page_mark[0] = NULL;

    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 91; i++) {
        if (SPELL_IS_SET(i) && fx_table[i].item_menu_magic == 1) {
            LINK_COUNTED_ITEM(&spell_entries[i], castable_spell_page_mark)
        }
    }
    if (first != NULL) {
        first->next = last;
        castable_spell_first[0] = first;
        castable_spell_top[0] = castable_spell_cursor[0] = first;
        last->prev = first;
        if (castable_spell_page_mark[0] == NULL) {
            castable_spell_page_mark[0] = last->next;
        } else {
            castable_spell_page_mark[0] = castable_spell_page_mark[0]->next;
        }
    } else {
        castable_spell_cursor[0] = NULL;
    }
    equip_list_shown[0] = (MenuItemList *)equip_list_refs[equip_group_shown];
}

/* Runs the callback of the load node at the head of the chain, or goes on to the next node when it has none or is done. */
void Menu_StepModelLoads(int unused) {
    LinkNode *node = model_load_chain;

    if (node != NULL) {
        if (node->unk20 == 0 && node->callback != NULL) {
            ((void (*)(LinkNode *))node->callback)(node); // match: the callback gets its node
        } else {
            model_load_chain = node->next;
        }
    }
}

/* Moves the model load in progress into the spare node model_load_spare, so that its own node can be reused: the
   node is reset with a fresh buffer and the chain is cut after the spare. */
static void Menu_MoveLoadToSpare(void) {
    LinkNode *node = model_load_chain;

    if (node != NULL && node->callback == (void (*)(void))Menu_StepModelLoad && node != &model_load_spare) {
        model_load_spare = *node;
        Menu_TakeLoadBuffer(model_load_chain);
        model_load_chain->unk08 = -1;
        model_load_chain->unk20 = 0;
        model_load_chain->unk1C = 0;
        model_load_chain = &model_load_spare;
        if (model_load_spare.next != NULL && model_load_spare.next->callback == (void (*)(void))Menu_StepModelLoad) {
            model_load_spare.next = NULL;
        }
        Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_spare);
        node = NULL; // match: a statement after the call keeps it from becoming a sibling jump
    }
}

/* Copies the model load in progress into the spare node model_load_spare, which then loads for the node it was
   copied from, and cuts the chain after it. */
static void Menu_CopyLoadToSpare(void) {
    LinkNode *node = model_load_chain;

    if (node != NULL) {
        if (node->callback == (void (*)(void))Menu_StepModelLoad) {
            if (node != &model_load_spare) {
                model_load_spare = *node;
                node->unk20 = 0;
                node->unk1C = 0;
                model_load_spare.unk10 = node;
                model_load_chain = &model_load_spare;
                if (model_load_spare.next != NULL && model_load_spare.next->callback == (void (*)(void))Menu_StepModelLoad) {
                    model_load_spare.next = NULL;
                }
                return;
            }
            if (node->next->callback != node->callback) { // match: next is not checked for NULL here
                return;
            }
        }
        node->next = NULL;
    }
}

/* The equipment screen went on to the next list: the model shown moves to node 2, node 1's becomes the one
   shown, nodes 1, 3 and 4 are made fresh, and the five are chained and named again. */
static void Menu_ShiftEquipModelsNextList(int value, void *unused_data) {
    LinkNode *pending = NULL;
    LinkNode *current;

    if (model_load_chain == &model_load_up || model_load_chain == &model_load_shown) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_down);
    model_load_down = model_load_shown;
    model_load_down.next = NULL;
    if (model_load_down.unk20 == 0) {
        pending = &model_load_down;
    }
    model_load_shown = model_load_up;
    model_load_shown.next = NULL;
    if (model_load_shown.unk20 == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_up);
    if (pending == NULL) {
        pending = &model_load_up;
    }
    model_load_up.unk20 = 0;
    model_load_up.unk1C = 0;
    model_load_up.unk08 = -1;
    model_load_right.unk20 = 0;
    model_load_right.unk1C = 0;
    model_load_right.unk08 = -1;
    model_load_right.next = NULL;
    model_load_left.unk20 = 0;
    model_load_left.unk1C = 0;
    model_load_left.unk08 = -1;
    model_load_up.next = NULL;
    current = model_load_chain;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_down;
    model_load_down.next = &model_load_up;
    model_load_up.next = &model_load_left;
    model_load_left.next = &model_load_right;
    model_load_right.next = NULL;
    Menu_SetEquipModelNames();
}

/* The equipment screen went back to the list before: the model shown moves to node 1, node 2's becomes the
   one shown, nodes 2, 3 and 4 are made fresh, and the five are chained and named again. */
static void Menu_ShiftEquipModelsPrevList(int arg, void *unused_data) {
    LinkNode *pending = NULL;
    LinkNode *current;

    if (model_load_chain == &model_load_down || model_load_chain == &model_load_shown) {
        ((void (*)(int))Menu_CopyLoadToSpare)(arg); // match: defined (void); passing the handler's argument through fixes the saved registers
    } else {
        ((void (*)(int))Menu_MoveLoadToSpare)(arg); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_up);
    model_load_up = model_load_shown;
    model_load_up.next = NULL;
    if (model_load_up.unk20 == 0) {
        pending = &model_load_up;
    }
    model_load_shown = model_load_down;
    model_load_shown.next = NULL;
    if (model_load_shown.unk20 == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_down);
    if (pending == NULL) {
        pending = &model_load_down;
    }
    model_load_down.unk20 = 0;
    model_load_down.unk1C = 0;
    model_load_down.unk08 = -1;
    model_load_right.unk20 = 0;
    model_load_right.unk1C = 0;
    model_load_right.unk08 = -1;
    model_load_right.next = NULL;
    model_load_left.unk20 = 0;
    model_load_left.unk1C = 0;
    model_load_left.unk08 = -1;
    model_load_down.next = NULL;
    current = model_load_chain;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = &model_load_left;
    model_load_left.next = &model_load_right;
    model_load_right.next = NULL;
    Menu_SetEquipModelNames();
}

/* The equipment screen went back one item in its list: the model shown moves to node 3, node 4's becomes the
   one shown, node 4 is made fresh, and the five are chained and named again. */
static void Menu_ShiftEquipModelsPrevItem(int unused, void *unused_data) {
    LinkNode *pending = NULL;
    LinkNode *current;

    if (model_load_chain != &model_load_left) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_left);
    model_load_left = model_load_shown;
    model_load_left.next = NULL;
    if (model_load_left.unk20 == 0) {
        pending = &model_load_left;
    }
    model_load_shown = model_load_right;
    model_load_shown.next = NULL;
    if (model_load_shown.unk20 == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_right);
    if (pending == NULL) {
        pending = &model_load_right;
    }
    model_load_up.next = NULL;
    model_load_down.next = NULL;
    model_load_right.unk20 = 0;
    model_load_right.unk1C = 0;
    model_load_right.next = NULL;
    model_load_right.unk08 = -1;
    current = model_load_chain;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_left;
    model_load_left.next = &model_load_right;
    model_load_right.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = NULL;
    Menu_SetEquipModelNames();
}

/* The equipment screen went on one item in its list: the model shown moves to node 4, node 3's becomes the one
   shown, node 3 is made fresh, and the five are chained and named again. */
static void Menu_ShiftEquipModelsNextItem(int unused, void *unused_data) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (model_load_chain != &model_load_right) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_right);
    model_load_right = model_load_shown;
    node = &model_load_right;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &model_load_left; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    model_load_shown = model_load_left;
    done = model_load_shown.unk20;
    model_load_shown.next = NULL;
    if (done == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_left);
    if (pending == NULL) {
        pending = &model_load_left;
    }
    model_load_up.next = NULL;
    model_load_down.next = NULL;
    model_load_left.unk20 = 0;
    model_load_left.unk1C = 0;
    model_load_left.next = NULL;
    model_load_left.unk08 = -1;
    current = model_load_chain;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_right;
    model_load_right.next = &model_load_left;
    model_load_left.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = NULL;
    Menu_SetEquipModelNames();
}

/* Stage 0 of a menu model's load: opens the node's file (a stand-in model when there is none) and starts reading it. */
static void Menu_OpenModelFile(ChunkUpload *upload) {
    s32 file_handle;

    file_handle = File_OpenUnflagged(upload->name, 1, 1);
    upload->unk08 = file_handle;
    if (file_handle < 0) {
        upload->unk08 = File_Open("menu/select2/i0000.mod", 1, 1);
    }
    File_Read(upload->unk08, upload->pool_node->buffer, 0x25800);
    model_read_file = upload->unk08;
    upload->stage++;
}

/* Stage 1 of a menu model's load: when the read has finished closes the file and goes on; on error starts over. */
static void Menu_WaitModelRead(ChunkUpload *upload) {
    s32 status;

    status = File_Sync(upload->unk08, 1);
    if (status == 0) {
        File_Close(upload->unk08);
        model_read_file = -1;
        upload->stage++;
    } else if (status < 0) {
        File_Close(upload->unk08);
        upload->done = upload->stage = 0;
        model_read_file = -1;
    }
}

/* Stage 2 of a menu model's load: hands the model part of the file over and marks the node done. */
static void Menu_FinishModelLoad(ChunkUpload *upload) {
    LinkFile *file;

    file = upload->pool_node->buffer;
    PoolPartFile_Relocate((char *)file + file->first_offset);
    FlushCache(0);
    upload->done = 1;
    upload->stage = 0;
    upload->unk08 = -1;
}

/* the stages of a menu model's load: open, wait for the read, hand the model over */
static const ChunkUploadStages model_load_stages = { {
    (void (*)(void *))Menu_OpenModelFile,
    (void (*)(void *))Menu_WaitModelRead,
    (void (*)(void *))Menu_FinishModelLoad,
} };

/* The callback of a menu model's load node: runs the stage the node is at, until it is done. */
void Menu_StepModelLoad(LinkNode *node) {
    ChunkUploadStages stages = model_load_stages;

    if (node->unk20 == 0) {
        stages.stage[node->unk1C](node);
    }
}

/* Stage 0 of the worn models' load: opens the file of the node waiting (node->unk10) and starts reading it into
   the loader's buffer; a file that cannot be opened is skipped. */
static void Menu_OpenWornModelFile(LinkNode *node) {
    s32 file_handle;

    file_handle = File_OpenUnflagged(node->unk10->name, 1, 1);
    node->unk08 = file_handle;
    if (file_handle < 0) {
        LinkNode *next;

        node->unk08 = -1;
        node->unk1C = 0;
        next = node->unk10->next;
        worn_model_loading = 0;
        if (next != NULL) {
            node->unk10 = next;
            return;
        }
        node->unk20 = 1;
        return;
    }
    File_Read(file_handle, ((ChunkPoolNode *)node->pool_node)->buffer, 0x25800);
    model_read_file = node->unk08;
    node->unk1C += 1;
}

/* Stage 1 of an equipment model's load: as Stage 1 of a menu model's, and an error also frees the screen's input. */
static void Menu_WaitWornModelRead(ChunkUpload *upload) {
    s32 status;

    status = File_Sync(upload->unk08, 1);
    if (status == 0) {
        File_Close(upload->unk08);
        model_read_file = -1;
        upload->stage++;
    } else if (status < 0) {
        File_Close(upload->unk08);
        upload->done = upload->stage = 0;
        model_read_file = -1;
        worn_model_loading = 0;
    }
}

/* per slot of pool_part_sources, the pool group (PoolSlot_LoadGroup's second argument) its model goes into */
static const LinkSlotValues slot_pool_groups = { { 4, 0xFF, 5, 2, 1, 3, 0, 6, 7, 8, 0xFF, 0xFF } };

/* Last stage of a node's load: points its slot at the two parts of the file, gives the buffer to
   the node it was loaded for and goes on to the next waiting node. */
static void Menu_FinishWornModelLoad(LinkNode *node) {
    LinkSlotValues values = slot_pool_groups;
    LinkNode *target = node->unk10;
    LinkNode *loaded;
    LinkNode *next;
    LinkFile *file;
    void *pool_node;

    if ((u8)target->slot >= 13) {
        node->unk10 = NULL;
        node->unk20 = 1;
        return;
    }
    file = ((ChunkPoolNode *)node->pool_node)->buffer;
    pool_part_sources[target->slot]->texture = (char *)file + file->second_offset;
    pool_part_sources[target->slot]->file = (PoolPartFile *)((char *)file + file->first_offset);
    PoolPartFile_Relocate((char *)pool_part_sources[target->slot]->file);
    pool_node = node->unk10->pool_node;
    node->unk10->pool_node = node->pool_node;
    node->pool_node = pool_node;
    PoolSlot_LoadGroup(pool_part_sources[node->unk10->slot], values.value[node->unk10->slot]);
    loaded = node->unk10;
    node->unk1C = 0;
    node->unk08 = -1;
    loaded->unk20 = 1;
    worn_model_loading = 0;
    next = loaded->next;
    if (next != NULL) {
        node->unk10 = next;
    } else {
        node->unk20 = 1;
        node->unk10 = NULL;
    }
    loaded->next = NULL;
}

/* the stages of an equipment model's load: open, wait for the read, give the model to its slot */
static const ChunkUploadStages stream_load_stages = { {
    (void (*)(void *))Menu_OpenWornModelFile,
    (void (*)(void *))Menu_WaitWornModelRead,
    (void (*)(void *))Menu_FinishWornModelLoad,
} };

/* The callback of the node that loads the worn models (worn_model_loader): runs the stage it is at, until all are loaded. */
static void Menu_StepWornModelLoad(ChunkUpload *upload) {
    ChunkUploadStages handlers = stream_load_stages;

    if (upload->done == 0) {
        handlers.stage[upload->stage](upload);
    }
}

/* Stage 0 of a chunk upload: open the named file and start reading up to 0x1A2000 bytes. */
static void Menu_OpenPackageFile(ChunkUpload *upload) {
    s32 file_handle;

    file_handle = File_OpenUnflagged(upload->name, 1, 1);
    upload->unk08 = file_handle;
    if (file_handle < 0) {
        upload->stage = 0;
        upload->done = 0;
        upload = NULL; // match: retail goes on to read through this null pointer
    }
    File_Read(upload->unk08, upload->pool_node->buffer, 0x1A2000);
    model_read_file = upload->unk08;
    upload->stage++;
}

/* Stage 1 of a chunk upload: when the read has finished close the file and go on; on error start over. */
static void Menu_WaitPackageRead(ChunkUpload *upload) {
    s32 status;

    status = File_Sync(upload->unk08, 1);
    if (status == 0) {
        File_Close(upload->unk08);
        model_read_file = -1;
        upload->stage++;
    } else if (status < 0) {
        File_Close(upload->unk08);
        upload->done = upload->stage = 0;
        model_read_file = -1;
    }
}

/* Stage 2 of a package's load: hands every chunk of the package over, then marks the load done. */
static void Menu_FinishPackageLoad(ChunkUpload *upload) {
    u32 i;
    u32 *header;
    char *next;
    char *chunk;

    header = upload->pool_node->buffer;
    chunk = (char *)header + 0x10;
    for (i = 0; i < *header; i++) {
        next = chunk + ((int *)chunk)[3];
        PoolPartFile_Relocate(chunk + ((int *)chunk)[1]);
        chunk = next + ((int *)chunk)[2];
    }
    FlushCache(0);
    upload->done = 1;
    upload->stage = 0;
    upload->unk08 = -1;
}

/* the stages of a package's load: open, wait for the read, hand every chunk over */
static const ChunkUploadStages package_load_stages = { {
    (void (*)(void *))Menu_OpenPackageFile,
    (void (*)(void *))Menu_WaitPackageRead,
    (void (*)(void *))Menu_FinishPackageLoad,
} };

/* The callback of the package load node npc_package_load: runs the stage it is at, until it is done. */
static void Menu_StepPackageLoad(ChunkUpload *upload) {
    ChunkUploadStages stages = package_load_stages;

    if (upload->done == 0) {
        stages.stage[upload->stage](upload);
    }
}

/* Gives a load node a buffer from the free list, if one is left. Returns nothing. */
int Menu_TakeLoadBuffer(LinkNode *node) { // match: declared int in ui/link.h, returns nothing
    ChunkPoolNode *free_node = free_load_buffers;

    if (free_node != NULL) {
        node->pool_node = free_node;
        free_load_buffers = free_node->next;
    }
}

/* Puts a load node's buffer back on the free list. */
static void Menu_FreeLoadBuffer(MenuFreeRequest *request) {
    ChunkPoolNode *node = (ChunkPoolNode *)request->node;
    ChunkPoolNode *old_head = free_load_buffers;

    free_load_buffers = node;
    node->next = old_head;
}

/* Puts every load buffer back on the free list (a screen is left); a load still in progress is kept, moved to
   the spare node, with the node it was for marked as having no slot. */
void Menu_ResetLoadBuffers(void) {
    LinkNode *loading = model_load_chain;
    u32 i;

    free_load_buffers = load_buffer_nodes;
    for (i = 0; i < 15; i++) {
        load_buffer_nodes[i].next = &load_buffer_nodes[i + 1];
    }
    load_buffer_nodes[15].next = NULL;
    if (loading != NULL) {
        model_load_spare = *loading;
        model_load_chain = &model_load_spare;
        model_load_spare.next = NULL;
        if (model_load_spare.unk10 != NULL) {
            model_load_spare.unk10->slot = -1;
        }
    }
}

/* Writes the model file name of a menu item that has a definition (its LevelDef's file number). */
static void Menu_WriteItemModelName(char *buffer, MenuItem *item) {
    sprintf(buffer, "menu/select2/i%04d.mod", item_name_defs[item->unk0C].file_number);
}

/* Writes the model file name of a spell (group 1): its own number. */
static void Menu_WriteSpellModelName(char *buffer, MenuItem *item) {
    sprintf(buffer, "menu/select/m%04d.mod", item->unk0C);
}

/* A copy of Menu_WriteItemModelName, the entry of group 10 in item_model_name_writers; no item has that group. */
static void Menu_WriteGroup10ModelName(char *buffer, MenuItem *item) {
    sprintf(buffer, "menu/select2/i%04d.mod", item_name_defs[item->unk0C].file_number);
}

/* per MenuItem.group, the function that writes the item's model file name */
static const MenuNameHandlers item_model_name_writers = { {
    Menu_WriteItemModelName, Menu_WriteSpellModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName,
    Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteGroup10ModelName, Menu_WriteItemModelName,
} };

/* Writes the model file names of the five items on show: the current one, those of the next and
   previous categories, and the next and previous items of this category. */
static void Menu_SetEquipModelNames(void) {
    MenuNameHandlers handlers = item_model_name_writers;
    MenuItem *item;
    void (*handler)(char *buffer, MenuItem *item);

    item = equip_list_shown[0]->item;
    handler = handlers.handler[item->group];
    handler(model_name_shown, item);
    item = (MenuItem *)equip_list_shown[0]; // match: the category passes through the item's register
    item = ((MenuCategory *)item)->next->item;
    handler = handlers.handler[item->group];
    handler(model_name_up, item);
    item = (MenuItem *)equip_list_shown[0]; // match: as above
    item = ((MenuCategory *)item)->prev->item;
    handler = handlers.handler[item->group];
    handler(model_name_down, item);
    item = (MenuItem *)equip_list_shown[0]; // match: as above
    item = ((MenuCategory *)item)->item->next;
    handler = handlers.handler[item->group];
    handler(model_name_left, item);
    item = (MenuItem *)equip_list_shown[0]; // match: as above
    item = ((MenuCategory *)item)->item->prev;
    handler = handlers.handler[item->group];
    handler(model_name_right, item);
}

/* Writes the model file names of the player's weapon and the eight things worn into their upload nodes. */
static void Menu_SetWornModelNames(void) {
    sprintf(weapon_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].file_number);
    sprintf(shield_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST].file_number);
    sprintf(armor_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST].file_number);
    sprintf(helm_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST].file_number);
    sprintf(gauntlet_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST].file_number);
    sprintf(boot_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST].file_number);
    sprintf(ring_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST].file_number);
    sprintf(bracelet_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST].file_number);
    sprintf(amulet_model_name, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST].file_number);
    worn_model_loader.unk20 = 0;
}

/* Sets up the loads of the player's equipment models (nine nodes behind worn_model_loader) and of the five
   menu item models, and chains them all for the loader. */
// match: the order of the statements decides which node address gets which register (and worn_model_loader gets none)
static void Menu_ChainEquipModels(void) {
    Menu_TakeLoadBuffer(&worn_model_loader);
    Menu_TakeLoadBuffer(&weapon_model_load);
    Menu_TakeLoadBuffer(&shield_model_load);
    Menu_TakeLoadBuffer(&armor_model_load);
    Menu_TakeLoadBuffer(&helm_model_load);
    Menu_TakeLoadBuffer(&gauntlet_model_load);
    Menu_TakeLoadBuffer(&boot_model_load);
    Menu_TakeLoadBuffer(&ring_model_load);
    Menu_TakeLoadBuffer(&bracelet_model_load);
    Menu_TakeLoadBuffer(&amulet_model_load);
    Menu_SetWornModelNames();
    shield_model_load.slot = 2;
    armor_model_load.slot = 4;
    helm_model_load.slot = 3;
    gauntlet_model_load.slot = 5;
    boot_model_load.slot = 6;
    ring_model_load.slot = 7;
    bracelet_model_load.slot = 8;
    amulet_model_load.slot = 9;
    worn_model_loader.callback = (void (*)(void))Menu_StepWornModelLoad;
    weapon_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    shield_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    armor_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    helm_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    gauntlet_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    boot_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    ring_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    bracelet_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    amulet_model_load.callback = (void (*)(void))Menu_StepWornModelLoad;
    weapon_model_load.slot = 0;
    weapon_model_load.unk1C = 0;
    shield_model_load.unk1C = 0;
    armor_model_load.unk1C = 0;
    helm_model_load.unk1C = 0;
    gauntlet_model_load.unk1C = 0;
    boot_model_load.unk1C = 0;
    ring_model_load.unk1C = 0;
    bracelet_model_load.unk1C = 0;
    amulet_model_load.unk1C = 0;
    worn_model_loader.unk10 = &shield_model_load;
    shield_model_load.next = &weapon_model_load;
    weapon_model_load.next = &armor_model_load;
    worn_model_loader.unk1C = 0;
    worn_model_loader.unk20 = 0;
    armor_model_load.next = &helm_model_load;
    helm_model_load.next = &gauntlet_model_load;
    gauntlet_model_load.next = &boot_model_load;
    boot_model_load.next = &ring_model_load;
    ring_model_load.next = &bracelet_model_load;
    bracelet_model_load.next = &amulet_model_load;
    amulet_model_load.next = NULL;
    Menu_SetEquipModelNames();
    Menu_TakeLoadBuffer(&model_load_shown);
    Menu_TakeLoadBuffer(&model_load_up);
    Menu_TakeLoadBuffer(&model_load_down);
    Menu_TakeLoadBuffer(&model_load_left);
    Menu_TakeLoadBuffer(&model_load_right);
    model_load_shown.next = &model_load_up;
    model_load_shown.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_up.next = &model_load_down;
    model_load_up.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_down.next = &model_load_left;
    model_load_down.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_left.next = &model_load_right;
    model_load_left.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_right.next = NULL;
    model_load_right.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_shown.unk20 = 0;
    model_load_up.unk20 = 0;
    model_load_down.unk20 = 0;
    model_load_left.unk20 = 0;
    model_load_right.unk20 = 0;
    model_load_up.unk1C = 0;
    model_load_down.unk1C = 0;
    model_load_left.unk1C = 0;
    model_load_shown.unk1C = model_load_right.unk1C = 0;
    model_load_chain = &worn_model_loader;
    worn_model_loader.next = &model_load_shown;
}

/* Writes the model file names of the five items on show in a long list: the current one, its two
   neighbours, and the items seven steps on either way (or the list's ends when nearer). */
static void Menu_SetAllItemsModelNames(void) {
    MenuItem *ahead;
    MenuItem *behind;
    s32 i;

    ahead = behind = owned_list_top[0];
    if (owned_list_count >= 8) {
        for (i = 0; i < 7; i++) {
            ahead = ahead->next;
            behind = behind->prev;
            if (ahead == owned_list_first) {
                i++;
                while (i < 7 && behind != owned_list_last_page) {
                    i++;
                    behind = behind->prev;
                }
                break;
            } else if (behind == owned_list_last_page) {
                i++;
                while (i < 7 && ahead != owned_list_first) {
                    i++;
                    ahead = ahead->next;
                }
                break;
            }
        }
    } else {
        ahead = behind = owned_list_first;
    }
    sprintf(model_name_shown, "menu/select2/i%04d.mod", item_name_defs[owned_list_current[0]->unk0C].file_number);
    sprintf(model_name_down, "menu/select2/i%04d.mod", item_name_defs[owned_list_current[0]->prev->unk0C].file_number);
    sprintf(model_name_up, "menu/select2/i%04d.mod", item_name_defs[owned_list_current[0]->next->unk0C].file_number);
    sprintf(model_name_left, "menu/select2/i%04d.mod", item_name_defs[ahead->unk0C].file_number);
    sprintf(model_name_right, "menu/select2/i%04d.mod", item_name_defs[behind->unk0C].file_number);
}

/* Opens main menu line 5: names the five models of the list of everything owned and chains their loads. */
static void Menu_ChainAllItemsModels(void) {
    if (owned_list_current[0] != 0) {
        Menu_SetAllItemsModelNames();
        Menu_TakeLoadBuffer(&model_load_shown);
        Menu_TakeLoadBuffer(&model_load_up);
        Menu_TakeLoadBuffer(&model_load_down);
        Menu_TakeLoadBuffer(&model_load_left);
        Menu_TakeLoadBuffer(&model_load_right);
        model_load_shown.next = &model_load_up;
        model_load_shown.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_shown.unk20 = 0;
        model_load_up.next = &model_load_down;
        model_load_up.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_up.unk20 = 0;
        model_load_up.unk1C = 0;
        model_load_down.next = &model_load_left;
        model_load_down.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_down.unk20 = 0;
        model_load_down.unk1C = 0;
        model_load_left.next = &model_load_right;
        model_load_left.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_left.unk20 = 0;
        model_load_left.unk1C = 0;
        model_load_right.next = NULL;
        model_load_right.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_right.unk20 = 0;
        model_load_shown.unk1C = model_load_right.unk1C = 0;
        model_load_chain = &model_load_shown;
    }
}

/* Writes the model file names of the three spells on show: the one the cursor is on and its two neighbours. */
static void Menu_SetSpellModelNames(void) {
    sprintf(model_name_shown, "menu/select/m%04d.mod", castable_spell_cursor[0]->unk0C);
    sprintf(model_name_down, "menu/select/m%04d.mod", castable_spell_cursor[0]->prev->unk0C);
    sprintf(model_name_up, "menu/select/m%04d.mod", castable_spell_cursor[0]->next->unk0C);
}

/* Opens main menu line 1: names the three spell models and chains the load nodes (nodes 3 and 4 apart, unused). */
static void Menu_ChainSpellModels(void) {
    Menu_SetSpellModelNames();
    Menu_TakeLoadBuffer(&model_load_shown);
    Menu_TakeLoadBuffer(&model_load_up);
    Menu_TakeLoadBuffer(&model_load_down);
    Menu_TakeLoadBuffer(&model_load_left);
    Menu_TakeLoadBuffer(&model_load_right);
    model_load_shown.next = &model_load_up;
    model_load_shown.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_up.next = &model_load_down;
    model_load_up.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_down.next = NULL;
    model_load_down.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_left.next = &model_load_right;
    model_load_left.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_right.next = NULL;
    model_load_right.callback = (void (*)(void))Menu_StepModelLoad;
    model_load_shown.unk20 = 0;
    model_load_up.unk20 = 0;
    model_load_down.unk20 = 0;
    model_load_left.unk20 = 0;
    model_load_right.unk20 = 0;
    model_load_up.unk1C = 0;
    model_load_down.unk1C = 0;
    model_load_left.unk1C = 0;
    model_load_shown.unk1C = model_load_right.unk1C = 0;
    model_load_chain = &model_load_shown;
}

/* the LevelDef numbers of the twelve key items, in the order of the key item screen */
static const KeyItemNumbers key_item_numbers = { { 49, 48, 47, 52, 50, 58, 57, 56, 55, 51, 59, 53 } };
/* twelve more numbers that Menu_ChainKeyItemModels copies and never reads */
static const KeyItemNumbers key_item_unused_numbers = { { 5, 35, 36, 53, 80, 88, 97, 98, 100, 101, 102, 103 } };

/* Sets up the loads of the twelve key item models (the model of each one held, a stand-in file
   for the others) and chains their nodes behind the node waiting. */
static void Menu_ChainKeyItemModels(void) {
    KeyItemNumbers numbers = key_item_numbers;
    KeyItemNumbers unused_numbers = key_item_unused_numbers; // match: a second table is copied and never read
    s32 i;

    for (i = 0; i < 12; i++) {
        Menu_TakeLoadBuffer(&key_item_model_loads[i]);
        key_item_model_loads[i].callback = (void (*)(void))Menu_StepModelLoad;
        key_item_model_loads[i].unk1C = 0;
        key_item_model_loads[i].unk20 = 0;
        if (item_counts[numbers.number[i]] != 0) {
            sprintf(key_item_model_loads[i].name, "menu/select/i%04d.mod", item_name_defs[numbers.number[i]].file_number);
        } else {
            sprintf(key_item_model_loads[i].name, "menu/equip/i7000.mod");
        }
    }
    if (model_load_chain != NULL) {
        model_load_chain->next = &key_item_model_loads[0];
    } else {
        model_load_chain = &key_item_model_loads[0];
    }
    for (i = 10; i >= 0; i--) {
        key_item_model_loads[i].next = &key_item_model_loads[i + 1];
    }
    key_item_model_loads[11].next = NULL;
}

/* The spell cursor went on one: the model shown moves to node 2, node 1's becomes the one shown, node 1 is
   made fresh, and the three are chained again. */
static void Menu_ShiftSpellModelsNext(int unused) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (model_load_chain != &model_load_down) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_down);
    model_load_down = model_load_shown;
    node = &model_load_down;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &model_load_up; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    model_load_shown = model_load_up;
    done = model_load_shown.unk20;
    model_load_shown.next = NULL;
    if (done == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_up);
    if (pending == NULL) {
        pending = &model_load_up;
    }
    model_load_up.unk08 = -1;
    model_load_up.unk1C = 0;
    model_load_up.next = NULL;
    model_load_up.unk20 = 0;
    current = model_load_chain;
    if (current != NULL) {
        current->next = pending;
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = NULL;
}

/* The spell cursor went back one: the model shown moves to node 1, node 2's becomes the one shown, node 2 is
   made fresh, and the three are chained again. */
static void Menu_ShiftSpellModelsPrev(int unused) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (model_load_chain != &model_load_up) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_up);
    model_load_up = model_load_shown;
    node = &model_load_up;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &model_load_down; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    model_load_shown = model_load_down;
    done = model_load_shown.unk20;
    model_load_shown.next = NULL;
    if (done == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_down);
    if (pending == NULL) {
        pending = &model_load_down;
    }
    model_load_down.unk08 = -1;
    model_load_down.unk1C = 0;
    model_load_down.next = NULL;
    model_load_down.unk20 = 0;
    current = model_load_chain;
    if (current != NULL) {
        current->next = pending;
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_down;
    model_load_down.next = &model_load_up;
    model_load_up.next = NULL;
}

/* Allocates the sixteen 0x25800-byte load buffers, builds their free list and sets up the package load node. */
void Menu_InitLoadBuffers(void) {
    u32 i;

    MenuArena_Init(packet_area);
    free_load_buffers = load_buffer_nodes;
    for (i = 0; i < 15; i++) {
        load_buffer_nodes[i].next = &load_buffer_nodes[i + 1];
        load_buffer_nodes[i].buffer = (void *)MenuArena_Alloc(0x25800, 0x10);
    }
    load_buffer_nodes[15].next = NULL;
    load_buffer_nodes[15].buffer = (void *)MenuArena_Alloc(0x25800, 0x10);
    npc_package_load.pool_node = load_buffer_nodes;
    npc_package_load.callback = Menu_StepPackageLoad;
    npc_package_load.stage = 0;
    npc_package_load.done = 0;
    sprintf(npc_package_load.name, "menu/select/npc.mod");
}

/* Loads a file of sprite parts (0x60-byte records behind a 0x10-byte header); gives back how many and where. */
static void Menu_LoadPartFile(char *name, u32 *record_count, SpritePacket **records) {
    u32 *file_data;

    file_data = MenuHeap_Alloc(0x2800);
    if (MenuFile_Load(name, file_data) != 0) {
        *record_count = (*file_data - 0x10) / 0x60;
        if (*record_count == 0) {
            *records = NULL;
        } else {
            file_data += 4;
            *records = (SpritePacket *)file_data;
        }
    }
}

/* Loads the 36 part files of the menu, trade and smith screens. */
static void Menu_LoadAllParts(void) {
    Menu_LoadPartFile("menu/main_para.dat", &main_part_count, &main_parts);
    Menu_LoadPartFile("menu/item_para.dat", &item_part_count, &item_parts);
    Menu_LoadPartFile("menu/magic_para.dat", &magic_part_count, &magic_parts);
    Menu_LoadPartFile("menu/equip_para.dat", &equip_part_count, &equip_parts);
    Menu_LoadPartFile("menu/equip_a_para.dat", &equip_a_part_count, &equip_a_parts);
    Menu_LoadPartFile("menu/equip_g_para.dat", &equip_g_part_count, &equip_g_parts);
    Menu_LoadPartFile("menu/equip_m_para.dat", &equip_m_part_count, &equip_m_parts);
    Menu_LoadPartFile("menu/equip_s_para.dat", &equip_s_part_count, &equip_s_parts);
    Menu_LoadPartFile("menu/equip_g2_para.dat", &equip_g2_part_count, &equip_g2_parts);
    Menu_LoadPartFile("menu/status_para.dat", &status_part_count, &status_parts);
    Menu_LoadPartFile("menu/keylist_para.dat", &keylist_part_count, &keylist_parts);
    Menu_LoadPartFile("menu/image_para.dat", &image_part_count, &image_parts);
    Menu_LoadPartFile("menu/recol_para.dat", &recol_part_count, &recol_parts);
    Menu_LoadPartFile("menu/recol_a_para.dat", &recol_a_part_count, &recol_a_parts);
    Menu_LoadPartFile("menu/recol_g_para.dat", &recol_g_part_count, &recol_g_parts);
    Menu_LoadPartFile("menu/recol_g2_para.dat", &recol_g2_part_count, &recol_g2_parts);
    Menu_LoadPartFile("menu/item_sel_para.dat", &item_sel_part_count, &item_sel_parts);
    Menu_LoadPartFile("menu/system_para.dat", &system_part_count, &system_parts);
    Menu_LoadPartFile("menu/option_para.dat", &option_part_count, &option_parts);
    Menu_LoadPartFile("menu/keyconf_para.dat", &keyconf_part_count, &keyconf_parts);
    Menu_LoadPartFile("menu/preshop_para.dat", &preshop_part_count, &preshop_parts);
    Menu_LoadPartFile("menu/shop_para.dat", &shop_part_count, &shop_parts);
    Menu_LoadPartFile("menu/shop_a_para.dat", &shop_a_part_count, &shop_a_parts);
    Menu_LoadPartFile("menu/shop_g_para.dat", &shop_g_part_count, &shop_g_parts);
    Menu_LoadPartFile("menu/shop_g2_para.dat", &shop_g2_part_count, &shop_g2_parts);
    Menu_LoadPartFile("menu/pretune_para.dat", &pretune_part_count, &pretune_parts);
    Menu_LoadPartFile("menu/tune_para.dat", &tune_part_count, &tune_parts);
    Menu_LoadPartFile("menu/tune_s_para.dat", &tune_s_part_count, &tune_s_parts);
    Menu_LoadPartFile("menu/slot_para.dat", &slot_part_count, &slot_parts);
    Menu_LoadPartFile("menu/data_para.dat", &data_part_count, &data_parts);
    Menu_LoadPartFile("menu/mc_para.dat", &mc_part_count, &mc_parts);
    Menu_LoadPartFile("menu/bright_para.dat", &bright_part_count, &bright_parts);
    Menu_LoadPartFile("menu/shop_mess_para.dat", &shop_mess_part_count, &shop_mess_parts);
    Menu_LoadPartFile("menu/tune_mess_para.dat", &tune_mess_part_count, &tune_mess_parts);
    Menu_LoadPartFile("menu/mc_mess_para.dat", &mc_mess_part_count, &mc_mess_parts);
    Menu_LoadPartFile("menu/equip_m2_para.dat", &equip_m2_part_count, &equip_m2_parts);
}

/* Sets the menus up: the frame buffers, the menu and font textures, the twenty equipment pictures, then every
   list of menu items (one MenuItem per definition, numbered) and the ring of the eleven lists. */
void Menu_Init(void) {
    char name[0x40];
    char *menu_file;
    s32 i;
    s32 number;

    sceGsResetGraph(0, 1, 2, 1);
    sceGsSetDefDBuffDc(&menu_frame_buffers, 0, 0x280, 0xE0, 3, 0x30, 1);
    menu_frame_buffers.clear1_rgbaq = menu_frame_buffers.clear0_rgbaq = 0x180000000L;
    MenuHeap_Reset();
    Menu_LoadAllParts();
    menu_file = MenuHeap_Alloc(0xE0000);
    MenuFile_Load("menu/menu.tmx", menu_file);
    MenuFile_IndexBlocks(menu_file);
    MenuHeap_Mark();
    menu_font_texture.header = MenuHeap_Alloc(0x23000);
    MenuFile_Load("menu/font.tmr", menu_font_texture.header);
    MenuTexture_GetRegisters(menu_font_texture.header, &menu_font_texture.tex0, &menu_font_texture.trailer);
    MenuHeap_FreeToMark();
    for (i = 0; i < 20; i++) {
        equip_weapon_motions[i] = MenuHeap_Alloc(0x2800);
        sprintf(name, "menu/equip/a%03d.mi2", i);
        MenuFile_Load(name, equip_weapon_motions[i]);
        PoolNodeFile_Relocate((RelocFile *)equip_weapon_motions[i]);
    }
    for (i = 0; i < 150; i++) {
        shortcut_entries[i].unk0C = i;
        shortcut_entries[i].unk00 = (int)&item_common_defs[i];
        shortcut_entries[i].group = 11;
    }
    for (i = 150; i < 241; i++) {
        shortcut_entries[i].unk00 = (int)&fx_table[i - 150];
        shortcut_entries[i].unk0C = i - 150;
        shortcut_entries[i].group = 1;
    }
    number = 0;
    for (i = 0; i < 150; i++) {
        all_item_entries[number].unk00 = (int)&item_common_defs[i];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 12;
        number++;
    }
    for (i = 0; i < 70; i++) {
        all_item_entries[number].unk00 = (int)&weapon_defs[i];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 0;
        number++;
    }
    for (i = 0; i < 30; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 2;
        number++;
    }
    for (i = 0; i < 25; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i + 30];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 3;
        number++;
    }
    for (i = 0; i < 25; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i + 55];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 4;
        number++;
    }
    for (i = 0; i < 25; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i + 80];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 5;
        number++;
    }
    for (i = 0; i < 25; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i + 105];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 6;
        number++;
    }
    for (i = 0; i < 10; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i + 130];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 7;
        number++;
    }
    for (i = 0; i < 10; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i + 140];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 8;
        number++;
    }
    for (i = 0; i < 10; i++) {
        all_item_entries[number].unk00 = (int)&worn_defs[i + 130];
        all_item_entries[number].unk0C = number;
        all_item_entries[number].group = 9;
        number++;
    }
    number = 0;
    for (i = 0; i < 150; i++) {
        common_item_entries[i].unk00 = (int)&item_common_defs[i];
        common_item_entries[i].unk0C = number;
        common_item_entries[i].group = 11;
        number++;
    }
    for (i = 0; i < 70; i++) {
        weapon_entries[i].unk00 = (int)&weapon_defs[i];
        weapon_entries[i].unk0C = number;
        weapon_entries[i].group = 0;
        number++;
    }
    for (i = 0; i < 30; i++) {
        shield_entries[i].unk00 = (int)&worn_defs[i];
        shield_entries[i].unk0C = number;
        shield_entries[i].group = 2;
        number++;
    }
    for (i = 0; i < 25; i++) {
        helm_entries[i].unk00 = (int)&worn_defs[i + 30];
        helm_entries[i].unk0C = number;
        helm_entries[i].group = 3;
        number++;
    }
    for (i = 0; i < 25; i++) {
        armor_entries[i].unk00 = (int)&worn_defs[i + 55];
        armor_entries[i].unk0C = number;
        armor_entries[i].group = 4;
        number++;
    }
    for (i = 0; i < 25; i++) {
        gauntlet_entries[i].unk00 = (int)&worn_defs[i + 80];
        gauntlet_entries[i].unk0C = number;
        gauntlet_entries[i].group = 5;
        number++;
    }
    for (i = 0; i < 25; i++) {
        boot_entries[i].unk00 = (int)&worn_defs[i + 105];
        boot_entries[i].unk0C = number;
        boot_entries[i].group = 6;
        number++;
    }
    for (i = 0; i < 10; i++) {
        ring_entries[i].unk00 = (int)&worn_defs[i + 130];
        ring_entries[i].unk0C = number;
        ring_entries[i].group = 7;
        number++;
    }
    for (i = 0; i < 10; i++) {
        bracelet_entries[i].unk00 = (int)&worn_defs[i + 140];
        bracelet_entries[i].unk0C = number;
        bracelet_entries[i].group = 8;
        number++;
    }
    for (i = 0; i < 10; i++) {
        amulet_entries[i].unk00 = (int)&worn_defs[i + 130];
        amulet_entries[i].unk0C = number;
        amulet_entries[i].group = 9;
        number++;
    }
    for (i = 0; i < 91; i++) {
        spell_entries[i].unk0C = i;
        spell_entries[i].unk00 = (int)&fx_table[i];
        spell_entries[i].group = 1;
    }
    use_item_list.group = 11;
    equip_weapon_list.item = equipped_weapon.item;
    equip_weapon_list.group = 0;
    equip_shield_list.item = equipped_shield.item;
    equip_shield_list.group = 2;
    equip_helm_list.item = equipped_helm.item;
    equip_helm_list.group = 3;
    equip_armor_list.item = equipped_armor.item;
    equip_armor_list.group = 4;
    equip_gauntlet_list.item = equipped_gauntlet.item;
    equip_gauntlet_list.group = 5;
    equip_boot_list.item = equipped_boot.item;
    equip_boot_list.group = 6;
    equip_ring_list.item = equipped_ring.item;
    equip_ring_list.group = 7;
    equip_bracelet_list.item = equipped_bracelet.item;
    equip_bracelet_list.group = 8;
    equip_amulet_list.item = equipped_amulet.item;
    equip_amulet_list.group = 9;
    equip_spell_list.item = equipped_spell.item;
    equip_spell_list.group = 1;
    equip_shortcut_list.item = equipped_shortcut.item;
    equip_shortcut_list.group = 10;
    equip_shortcut_list.prev = &equip_weapon_list;
    equip_weapon_list.prev = &equip_spell_list;
    equip_spell_list.prev = &equip_shield_list;
    equip_shield_list.prev = &equip_helm_list;
    equip_helm_list.prev = &equip_armor_list;
    equip_armor_list.prev = &equip_gauntlet_list;
    equip_gauntlet_list.prev = &equip_boot_list;
    equip_boot_list.prev = &equip_ring_list;
    equip_ring_list.prev = &equip_bracelet_list;
    equip_bracelet_list.prev = &equip_amulet_list;
    equip_amulet_list.prev = &equip_shortcut_list;
    equip_weapon_list.next = &equip_shortcut_list;
    equip_spell_list.next = &equip_weapon_list;
    equip_shield_list.next = &equip_spell_list;
    equip_helm_list.next = &equip_shield_list;
    equip_armor_list.next = &equip_helm_list;
    equip_gauntlet_list.next = &equip_armor_list;
    equip_boot_list.next = &equip_gauntlet_list;
    equip_ring_list.next = &equip_boot_list;
    equip_bracelet_list.next = &equip_ring_list;
    equip_amulet_list.next = &equip_bracelet_list;
    equip_shortcut_list.next = &equip_amulet_list;
    MenuHeap_Mark();
}

extern char sunday_name[8];
extern char monday_name[8];
extern char tuesday_name[8];
extern char wednesday_name[8];
extern char thursday_name[8];
extern char friday_name[8];
extern char saturday_name[8];
/* the seven weekday names (Sunday first, in Japanese); copied to a local and never read */
static const MenuWeekdayNames weekday_names_table = { {
    sunday_name, monday_name, tuesday_name, wednesday_name, thursday_name, friday_name, saturday_name,
} };

/* Runs the menu from opening to closing: sets up, calls the handler menu_run_step names every frame until it gives 0,
   then shows the last frames. `mode` (menu_mode) is which menu; with mode 1 the result is 1 when
   card_screen_state.unk16 was set. */
int Menu_Run(s8 mode) { // match: callers declare the parameter int; retail sign-extends a byte here
    ClockTime clock;
    int tm_fields[9];
    MenuWeekdayNames weekday_names;
    u32 *packet;
    s32 status; // match: one variable for the card state in the loop and the menu number after it

    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    Vram_Clear(0x3D00, 0x3F00);
    Sound_MutePositional();
    Sound_UpdateTagged();
    Sound_MuteTracked();
    Sound_MuteTagged();
    sceCdReadClock((struct sceCdCLOCK *)&clock);
    weekday_names = weekday_names_table; // match: never read
    // match: the clock bytes are BCD and are used as they are, the day is lowered by one, and the result is not used
    tm_fields[0] = clock.sec;
    tm_fields[1] = clock.min;
    tm_fields[2] = clock.hour;
    tm_fields[3] = clock.mday - 1;
    tm_fields[4] = clock.mon - 1;
    tm_fields[5] = clock.year + 100;
    mktime(tm_fields);
    menu_mode = mode;
    pad_mode = pad_mode_before = Pad_GetMode(0, 0);
    while (SoundDriver_FlushCommands(), menu_run_steps[menu_run_step]() != 0) {
        sceGsSyncPath(0, 0);
        menu_even_field = Menu_EndFrame(1);
        Input_Update();
        status = Pad_GetMode(0, 0);
        pad_mode = status;
        if (status != pad_mode_before) {
            pad_mode_change_frames = 1;
        }
        pad_mode_before = status;
        MenuPad_Update();
        if (pad_mode_change_frames > 0) {
            pad_mode_change_frames++;
            if (pad_mode_change_frames > 20) {
                pad_mode_change_frames = 0;
            } else {
                menu_buttons = 0;
            }
        }
    }
    if (menu_mode == 0 && card_screen_state.save_loaded == 0) {
        packet = MenuDraw_NextPacket();
        packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
        packet[2] = 0xE;
        packet[3] = 0;
        *(u64 *)&packet[4] = 0x66802B240;
        packet[6] = 6;
        packet[7] = 0;
        *(u64 *)&packet[8] = game_dbuff_draw0.frame;
        packet[10] = 0x4C;
        packet[11] = 0;
        packet[12] = 0;
        packet[13] = 1;
        packet[14] = 0x4E;
        packet[15] = 0;
        packet[16] = 0x30000;
        packet[17] = 0;
        packet[18] = 0x47;
        packet[19] = 0;
        packet[20] = 0;
        packet[21] = 0;
        packet[22] = 0x18;
        packet[23] = 0;
        packet[24] = 0;
        packet[25] = 0;
        packet[26] = 2;
        packet[27] = 0;
        packet[28] = 0;
        packet[29] = 0;
        packet[30] = 5;
        packet[31] = 0;
        *(f32 *)&packet[32] = 0.625f;
        *(f32 *)&packet[33] = 0.4375f;
        packet[34] = 2;
        packet[35] = 0;
        packet[36] = 0x1C002800;
        packet[37] = 0;
        packet[38] = 5;
        packet[39] = 0;
        packet[40] = 0;
        packet[41] = 0;
        packet[42] = 0x3F;
        packet[43] = 0;
        packet[0] = 0x800A;
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
        *D2_QWC = 11;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = D_CHCR_STR;
        context2_buffer_half = context1_buffer_half = 1;
    }
    menu_run_step = 0;
    Sound_UnmuteTagged();
    Sound_UnmutePositional();
    Sound_UnmuteTracked();
    status = menu_mode;
    if (status == 0) {
        sceGsSyncPath(0, 0);
        while (sceGsSyncV(0) != 0);
    }
    switch (menu_mode) {
    case 2:
    case 3:
    case 4:
    case 5:
        card_screen_state.save_loaded = 0;
        sceGsSyncPath(0, 0);
        Menu_EndFrame(0);
        sceGsSyncV(0);
        sceGsSyncPath(0, 0);
        Menu_EndFrame(0);
        sceGsSyncV(0);
        return 0;
    case 0:
        if (card_screen_state.save_loaded != 0) {
            card_screen_state.save_loaded = 0;
            sceGsSyncPath(0, 0);
            Menu_EndFrame(0);
            sceGsSyncV(0);
            sceGsSyncPath(0, 0);
            Menu_EndFrame(0);
            sceGsSyncV(0);
        }
        return 0;
    case 1:
        MenuHeap_FreeToMark();
        sceGsSyncPath(0, 0);
        Menu_EndFrame(0);
        sceGsSyncV(0);
        sceGsSyncPath(0, 0);
        Menu_EndFrame(0);
        sceGsSyncV(0);
        if (card_screen_state.save_loaded != 0) {
            card_screen_state.save_loaded = 0;
            return 1;
        }
        return 0;
    }
}

extern char equip_screen_name[8];
/* the steps of a menu's start, run by Menu_RunStep until one gives 0 */
static int (*const menu_steps[4])(void) = {
    Menu_Start,
    Menu_RunHandler,
    Menu_CutModelChain,
    NULL,
};
/* the screen each line of the main menu opens */
static void *const main_menu_screens[8] = {
    Menu_UpdateUseItem, Menu_UpdateCastSpell, Menu_UpdateEquip, Menu_UpdateStatus, Menu_UpdateRecollection, Menu_UpdateVisibleItem, Menu_UpdateSystem, NULL,
};
/* the screen each line of the system menu opens */
static void *const system_menu_screens[6] = {
    Menu_UpdateCardSlot, Menu_UpdateOptions, Menu_UpdateButtonConfig, Menu_UpdateBrightness, Menu_WaitForCancel, NULL,
};
/* the names of the main menu's seven screens; nothing reads them */
const char *const main_menu_screen_names[8] = {
    "Use Item", "Cast a Spell", equip_screen_name, "Status Viewer", "Recollection", "Visible Item", "System Config", NULL,
};

/* Sends a fifteen-quadword packet of GS settings around one flat sprite. Nothing calls it. */
static void Menu_SendUnusedFlatSprite(void) {
    GsPacketWord *packet;
    GsPacketWord *cursor;

    packet = (GsPacketWord *)MenuDraw_NextPacket();
    cursor = packet;
    cursor++;
    (cursor++)->as_int = 0x10000000;
    (cursor++)->as_int = 0xE;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x40112;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x4D;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xA000132;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x4F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x7800;
    (cursor++)->as_int = 0x7800;
    (cursor++)->as_int = 0x19;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xFF000F;
    (cursor++)->as_int = 0xFF000E;
    (cursor++)->as_int = 0x41;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x30000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x48;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xA9;
    (cursor++)->as_int = 0x60;
    (cursor++)->as_int = 0x43;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x246;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x80000000;
    (cursor++)->as_float = 1.0f;
    (cursor++)->as_int = 0x1;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x77F077F0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x5;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x98108810;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x5;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x50000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x48;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x43;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xF0000F;
    (cursor++)->as_int = 0xF2000E;
    (cursor++)->as_int = 0x41;
    (cursor++)->as_int = 0;
    packet[0].as_int = 0x8000 | ((cursor - packet) / 4 - 1);
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = D_CHCR_STR;
}

/* Sends a fifteen-quadword packet: GS settings, one textured sprite, GS settings again. Nothing calls it. */
static void Menu_SendUnusedSpriteFirst(void) {
    GsPacketWord *packet;
    GsPacketWord *cursor;

    packet = (GsPacketWord *)MenuDraw_NextPacket();
    cursor = packet;
    cursor++;
    (cursor++)->as_int = 0x10000000;
    (cursor++)->as_int = 0xE;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x30000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x47;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x68;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x56;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    *(u64 *)cursor = 0x620012240;
    cursor += 2;
    (cursor++)->as_int = 6;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x80202020;
    (cursor++)->as_float = 1.0f;
    (cursor++)->as_int = 1;
    (cursor++)->as_int = 0;
    cursor[1].as_float = cursor[0].as_float = 0.01f;
    cursor += 2;
    (cursor++)->as_int = 2;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x7B708310;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 5;
    (cursor++)->as_int = 0;
    cursor[1].as_float = cursor[0].as_float = 0.99f;
    cursor += 2;
    (cursor++)->as_int = 2;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x83709310;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 5;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x50000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x47;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    packet[0].as_int = 0x8000 | ((cursor - packet) / 4 - 1);
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = D_CHCR_STR;
}

/* As Menu_SendUnusedSpriteFirst, with the sprite at another place. Nothing calls it. */
static void Menu_SendUnusedSpriteSecond(void) {
    GsPacketWord *packet;
    GsPacketWord *cursor;

    packet = (GsPacketWord *)MenuDraw_NextPacket();
    cursor = packet;
    cursor++;
    (cursor++)->as_int = 0x10000000;
    (cursor++)->as_int = 0xE;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x30000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x47;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x68;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x56;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    *(u64 *)cursor = 0x620012240;
    cursor += 2;
    (cursor++)->as_int = 6;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x80202020;
    (cursor++)->as_float = 1.0f;
    (cursor++)->as_int = 1;
    (cursor++)->as_int = 0;
    cursor[1].as_float = cursor[0].as_float = 0.01f;
    cursor += 2;
    (cursor++)->as_int = 2;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x7BC06E80;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 5;
    (cursor++)->as_int = 0;
    cursor[1].as_float = cursor[0].as_float = 0.99f;
    cursor += 2;
    (cursor++)->as_int = 2;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x83C07E80;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 5;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x50000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x47;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    packet[0].as_int = 0x8000 | ((cursor - packet) / 4 - 1);
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = D_CHCR_STR;
}

/* Draws the menu's item model with the matrix given: sends a three-quadword packet of GS settings,
   draws the model of node model_load_shown and puts the view-to-screen matrix back. */
int Menu_DrawItemModel(f32 *matrix) { // match: callers declare an int result; nothing is returned
    u32 *packet;

    packet = MenuDraw_NextPacket();
    packet[0] = 0x8002;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0x60F0;
    packet[5] = 0x7990;
    packet[6] = 0x19;
    packet[7] = 0;
    packet[8] = 0x44;
    packet[9] = 0;
    packet[10] = 0x43;
    packet[11] = 0;
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 3;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = D_CHCR_STR;
    DRAW_NODE_MODEL(&model_load_shown, matrix, menu_draw_color);
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
}

/* First step of Menu_Run's loop. In the game's menu (mode 0): copies the shown frame buffer into the drawn one in
   two half-brightness passes, as the backdrop. Returns 1. */
static s32 Menu_CopyFrameToTexture(void) {
    f32 unused[28]; // match: 0x70 bytes of locals nothing uses
    u32 texture_base; /* where the shown buffer is as a texture: 0x1180 when it is the second one */
    u32 *packet;

    if (menu_mode == 0) {
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
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = D_CHCR_STR;
    }
    sceGsSyncPath(0, 0);
    menu_run_step++;
    return 1;
}

/* Returns 1. Nothing calls it. */
static int Menu_ReturnTrue(void) {
    return 1;
}

/* Ends a menu frame: draws the brightness sprite, waits for the vertical blank, sets the half-line
   offset of the buffer drawn next, swaps the buffers and (when asked) runs the option's handler.
   Gives back whether the field just shown was the even one. */
s32 Menu_EndFrame(s32 run_handler) {
    s32 even_field;

    Menu_DrawBrightness(game_options[10]);
    SoundDriver_FlushCommands();
    sceGsSyncPath(0, 0);
    menu_frame_ticks = Timer_GetTicks();
    even_field = sceGsSyncV(0) == 0;
    Timer_Restart();
    if (menu_buttons & PAD_L2) {
        D_00447208++;
    }
    D_00447208 %= 3;
    sceGsSetHalfOffset(context1_buffer_half != 0 ? menu_second_draw_env : (char *)menu_first_draw_env, 0x800, 0x800, even_field);
    sceGsSetHalfOffset2(context1_buffer_half != 0 ? menu_second_draw_env2 : menu_first_draw_env2, 0x800, 0x800, even_field);
    FlushCache(0);
    sceGsSyncPath(0, 0);
    sceGsSwapDBuffDc(&menu_frame_buffers, context1_buffer_half);
    if (run_handler != 0) {
        frame_end_drawers[(s8)game_options[8]]();
    }
    context1_buffer_half = context1_buffer_half == 0;
    context2_buffer_half = context2_buffer_half == 0;
    return even_field;
}

/* The backdrop of option 8's first setting, in the game's menu only: the grabbed frame as a sprite, then a
   rectangle of the colour { 0x10, 0x10, 0x10 } over it. */
static void Menu_DrawBackdropPlain(void) {
    f32 unused[24];
    ColorBytes color = backdrop_shade_color[0];
    u32 *packet;

    if (menu_mode == 0) {
        packet = MenuDraw_NextPacket();
        packet[0] = 0x8001;
        packet[1] = 0x740B4000;
        packet[2] = 0xF5252160;
        packet[3] = 0;
        packet[4] = 0x16;
        packet[5] = 0;
        *(u64 *)&packet[6] = 0x66802B240;
        packet[8] = 0x80808080;
        *(f32 *)&packet[9] = 1.0f;
        packet[10] = 0;
        packet[11] = 0;
        packet[12] = 0x79006C00;
        packet[13] = 1;
        *(f32 *)&packet[14] = 0.625f;
        *(f32 *)&packet[15] = 0.4375f;
        packet[16] = 0x87009400;
        packet[17] = 1;
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
        *D2_QWC = 5;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = D_CHCR_STR;
        sceGsSyncPath(0, 0);
        packet = MenuDraw_NextPacket();
        packet[0] = 0x8001;
        packet[1] = 0x44000000;
        packet[2] = 0x5510;
        packet[3] = 0;
        packet[4] = 0x46;
        packet[5] = 0;
        packet[6] = color.red | (color.green << 8) | (color.blue << 16) | 0x40000000;
        packet[7] = 0;
        packet[8] = 0x79006C00;
        packet[9] = 2;
        packet[10] = 0x87009400;
        packet[11] = 2;
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = D_CHCR_STR;
    }
}

/* The backdrop of option 8's third setting: every kind 2 part of menu/item_sel_para.dat. That these are
   backdrops is a guess from where they are drawn. */
static void Menu_DrawBackdropItemSelParts(void) {
    u32 i;

    for (i = 0; i < item_sel_part_count; i++) {
        if (item_sel_parts[i].handler_index == 2) {
            Menu_SetBlendMode(item_sel_parts[i].blend_mode);
            MenuSprite_DrawPlain((int)&item_sel_parts[i], 2, 0);
            Menu_SetBlendMode(0);
        }
    }
}

/* The backdrop of option 8's second setting: every kind 2 part of menu/preshop_para.dat. */
static void Menu_DrawBackdropPreshopParts(void) {
    u32 i;

    for (i = 0; i < preshop_part_count; i++) {
        if (preshop_parts[i].handler_index == 2) {
            Menu_SetBlendMode(preshop_parts[i].blend_mode);
            MenuSprite_DrawPlain((int)&preshop_parts[i], 2, 0);
            Menu_SetBlendMode(0);
        }
    }
}

/* Second step of Menu_Run's loop: steps the model loads, then runs the menu step menu_step names.
   Returns what the step returns, 1 when there is none. */
static int Menu_RunStep(void) {
    int (*step)(char *);

    Menu_StepModelLoads(0);
    step = (int (*)(char *))menu_steps[menu_step]; // match: the steps take nothing, yet D_005CE0E0 is passed
    if (step != NULL) {
        return step(D_005CE0E0);
    }
    return 1;
}

/* Start-up step of a menu screen: loads its textures and blocks, then picks the handler for the screen menu_mode names. */
static s32 Menu_Start(void) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;

    Packet_SwapBuffers();
    func_001095D0();
    TextureTable_QueueAll((TextureTableHeader *)Message_GetFontTexture(menu_heap));
    GsDraw_QueueTexFlush();
    func_001095D8();
    Packet_SendChain();
    ((s32 (*)(void))Packet_WaitForSend)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < menu_file_block_count; i++) {
        block = &menu_file_blocks[i];
        MenuTexture_Upload(block->data, block);
    }
    MenuArena_Init(packet_area);
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
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = D_CHCR_STR;
    if (menu_mode == 0) {
        equip_group_shown = 0;
        Menu_BuildItemRings();
        Menu_InitLoadBuffers();
    }
    switch (menu_mode) {
    case 0:
        menu_screen_handler = (void *)Menu_OpenMain;
        break;
    case 1:
        menu_screen_handler = (void *)Menu_OpenCardSlot;
        break;
    case 2:
        menu_screen_handler = (void *)Menu_UpdateOptions;
        break;
    case 3:
        Menu_ReadButtonSettings();
        menu_screen_handler = (void *)Menu_UpdateButtonConfig;
        break;
    case 4:
        menu_screen_handler = (void *)Menu_OpenCardCheck;
        break;
    case 5:
        brightness_before = brightness_option[0];
        menu_screen_handler = (void *)Menu_UpdateBrightness;
        pulse_value_falling = 0;
        system_screen_state = 0;
        break;
    }
    pulse_sprite_alpha = 128.0f;
    menu_step++;
    pulse_sprite_falling = 0;
    menu_draw_color.blue = 0x80;
    menu_draw_color.green = 0x80;
    menu_draw_color.red = 0x80;
    menu_draw_color.alpha = 0;
    panel_text_color.blue = 0x80;
    panel_text_color.green = 0x80;
    panel_text_color.red = 0x80;
    panel_text_color.alpha = 0;
    model_draw_color.blue = 0x80;
    model_draw_color.green = 0x80;
    model_draw_color.red = 0x80;
    model_draw_color.alpha = 0;
    return TRUE;
}

/* Runs the current screen handler and reports that it ran. */
static s32 Menu_RunHandler(void) {
    ((void (*)(void))menu_screen_handler)();
    return TRUE;
}

/* Last step of a menu: in the game's menu waits for the model load in progress with the chain cut behind it, then
   takes the lists' cursors back as they were, closes the load's file and ends the menu (step 0, returns FALSE). */
static s32 Menu_CutModelChain(void) {
    LinkNode *node;

    switch (menu_mode) {
    case 0:
        node = model_load_chain;
        if (node == NULL) {
            equip_weapon_list.item = equipped_weapon.item;
            equip_shield_list.item = equipped_shield.item;
            equip_spell_list.item = equipped_spell.item;
            equip_helm_list.item = equipped_helm.item;
            equip_armor_list.item = equipped_armor.item;
            equip_gauntlet_list.item = equipped_gauntlet.item;
            equip_boot_list.item = equipped_boot.item;
            equip_ring_list.item = equipped_ring.item;
            equip_bracelet_list.item = equipped_bracelet.item;
            equip_amulet_list.item = equipped_amulet.item;
            Sound_LoadMgBank(equipped_spell.item->unk0C);
            File_Close(model_read_file);
            menu_step = 0;
            main_menu_line = 0;
            return FALSE;
        }
        node->next = NULL;
        if (node->unk10 != NULL) {
            node->unk10->next = NULL;
        }
        return TRUE;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        menu_step = 0;
        return FALSE;
    }
    // match: other steps return nothing (see the report's retail bugs)
}

/* Makes the main menu the current screen, with its parts faded out. */
static void Menu_OpenMain(void) {
    equip_model_shown = 0;
    pulse_sprite_alpha = 128.0f;
    pulse_sprite_falling = 0;
    menu_input_enabled = 0;
    memset(screen_part_alphas, 0, 0x400);
    memset(panel_part_alphas, 0, 0x400);
    menu_screen_handler = (void *)Menu_UpdateMain;
}

/* The main menu: moves the cursor over its seven lines, draws the parts, and fades out to the chosen screen. */
static void Menu_UpdateMain(HandlerContext *context) {
    u32 i;
    s32 line_count;
    u32 full_alpha;
    u8 kind;

    line_count = 7;
    if ((menu_buttons & PAD_UP) && menu_input_enabled != 0) {
        main_menu_line--;
        Sound_PlayMenu(1);
    } else if ((menu_buttons & PAD_DOWN) && menu_input_enabled != 0) {
        main_menu_line++;
        Sound_PlayMenu(1);
    } else if ((menu_buttons & PAD_CROSS) && menu_input_enabled != 0) {
        if ((Menu_OwnsAnyItem() == 0 && main_menu_line == 5) || (Menu_KnowsCastableSpell() == 0 && main_menu_line == 1)) {
            Sound_PlayMenu(3);
        } else {
            Sound_PlayMenu(0);
            main_menu_state = 2;
            menu_input_enabled = 0;
            for (i = 0; i < main_part_count; i++) {
                if (main_parts[i].handler_index == 1 && main_menu_line == main_parts[i].slot) {
                    screen_part_alphas[i] = pulse_sprite_alpha;
                }
            }
        }
    } else if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        menu_input_enabled = 0;
        main_menu_state = 3;
        Sound_PlayMenu(2);
        for (i = 0; i < main_part_count; i++) {
            if (main_parts[i].handler_index == 1 && main_menu_line == main_parts[i].slot) {
                screen_part_alphas[i] = pulse_sprite_alpha;
            }
        }
    }
    main_menu_line = (main_menu_line + 7) % line_count;

    for (i = 0; i < main_part_count; i++) {
        kind = main_parts[i].handler_index;
        Menu_SetBlendMode(main_parts[i].blend_mode);
        if (main_menu_state != 1) {
            MenuSprite_Draw((int)&main_parts[i], kind + 1000, main_menu_line, (int)screen_part_alphas[i]);
        } else {
            MenuSprite_Draw((int)&main_parts[i], kind, main_menu_line, (int)screen_part_alphas[i]);
        }
        Menu_SetBlendMode(0);
    }

    switch (main_menu_state) {
    case 0:
        for (i = 0; i < main_part_count; i++) {
            screen_part_alphas[i] += main_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > main_parts[i].alpha) {
                for (i = 0; i < main_part_count; i++) {
                    full_alpha = main_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                menu_input_enabled = main_menu_state = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < main_part_count; i++) {
            screen_part_alphas[i] -= main_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                screen_part_alphas[i] = 0.0f;
                if (main_parts[i].handler_index != 1 || main_menu_line != main_parts[i].slot) {
                    memset(screen_part_alphas, 0, 0x400);
                    if (main_menu_state == 2) {
                        if (Menu_OwnsAnyItem() == 0 && main_menu_line == 5) {
                            main_menu_state = 0;
                            break;
                        }
                        if (Menu_KnowsCastableSpell() != 0 || main_menu_line != 1) {
                            menu_screen_handler = (void *)Menu_OpenChosenScreen;
                            context->unk04 = main_menu_line;
                            switch (context->unk04) {
                            case 0:
                                ((void (*)(s32))Menu_ChainItemListModels)(0); // match: the callee takes nothing
                                break;
                            case 1:
                                ((void (*)(s32))Menu_ChainSpellModels)(0); // match: the callee takes nothing
                                break;
                            case 2:
                                ((void (*)(s32))Menu_ChainEquipModels)(0); // match: the callee takes nothing
                                break;
                            case 4:
                                if (model_load_chain != NULL) {
                                    model_load_chain->next = (LinkNode *)&npc_package_load;
                                } else {
                                    model_load_chain = (LinkNode *)&npc_package_load;
                                }
                                recollection_cursor = 0;
                                break;
                            case 5:
                                ((void (*)(s32))Menu_ChainAllItemsModels)(0); // match: the callee takes nothing
                                break;
                            }
                        }
                    } else if (main_menu_state == 3) {
                        context->unk04 = 0;
                        main_menu_line = 0;
                        menu_step++;
                    }
                    main_menu_state = 0;
                    break;
                }
            }
        }
        break;
    }
}

/* Makes the screen of the main menu line chosen (context->unk04) the current handler and clears the line. */
static void Menu_OpenChosenScreen(HandlerContext *context) {
    int *line = &context->unk04;

    pulse_sprite_alpha = 128.0f;
    menu_screen_handler = main_menu_screens[*line];
    *line = 0;
    pulse_sprite_falling = 0;
    pulse_value_falling = 0;
}

/* Steps the counter D_00446B39 round 0 to 6. Nothing calls it, and nothing else touches the counter. */
static void Menu_StepBlink(void) {
    D_00446B39 = (s8)(D_00446B39 + 1) % 7;
}

/* Takes item out of its ring and links it in right after the item `after`. Nothing calls it. */
static void Menu_MoveItemAfter(MenuItem *item, MenuItem *after) {
    item->next->prev = item->prev;
    item->prev->next = item->next;
    item->next = after->next;
    item->prev = after;
    after->next->prev = item;
    after->next = item;
}

/* Steps value toward `target` by (target - start) / 60 per call, bouncing between the two ends. */
static f32 Menu_PulseValue(f32 value, s32 start, s32 target) {
    f32 bound;

    if (pulse_value_falling == 0) {
        bound = (f32) target;
        value = value + ((f32) (target - start) / 60.0f);
        if (bound < value) {
            value = bound;
            pulse_value_falling = 1;
        }
    } else {
        bound = (f32) start;
        value = value - ((f32) (target - start) / 60.0f);
        if (value < bound) {
            pulse_value_falling = 0;
            value = bound;
        }
    }
    return value;
}

/* The Use Item screen (main menu line 0): its parts, the turning model, the list of seven and the cursor bar. The
   confirm button uses the item and closes the menu, button 0x8000 opens the key items; fades in and out. */
static void Menu_UpdateUseItem(HandlerContext *context) {
    MenuCursorBar bar;
    f32 view[16];
    f32 local[16];
    GsPackedWord word;
    GsPackedWord corner;
    MenuItem *item;
    MenuItem *bar_item;
    u32 i;
    s32 row;
    s32 table;
    s32 entry;
    s32 slot;
    s32 text;
    u8 kind;
    u32 full_alpha;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 0.8f;
    view[15] = 1.0f;
    word.raw = 0x0A0AAE607C307080;
    corner.raw = 0x0A0F42407C306FE0;
    item = item_list_top[0]; // match: dead (the magic menu, Menu_UpdateCastSpell, takes its first row here); it keeps the address's high part out of a register
    if (item_list_cursor[0] != NULL) {
        Menu_MoveItemListCursor(context, menu_input_enabled == 0);
    }
    if (item_list_cursor[0] != NULL) {
        D_00446DF4 = item_counts[item_list_cursor[0]->unk0C];
    } else {
        D_00446DF4 = 0;
    }
    for (i = 0; i < item_part_count; i++) {
        kind = item_parts[i].handler_index;
        ((void (*)(u8))Menu_SetBlendMode)(item_parts[i].blend_mode); // match: called as if it returned nothing
        if (item_list_cursor[0] != NULL) {
            MenuSprite_Draw((int)&item_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
        } else {
            MenuSprite_Draw((int)&item_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
        }
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as if it returned nothing
    }
    use_item_frames++;
    if (use_item_frames >= 0x80) {
        use_item_frames = 0;
    }
    if (item_list_cursor[0] != NULL) {
        bar_item = item_list_top[0];
        for (row = 0; row < 7; row++, bar_item = bar_item->prev) {
            if (bar_item == item_list_cursor[0]) {
                bar.first.f.lo16 = corner.f.lo16;
                bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
                bar.first.f.mid24 = corner.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
                bar.second.f.mid24 = bar.first.f.mid24;
                if (use_item_state == 1) {
                    screen_part_alphas[253] = Menu_PulseValue(screen_part_alphas[253], 0x10, 0x20);
                }
                bar.alpha = (s32)screen_part_alphas[253];
                MenuDraw_Bar(&bar);
                break;
            }
        }
    }
    if (item_list_cursor[0] != NULL) {
        sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        use_item_model_angle = wrap_angle(use_item_model_angle);
        sceVu0RotMatrixY(local, view, use_item_model_angle);
        sceVu0TransMatrix(local, local, use_item_model_position);
        Menu_DrawItemModel(local);
        item = item_list_top[0];
        MenuText_SetColor(menu_draw_color);
        for (row = 0; row < 7; row++, item = item->prev) {
            text = (s32)&item_name_defs[item->unk0C];
            MenuText_SetPen(word);
            ((s32 (*)(s32, s32))MenuText_DrawWrapped)(text, 0x1A); // match: called as if it took and returned a number
            word.f.mid16 += 0xB0;
            word.f.lo16 = 0x7080;
            if (item_list_page_mark[0]->prev == item) {
                break;
            }
        }
        item = item_list_cursor[0];
        table = Message_GetFontText(menu_heap);
        slot = item->unk0C + MSG_ITEM_TEXT_FIRST + 1;
        entry = ((s32 *)table)[slot];
        word.raw = 0x000A875083207080;
        MenuText_SetPen(word);
        if (entry != 0) {
            text = table + entry;
            MenuText_SetColor(menu_draw_color);
            MenuText_DrawWordWrappedLong((u16 *)text, 0x33);
        }
    }
    if ((menu_buttons & PAD_CROSS) && menu_input_enabled != 0 && item_list_cursor[0] != NULL) {
        if (item_list_cursor[0]->unk0C < ITEM_ARROW || item_list_cursor[0]->unk0C > ITEM_WIDDAS_ARROW) {
            Sound_PlayMenu(0);
            game_status.bit6 = TRUE;
            game_status.item_to_use = item_list_cursor[0]->unk0C;
            use_item_state = 2;
            menu_input_enabled = 0;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        } else {
            Sound_PlayMenu(2);
        }
    } else if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        use_item_state = 3;
        menu_input_enabled = 0;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    } else if ((menu_buttons & PAD_SQUARE) && menu_input_enabled != 0) {
        Sound_PlayMenu(0);
        use_item_state = 4;
        menu_input_enabled = 0;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    }
    if (use_item_state == 2 && model_load_chain == NULL) {
        use_item_state = 5;
    }
    if (use_item_state == 3 && model_load_chain == NULL) {
        use_item_state = 6;
    }
    if (use_item_state == 4 && model_load_chain == NULL) {
        use_item_state = 7;
    }
    switch (use_item_state) {
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
        for (i = 0; i < item_part_count; i++) {
            screen_part_alphas[i] += item_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > item_parts[i].alpha) {
                for (i = 0; i < item_part_count; i++) {
                    full_alpha = item_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                use_item_state = 1;
                menu_input_enabled = use_item_state;
                return;
            }
        }
        break;
    case 5:
    case 6:
    case 7:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[253] -= 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < item_part_count; i++) {
            screen_part_alphas[i] -= item_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                switch (use_item_state) {
                case 5:
                    menu_screen_handler = NULL;
                    menu_step++;
                    break;
                case 6:
                    ((s32 (*)(s32))Menu_ResetLoadBuffers)(0); // match: called as if it took a number and returned one
                    menu_screen_handler = Menu_OpenMain;
                    break;
                case 7:
                    ((s32 (*)(s32))Menu_ResetLoadBuffers)(0); // match: called as if it took a number and returned one
                    ((void (*)(s32))Menu_ChainKeyItemModels)(0); // match: the callee takes nothing
                    menu_screen_handler = Menu_UpdateKeyItems;
                    break;
                }
                use_item_state = 0;
                return;
            }
        }
        break;
    }
}

/* The Cast a Spell screen (main menu line 1): its parts, the list of seven spells and the cursor bar, the spell's
   text and model; fades in and out. */
static void Menu_UpdateCastSpell(s32 context) {
    f32 view[16];
    f32 local[16];
    MenuCursorBar bar;
    GsPackedWord word;
    GsPackedWord corner;
    GsPackedWord pitch;
    MenuItem *item;
    MenuItem *selected;
    u32 *packet;
    u32 i;
    s32 row;
    s32 page;
    s32 number;
    s32 table;
    s32 entry;
    s32 slot;
    u8 kind;
    u32 full_alpha;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 0.8f;
    view[15] = 1.0f;
    if (castable_spell_first[0] == 0) {
        menu_screen_handler = Menu_OpenMain;
        return;
    }
    if (cast_spell_state < 2) {
        ((void (*)(s32))Menu_MoveSpellCursor)(context); // match: the callee takes nothing
    }
    item = castable_spell_top[0];
    word.raw = 0x0C0AAE607C307080;
    MenuText_SetPen(word);
    corner.raw = 0x0A0F42407C307000;
    pitch.raw = 0xB00140;
    MenuText_SetGlyphSize(pitch);
    i = 0;
    number = castable_spell_cursor[0]->unk0C; /* the selected magic's number */
    page = (number - 1) / 15;
    D_00446DF4 = fx_table[number].mp_cost;
    D_00446DF8 = (number - 1) % 3 + 1;
    for (; i < magic_part_count; i++) {
        kind = magic_parts[i].handler_index;
        Menu_SetBlendMode(magic_parts[i].blend_mode);
        MenuSprite_Draw((int)&magic_parts[i], kind + 1000, page, (int)screen_part_alphas[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as if it returned nothing
    }
    for (row = 0; row < 7; row++, item = item->prev) {
        if (castable_spell_cursor[0] == item) {
            bar.first.f.lo16 = corner.f.lo16;
            bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
            bar.first.f.mid24 = corner.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1220;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (cast_spell_state == 1) {
                screen_part_alphas[253] = Menu_PulseValue(screen_part_alphas[253], 0x10, 0x20);
            }
            bar.alpha = (s32)screen_part_alphas[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }
    item = castable_spell_top[0];
    MenuText_SetColor(menu_draw_color);
    for (row = 0; row < 7; row++, item = item->prev) {
        MenuText_DrawLine((u16 *)&fx_table[item->unk0C], 0x1A);
        word.f.mid16 += 0xB0; // match: dead, left over from the item menu (Menu_UpdateUseItem), which sets the row's place here
        word.f.lo16 = 0x7080; // match: dead as well; without the two lines the loop is compiled differently
        if (castable_spell_page_mark[0]->prev == item) {
            break;
        }
    }
    selected = castable_spell_cursor[0];
    table = Message_GetFontText(menu_heap);
    slot = selected->unk0C + MSG_MAGIC_TEXT_FIRST + 1;
    entry = ((s32 *)table)[slot];
    word.raw = 0x000A875082E07000;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong((u16 *)(table + entry), 0x35);
    }
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    sceVu0TransMatrix(local, view, cast_spell_model_position);
    packet = MenuDraw_NextPacket();
    packet[0] = 0x8002;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0x60F0;
    packet[5] = 0x7990;
    packet[6] = 0x19;
    packet[7] = 0;
    packet[8] = 0x44;
    packet[9] = 0;
    packet[10] = 0x43;
    packet[11] = 0;
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 3;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = D_CHCR_STR;
    DRAW_NODE_MODEL(&model_load_shown, local, menu_draw_color);
    if ((menu_buttons & PAD_CROSS) && menu_input_enabled != 0) {
        if (player_state.mp_now >= ((FxTableEntry *)castable_spell_cursor[0]->unk00)->mp_cost && player_state.magic_gauge >= 5000) {
            Sound_PlayMenu(0);
            game_status.magic_to_cast = castable_spell_cursor[0]->unk0C;
            cast_spell_state = 2;
            menu_input_enabled = 0;
            if (model_load_chain != NULL) {
                model_load_chain->next = NULL;
            }
        } else {
            Sound_PlayMenu(3);
        }
    } else if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        cast_spell_state = 2;
        menu_input_enabled = 0;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    }
    if (cast_spell_state == 2 && model_load_chain == NULL) {
        cast_spell_state = 3;
    }
    switch (cast_spell_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        screen_part_alphas[253] += 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[255] = 16.0f; // match: retail clamps the text alpha here, not the bar's ([253])
        }
        for (i = 0; i < magic_part_count; i++) {
            screen_part_alphas[i] += magic_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > magic_parts[i].alpha) {
                menu_input_enabled = cast_spell_state = 1;
                for (i = 0; i < magic_part_count; i++) {
                    full_alpha = magic_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                return;
            }
        }
        break;
    case 3:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[253] -= 1.6f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < magic_part_count; i++) {
            screen_part_alphas[i] -= magic_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                cast_spell_state = 0;
                if (game_status.magic_to_cast != 0) {
                    menu_screen_handler = NULL;
                    menu_step++;
                } else {
                    ((s32 (*)(s32))Menu_ResetLoadBuffers)(0); // match: called as if it took a number and returned one
                    menu_screen_handler = Menu_OpenMain;
                }
                return;
            }
        }
        break;
    }
}

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
/* per MenuItem.group, the LevelDef number its records start at, as the equipment screen has it */
static const EquipGroupBases equip_group_bases = { { 150, 0, 220, 220, 220, 220, 220, 220, 220, 220, 0, 0 } };
/* the names of the equipment groups ("weapons", "magic", "shield"...); copied to a local and never read */
static const EquipUnusedTable equip_group_names = { {
    group_name_weapons, group_name_magic, group_name_shield, group_name_helm, group_name_armor, group_name_gauntlet, group_name_boot, group_name_ring, group_name_bracelet, group_name_amulet, group_name_item, group_name_item,
} };
/* where the three models of the equipment screen go */
static const VuVector4 equip_model_offset_low = { 55.0f, -60.0f, 400.0f, 0.0f };
static const VuVector4 equip_model_offset_middle = { 55.0f, 0.0f, 400.0f, 0.0f };
static const VuVector4 equip_model_offset_high = { 55.0f, 60.0f, 400.0f, 0.0f };
/* per MenuItem.group, the glyph that names the group */
static const EquipGroupBytes equip_group_glyphs = { { 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x63, 0x62, 0x64, 0x65 } };
/* a byte per MenuItem.group (for groups 0 to 6 the values slot_pool_groups has); copied to a local and never read */
static const EquipGroupBytes D_0043FDD0 = { { 4, -1, 5, 2, 1, 3, 0, -1, -1, -1, -1 } };

/* The Equip screen (main menu line 2): moves between the eleven lists and inside the list shown, draws the
   screen's parts, the worn weapon's model and the cursor item's model, the item's detail panel (button
   0x1000) or the list of everything equipped (0x8000), equips the cursor item (0x4000) and fades in and out. */
static void Menu_UpdateEquip(void) {
    EquipGroupBases bases = equip_group_bases;
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
    void (*draw)(PoolSlot *, ColorBytes);
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
    unused_table = equip_group_names;
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    DEAD_ADDITIONS_8(filler_first, filler_second);
    {
    void *definition;
    SpritePacket *parts;
    if ((menu_buttons & PAD_UP) && equip_model_shown != 0 && menu_input_enabled != 0 && worn_model_loading == 0) {
        EQUIP_LIST = EQUIP_LIST->next;
        ((void (*)(s32))Menu_ShiftEquipModelsNextList)(0); // match: called with the number alone
        equip_group_shown--;
        Sound_PlayMenu(1);
    } else if ((menu_buttons & PAD_DOWN) && equip_model_shown != 0 && menu_input_enabled != 0 && worn_model_loading == 0) {
        EQUIP_LIST = EQUIP_LIST->prev;
        ((void (*)(s32))Menu_ShiftEquipModelsPrevList)(0); // match: called with the number alone
        equip_group_shown++;
        Sound_PlayMenu(1);
    } else if (equip_state < 2 && worn_model_loading == 0) {
        ((void (*)(s32))Menu_MoveEquipCursor)(0); // match: the callee takes nothing
    }
    if (equip_group_shown > 10) {
        equip_group_shown = 0;
    } else if (equip_group_shown < 0) {
        equip_group_shown = 10;
    }
    index = CURRENT_ITEM->unk0C - bases.first_item[CURRENT_ITEM->group];
    if (CURRENT_ITEM->group == 0) {
        D_00446E30 = weapon_level_records[index].level_index + 1;
    } else if (CURRENT_ITEM->group == 1) {
        D_00446E30 = (index - 1) % 3 + 1;
    }
    for (i = 0; i < equip_part_count; i++) {
        kind = equip_parts[i].handler_index;
        SET_BLEND(equip_parts[i].blend_mode);
        if (Menu_IsEquipped(equip_group_shown, CURRENT_ITEM->group, CURRENT_ITEM->unk0C)) {
            MenuSprite_Draw((int)&equip_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
        } else {
            MenuSprite_Draw((int)&equip_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
        }
        SET_BLEND(0);
    }
    if ((menu_buttons & PAD_TRIANGLE) && equipped_list_open == 0 && menu_input_enabled != 0
        && Menu_IsEquipSlotFilled(index, CURRENT_ITEM->group)) {
        if (detail_panel_open == 0) {
            detail_panel_sound_pending = 1;
            detail_panel_open = 1;
        } else if (equip_panel_state != 2) {
            Sound_PlayMenu(1);
            equip_panel_state = 2;
        } else {
            equip_panel_state = 0;
        }
    }
    if ((menu_buttons & PAD_SQUARE) && detail_panel_open == 0 && menu_input_enabled != 0) {
        Sound_PlayMenu(1);
        if (equipped_list_open == 0) {
            equipped_list_open = 1;
        } else if (equip_panel_state != 2) {
            equip_panel_state = 2;
        } else {
            equip_panel_state = 0;
        }
    }
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    if (menu_buttons & PAD_R1) {
        equip_weapon_model_angle += 0.03f;
    } else if (menu_buttons & PAD_L1) {
        equip_weapon_model_angle -= 0.03f;
    }
    equip_weapon_model_angle -= (f32)(s32)(equip_weapon_model_angle / 3.1415927f) * 6.2831855f;
    sceVu0RotMatrixY(weapon_matrix, unit, equip_weapon_model_angle);
    sceVu0TransMatrix(weapon_matrix, weapon_matrix, equip_weapon_model_position);
    node.pool_node = &D_004472B8;
    node.unk20 = 1;
    node.unk00 = 0;
    D_00446ACC = weapon_defs[player_state.weapon].equip_menu_motion;
    if (weapon_defs[player_state.weapon].action_handler_index == 0) {
        D_00446ACC += worn_defs[player_state.equipment[EQUIP_SHIELD]].equip_menu_motion_offset;
    }
    PoolNode_Pose((RelocEntry *)equip_weapon_motions[D_00446ACC]->frames, pool_slots, weapon_matrix, equip_weapon_motion_frame);
    equip_weapon_motion_frame++;
    equip_weapon_motion_frame %= equip_weapon_motions[D_00446ACC]->frame_count;
    // match: a pointer-typed load, as in CURRENT_ITEM
    D_00446ACC = ((PlayerItemDef *)((MenuItemList *)equipped_weapon.item)->item)->equip_menu_draw_index;
    switch (weapon_defs[player_state.weapon].action_handler_index) {
    case 0:
        D_00446ACC += player_state.equipment[EQUIP_SHIELD] == ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
        break;
    case 1:
        D_00446ACC++;
        break;
    case 2:
        D_00446ACC = 0;
        break;
    }
    draw = pool_slot_drawers[D_00446ACC];
    scissor_packet = MenuDraw_NextPacket();
    scissor_packet[0] = 0x8001;
    scissor_packet[1] = 0x10000000;
    scissor_packet[2] = 0xE;
    scissor_packet[3] = 0;
    scissor_packet[4] = 0x02590150;
    scissor_packet[5] = 0x00AD0015;
    scissor_packet[6] = 0x40;
    scissor_packet[7] = 0;
    GIF_SEND(scissor_packet, 2);
    first_offset = equip_model_offset_low;
    second_offset = equip_model_offset_middle;
    third_offset = equip_model_offset_high;
    sceVu0TransMatrix((f32 *)&pool_slots[21], unit, &first_offset.x);
    sceVu0TransMatrix((f32 *)&pool_slots[22], unit, &second_offset.x);
    sceVu0TransMatrix((f32 *)&pool_slots[23], unit, &third_offset.x);
    DRAW_SLOT(&pool_slots[21]);
    DRAW_SLOT(&pool_slots[22]);
    DRAW_SLOT(&pool_slots[23]);
    draw(pool_slots, model_draw_color);
    full_packet = MenuDraw_NextPacket();
    full_packet[0] = 0x8001;
    full_packet[1] = 0x10000000;
    full_packet[2] = 0xE;
    full_packet[3] = 0;
    full_packet[4] = 0x02800000;
    full_packet[5] = 0x00E00000;
    full_packet[6] = 0x40;
    full_packet[7] = 0;
    GIF_SEND(full_packet, 2);
    packet = MenuDraw_NextPacket();
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
        if (equip_state != 2 && model_load_shown.unk20 != 0) {
            equip_model_shown = 1;
            screen_part_alphas[254] += 12.8f;
            if (screen_part_alphas[254] > 128.0f) {
                screen_part_alphas[254] = 128.0f;
            }
            model_draw_color.alpha = (s32)screen_part_alphas[254];
        }
        sceVu0TransMatrix(item_matrix, scale, equip_item_model_position);
        DRAW_ITEM_MODEL(item_matrix);
    } else {
        if (equip_state != 2 && model_load_shown.unk20 != 0) {
            equip_model_shown = 1;
            screen_part_alphas[254] += 12.8f;
            if (screen_part_alphas[254] > 128.0f) {
                screen_part_alphas[254] = 128.0f;
            }
            model_draw_color.alpha = (s32)screen_part_alphas[254];
        }
        angle = equip_item_model_angle + 3.1415927f;
        equip_item_model_angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
        sceVu0RotMatrixY(item_matrix, scale, equip_item_model_angle);
        sceVu0TransMatrix(item_matrix, item_matrix, equip_item_model_position);
        DRAW_ITEM_MODEL(item_matrix);
    }
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    if (CURRENT_ITEM->group == 1) {
        definition = &fx_table[CURRENT_ITEM->unk0C];
    } else {
        definition = &item_name_defs[CURRENT_ITEM->unk0C];
    }
    if (detail_panel_open != 0) {
        switch (CURRENT_ITEM->group) {
        case 0:
            D_00446DF4 = (weapon_level_records[index].gained_values[0] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[0]) * weapon_level_records[index].level / 100;
            D_00446DF8 = (weapon_level_records[index].gained_values[1] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[1]) * weapon_level_records[index].level / 100;
            D_00446DFC = (weapon_level_records[index].gained_values[2] + weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[2]) * weapon_level_records[index].level / 100;
            D_00446E00 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[3] * weapon_level_records[index].level / 100;
            D_00446E04 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[4] * weapon_level_records[index].level / 100;
            D_00446E08 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[5] * weapon_level_records[index].level / 100;
            D_00446E0C = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[6] * weapon_level_records[index].level / 100;
            D_00446E10 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[7] * weapon_level_records[index].level / 100;
            D_00446E14 = weapon_defs[index].attack_by_level[weapon_level_records[index].level_index].values[8] * weapon_level_records[index].level / 100;
            D_00446E1C = (s32)(weapon_defs[index].weapon_length_cm * 0.03281f * 10.0f);
            D_00446E20 = weapon_level_records[index].level;
            D_00446E18 = item_name_defs[CURRENT_ITEM->unk0C].weight_tenths_of_pound;
            D_00446E24 = 100;
            D_00446E28 = player_weapon[EQUIP_LIST->group];
            if (equip_panel_state == 1) {
                RESET_ALPHAS(equip_a_parts, equip_a_part_count);
            }
            if (index != 0) {
                if (detail_panel_sound_pending != 0) {
                    Sound_PlayMenu(1);
                }
                detail_panel_sound_pending = 0;
                DRAW_PARTS(equip_a_parts, equip_a_part_count, 0);
                MenuText_SetColor(panel_text_color);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
            }
            parts = equip_a_parts;
            count = equip_a_part_count;
            break;
        case 1:
            if (fx_table[CURRENT_ITEM->unk0C].item_menu_magic == 0) {
                magic = &fx_table[index];
                D_00446DF4 = magic->hit_checks[0].strengths.unk00[0];
                D_00446DF8 = magic->hit_checks[0].strengths.unk00[1];
                D_00446DFC = magic->hit_checks[0].strengths.unk00[2];
                D_00446E00 = magic->hit_checks[0].strengths.unk00[3];
                D_00446E04 = magic->hit_checks[0].strengths.unk00[4];
                D_00446E08 = magic->hit_checks[0].strengths.unk00[5];
                D_00446E0C = magic->hit_checks[0].strengths.unk00[6];
                D_00446E10 = magic->hit_checks[0].strengths.unk00[7];
                D_00446E14 = magic->hit_checks[0].strengths.unk00[8];
                D_00446E1C = magic->mp_cost;
                if (equip_panel_state == 1) {
                    RESET_ALPHAS(equip_m_parts, equip_m_part_count);
                }
                if (index != 0) {
                    if (detail_panel_sound_pending != 0) {
                        Sound_PlayMenu(1);
                    }
                    detail_panel_sound_pending = 0;
                    DRAW_PARTS(equip_m_parts, equip_m_part_count, (index - 1) / 15);
                    MenuText_SetColor(panel_text_color);
                    Menu_DrawEquipItemText(MSG_MAGIC_TEXT_FIRST);
                }
                parts = equip_m_parts;
                count = equip_m_part_count;
            } else {
                D_00446E1C = fx_table[index].mp_cost;
                if (equip_panel_state == 1) {
                    RESET_ALPHAS(equip_m2_parts, equip_m2_part_count);
                }
                if (index != 0) {
                    if (detail_panel_sound_pending != 0) {
                        Sound_PlayMenu(1);
                    }
                    detail_panel_sound_pending = 0;
                    DRAW_PARTS(equip_m2_parts, equip_m2_part_count, (index + 1) / 15);
                    MenuText_SetColor(panel_text_color);
                    Menu_DrawEquipItemText(MSG_MAGIC_TEXT_FIRST);
                }
                parts = equip_m2_parts;
                count = equip_m2_part_count;
            }
            break;
        case 11:
            if (equip_panel_state == 1) {
                RESET_ALPHAS(equip_s_parts, equip_s_part_count);
            }
            if (index != 0) {
                if (detail_panel_sound_pending != 0) {
                    Sound_PlayMenu(1);
                }
                detail_panel_sound_pending = 0;
                for (i = 0; i < equip_s_part_count; i++) {
                    SET_BLEND(equip_s_parts[i].blend_mode);
                    MenuSprite_Draw((int)&equip_s_parts[i], 1002, 1, (int)panel_part_alphas[i]);
                    SET_BLEND(0);
                }
                MenuText_SetColor(panel_text_color);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
            }
            parts = equip_s_parts;
            count = equip_s_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            D_00446DF4 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            D_00446DF8 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            D_00446DFC = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            D_00446E00 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            D_00446E04 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            D_00446E08 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            D_00446E0C = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            D_00446E10 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            D_00446E14 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            D_00446E18 = item_name_defs[CURRENT_ITEM->unk0C].weight_tenths_of_pound;
            D_00446E1C = player_weapon[EQUIP_LIST->group];
            D_00446E20 = worn_item_level_records[index].level;
            D_00446E24 = 100;
            if (index != 0 && index != 30 && index != 55 && index != 80 && index != 105) {
                if (detail_panel_sound_pending != 0) {
                    Sound_PlayMenu(1);
                }
                detail_panel_sound_pending = 0;
                if (equip_panel_state == 1) {
                    RESET_ALPHAS(equip_g_parts, equip_g_part_count);
                }
                DRAW_PARTS(equip_g_parts, equip_g_part_count, D_00446E1C);
                MenuText_SetColor(panel_text_color);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
            }
            parts = equip_g_parts;
            count = equip_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            D_00446DF4 = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
            D_00446DF8 = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
            D_00446DFC = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
            D_00446E00 = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
            D_00446E04 = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
            D_00446E08 = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
            D_00446E0C = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
            D_00446E10 = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
            D_00446E14 = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
            D_00446E18 = item_name_defs[CURRENT_ITEM->unk0C].weight_tenths_of_pound;
            D_00446E1C = player_weapon[EQUIP_LIST->group];
            if (index != 130 && index != 140 && index != 150) {
                if (detail_panel_sound_pending != 0) {
                    Sound_PlayMenu(1);
                }
                detail_panel_sound_pending = 0;
                if (equip_panel_state == 1) {
                    RESET_ALPHAS(equip_g_parts, equip_g_part_count); // match: the armour panel's list; the parts drawn and faded in this case are equip_g2_parts's
                }
                DRAW_PARTS(equip_g2_parts, equip_g2_part_count, D_00446E1C);
                MenuText_SetColor(panel_text_color);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
            }
            parts = equip_g2_parts;
            count = equip_g2_part_count;
            break;
        }
        switch (equip_panel_state) {
        case 0:
            panel_part_alphas[255] += 12.8f;
            if (panel_part_alphas[255] > 128.0f) {
                panel_part_alphas[255] = 128.0f;
            }
            panel_text_color.alpha = (s32)panel_part_alphas[255];
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        alpha = parts[i].alpha;
                        panel_part_alphas[i] = alpha;
                    }
                    equip_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            panel_part_alphas[255] -= 12.8f;
            if (panel_part_alphas[255] < 0.0f) {
                panel_part_alphas[255] = 0.0f;
            }
            panel_text_color.alpha = (s32)panel_part_alphas[255];
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    detail_panel_open = 0;
                    equip_panel_state = 0;
                    break;
                }
            }
            break;
        }
    }
    MenuText_SetColor(menu_draw_color);
    word.raw = 0x0A0A875084106EF0;
    word.f.mid24 = 0xA8750;
    MenuText_SetPen(word);
    MenuText_DrawLineCentered(definition, 0x18);
    {
        EquipGroupBytes glyphs = equip_group_glyphs;

        glyph = glyphs.per_group[EQUIP_LIST->group];
        width = MenuDraw_GetWordStart(glyph + 1) - MenuDraw_GetWordStart(glyph) - 1;
        offset = (s32)(width * 0.5f * 20.0f);
        sprite.red = menu_draw_color.red;
        sprite.green = menu_draw_color.green;
        sprite.blue = menu_draw_color.blue;
        sprite.alpha = menu_draw_color.alpha;
        sprite.kind = 1;
        sprite.glyph = glyph;
        sprite.first.f.lo16 = (1865 << 4) - (offset << 4);
        sprite.first.f.mid16 = 0x7B60;
        sprite.first.f.mid24 = 0xA8750;
        sprite.second.f.lo16 = (width * 20 << 4) + (1865 << 4) - (offset << 4);
        sprite.second.f.mid16 = 0x7C00;
        sprite.second.f.mid24 = 0xA8750;
        MenuText_SetColor(menu_draw_color);
        ((int (*)(KindObject *))MenuDraw_Word)((KindObject *)&sprite); // match: the result register stays taken
    }
    if (equipped_list_open != 0) {
        MenuDraw_SetGsRegister(0x47, 0x30000);
        word.raw = 0x000A87507AF07D00;
        for (i = 0; i < equip_s_part_count; i++) {
            SET_BLEND(equip_s_parts[i].blend_mode);
            MenuSprite_Draw((int)&equip_s_parts[i], equip_s_parts[i].handler_index + 1000, 0, (int)panel_part_alphas[i]);
            SET_BLEND(0);
        }
        switch (equip_panel_state) {
        case 0:
            panel_part_alphas[255] += 12.8f;
            if (panel_part_alphas[255] > 128.0f) {
                panel_part_alphas[255] = 128.0f;
            }
            panel_text_color.alpha = (s32)panel_part_alphas[255];
            for (i = 0; i < equip_s_part_count; i++) {
                panel_part_alphas[i] += equip_s_parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] > equip_s_parts[i].alpha) {
                    for (i = 0; i < equip_s_part_count; i++) {
                        alpha = equip_s_parts[i].alpha;
                        panel_part_alphas[i] = alpha;
                    }
                    equip_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            panel_part_alphas[255] -= 12.8f;
            if (panel_part_alphas[255] < 0.0f) {
                panel_part_alphas[255] = 0.0f;
            }
            panel_text_color.alpha = (s32)panel_part_alphas[255];
            for (i = 0; i < equip_s_part_count; i++) {
                panel_part_alphas[i] -= equip_s_parts[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400);
                    equip_panel_state = 0;
                    equipped_list_open = 0;
                    break;
                }
            }
            break;
        }
        for (i = 0; i < 11; i++) {
            if (i == 1) {
                listed = &fx_table[(*equipped_item_refs[1])->unk0C];
            } else if (i == 10) {
                if ((*equipped_item_refs[10])->group == 1) {
                    listed = &fx_table[(*equipped_item_refs[10])->unk0C];
                } else {
                    listed = &item_name_defs[(*equipped_item_refs[10])->unk0C];
                }
            } else {
                listed = &item_name_defs[(*equipped_item_refs[i])->unk0C];
            }
            MenuText_SetPen(word);
            MenuText_SetColor(panel_text_color);
            ((int (*)(void *, s32))MenuText_DrawWrapped)(listed, 0x1A); // match: the result register stays taken
            word.f.mid16 += 0xD0;
        }
    }
    if ((menu_buttons & PAD_CROSS) && equip_model_shown != 0 && menu_input_enabled != 0) {
        EquipGroupBytes unused_bytes = D_0043FDD0;
        f32 unused_pair[32];

        if (*equipped_item_refs[EQUIP_LIST->group] != *equip_list_refs[EQUIP_LIST->group]) {
            Sound_PlayMenu(0);
            if (weapon_defs[player_state.weapon].action_handler_index == 2 && EQUIP_LIST->group == 0
                && weapon_defs[index].action_handler_index != 2) {
*equipped_item_refs[2] = shield_entries;
player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
pool_slots[20].part = NULL;
            }
            (&player_state.weapon)[EQUIP_LIST->group] = index;
            *equipped_item_refs[EQUIP_LIST->group] = CURRENT_ITEM;
            if (weapon_defs[player_state.weapon].action_handler_index != 0 && EQUIP_LIST->group == 0) {
*equipped_item_refs[2] = shield_entries;
player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
pool_slots[20].part = NULL;
            }
            if (weapon_defs[player_state.weapon].action_handler_index != 0 && EQUIP_LIST->group == 2) {
*equipped_item_refs[0] = weapon_entries;
player_state.weapon = 0;
pool_slots[15].part = NULL;
            }
            if (EQUIP_LIST->group == 10) {
                if (CURRENT_ITEM->unk0C != 0) {
                    switch (CURRENT_ITEM->group) {
                    case 11:
                        player_state.shortcut_type = 1;
                        break;
                    case 1:
                        player_state.shortcut_type = 2;
                        break;
                    default:
                        player_state.shortcut_type = 0;
                        break;
                    }
                } else {
                    player_state.shortcut_type = 0;
                }
            }
            Player_RecomputeStats();
            if (player_state.hp_max < player_state.hp_now) {
                player_state.hp_now = player_state.hp_max;
            }
            if (player_state.mp_now > player_state.mp_max) {
                player_state.mp_now = player_state.mp_max;
            }
            if (CURRENT_ITEM->group != 1 && CURRENT_ITEM->group != 11) {
                Menu_SetWornModelNames();
                equip_model_loads[EQUIP_LIST->group]->unk20 = 0;
                if (worn_model_loader.unk10 != NULL) {
                    equip_model_loads[EQUIP_LIST->group]->next = worn_model_loader.unk10->next;
                    worn_model_loader.unk10->next = equip_model_loads[EQUIP_LIST->group];
                } else {
                    worn_model_loader.unk10 = equip_model_loads[EQUIP_LIST->group];
                }
                if (model_load_chain != NULL) {
                    worn_model_loader.next = model_load_chain->next;
                    model_load_chain->next = &worn_model_loader;
                } else {
                    model_load_chain = &worn_model_loader;
                }
                worn_model_loading = 1;
            }
        }
    } else if ((menu_buttons & PAD_CIRCLE) && equip_model_shown != 0 && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        equip_state = 2;
        menu_input_enabled = 0;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    }
    if (equip_state == 2 && model_load_chain == NULL) {
        equip_state = 3;
    }
    switch (equip_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        for (i = 0; i < equip_part_count; i++) {
            screen_part_alphas[i] += equip_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > equip_parts[i].alpha) {
                for (i = 0; i < equip_part_count; i++) {
                    alpha = equip_parts[i].alpha;
                    screen_part_alphas[i] = alpha;
                }
                equip_state = 1;
                menu_input_enabled = 1;
                return;
            }
        }
        break;
    case 3:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        screen_part_alphas[254] -= 12.8f;
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        if (screen_part_alphas[254] < 0.0f) {
            screen_part_alphas[254] = 0.0f;
        }
        model_draw_color.alpha = (s32)screen_part_alphas[254];
        for (i = 0; i < equip_part_count; i++) {
            screen_part_alphas[i] -= equip_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                menu_screen_handler = Menu_OpenMain;
                ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                PoolSlot_ClearParts();
                panel_text_color.alpha = 0;
                panel_text_color.red = panel_text_color.green = panel_text_color.blue = 0x80;
                equip_state = 0;
                equip_panel_state = 0;
                detail_panel_open = 0;
                equipped_list_open = 0;
                worn_model_loading = 0;
                Menu_BuildItemRings();
                return;
            }
        }
        break;
    }
    }
}

/* The Status Viewer screen (main menu line 3): only shows its parts; fades in, and on the cancel button out. */
static void Menu_UpdateStatus(void) {
    u32 i;
    u32 full_alpha;

    for (i = 0; i < status_part_count; i++) {
        Menu_SetBlendMode(status_parts[i].blend_mode);
        MenuSprite_Draw((int)&status_parts[i], status_parts[i].handler_index + 1000, 0, (int)screen_part_alphas[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        menu_input_enabled = 0;
        key_items_state = 3;
    }

    switch (key_items_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        for (i = 0; i < status_part_count; i++) {
            screen_part_alphas[i] += status_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > status_parts[i].alpha) {
                for (i = 0; i < status_part_count; i++) {
                    full_alpha = status_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                menu_input_enabled = key_items_state = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        for (i = 0; i < status_part_count; i++) {
            screen_part_alphas[i] -= status_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                if (key_items_state == 3) {
                    menu_screen_handler = (void *)Menu_OpenMain;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else {
                    menu_screen_handler = (void *)Menu_UpdateKeyItems;
                }
                key_items_state = 0;
                break;
            }
        }
        break;
    }
}

/* Empties the twelve key item slots. */
void Menu_ClearKeyItems(void) {
    s32 i;
    s32 empty = -1;

    for (i = 11; i >= 0; i--) {
        key_item_slots[i] = empty;
    }
}

/* Puts the key item that goes with LevelDef number `key` into the first empty key item slot. */
void Menu_AddKeyItem(s32 key) {
    s32 slot;

    for (slot = 0; slot < 12; slot++) {
        if (key_item_slots[slot] < 0) {
            break;
        }
    }
    if (slot < 12) {
        switch (key) {
        case 0x31:
            key_item_slots[slot] = 0;
            break;
        case 0x30:
            key_item_slots[slot] = 1;
            break;
        case 0x2F:
            key_item_slots[slot] = 2;
            break;
        case 0x34:
            key_item_slots[slot] = 3;
            break;
        case 0x32:
            key_item_slots[slot] = 4;
            break;
        case 0x3A:
            key_item_slots[slot] = 5;
            break;
        case 0x39:
            key_item_slots[slot] = 6;
            break;
        case 0x38:
            key_item_slots[slot] = 7;
            break;
        case 0x37:
            key_item_slots[slot] = 8;
            break;
        case 0x33:
            key_item_slots[slot] = 9;
            break;
        case 0x3B:
            key_item_slots[slot] = 10;
            break;
        case 0x35:
            key_item_slots[slot] = 11;
            break;
        }
    }
}

/* the GS texture and CLUT base pointers of the six key item models on screen */
static const KeyItemTexturePages key_item_texture_pages = { { 0x1A40, 0x1AC0, 0x1B40, 0x1BC0, 0x1C40, 0x1CC0 } };
static const KeyItemTexturePages key_item_clut_pages = { { 0x3D40, 0x3D44, 0x3D48, 0x3D4C, 0x3D50, 0x3D54 } };
/* where the six key item models go */
static const KeyItemPositions key_item_positions = { {
    { -140.0f, -65.0f, 380.0f, 0.0f },
    { 0.0f, -65.0f, 380.0f, 0.0f },
    { 140.0f, -65.0f, 380.0f, 0.0f },
    { -140.0f, 55.0f, 380.0f, 0.0f },
    { 0.0f, 55.0f, 380.0f, 0.0f },
    { 140.0f, 55.0f, 380.0f, 0.0f },
} };
/* where the six key item names go, as MenuText_SetPen takes them */
static const KeyItemNameWords key_item_name_pens = { {
    { 0x0A2DC6C07F606F50 },
    { 0x0A2DC6C07F607B30 },
    { 0x0A2DC6C07F608710 },
    { 0x0A2DC6C084706F50 },
    { 0x0A2DC6C084707B30 },
    { 0x0A2DC6C084708710 },
} };

/* The key item screen: six of the twelve slots at a time, each with its model and, once got, its name. */
static void Menu_UpdateKeyItems(void) {
    KeyItemNumbers numbers = key_item_numbers;
    KeyItemTexturePages texture_pages = key_item_texture_pages;
    KeyItemTexturePages clut_pages = key_item_clut_pages;
    KeyItemPositions positions = key_item_positions;
    f32 view[16];
    RenderSlotArg slot_arg;
    KeyItemModelFile *file;
    TextureHeader *texture;
    u32 i;
    u32 full_alpha;
    s32 slot;
    s32 *number;
    struct PoolPart *part;
    ChunkUpload *upload;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 1.0f;
    view[15] = 1.0f;
    if ((menu_buttons & PAD_UP) && menu_input_enabled != 0) {
        Sound_PlayMenu(1);
        key_item_row--;
    } else if ((menu_buttons & PAD_DOWN) && menu_input_enabled != 0) {
        Sound_PlayMenu(1);
        key_item_row++;
    }
    key_item_row += 3;
    key_item_row %= 3;
    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    for (i = 0; i < keylist_part_count; i++) {
        Menu_SetBlendMode(keylist_parts[i].blend_mode);
        MenuSprite_Draw((int)&keylist_parts[i], 1000, 0, (int)screen_part_alphas[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }
    key_items_frames++;
    for (i = 0; (s32)i < 6; i++) {
        slot = key_item_slots[i + key_item_row * 3];
        if (slot < 0) {
            continue;
        }
        upload = (ChunkUpload *)((s32)key_item_model_loads + slot * sizeof(ChunkUpload)); // match: &key_item_model_loads[slot] as a sum of numbers gives the madd
        if (upload->done != 0) {
            file = upload->pool_node->buffer;
            texture = (TextureHeader *)((char *)file + file->texture_offset);
            part = (struct PoolPart *)((char *)file + file->part_offset + 0x10);
            TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages.base[i];
            TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages.base[i];
            pool_slots[31].part = part;
            // match: both words are read through unions, which keeps the two NULL stores below them
            pool_slots[31].unk58 = TEXTURE_HEADER_BITS(texture)->tex0.raw;
            pool_slots[31].unk60 = TEXTURE_HEADER_BITS(texture)->unk18.raw;
            pool_slots[31].unk48 = NULL;
            pool_slots[31].unk40 = NULL;
            MenuTexture_Upload(texture, NULL);
            sceVu0TransMatrix((f32 *)&pool_slots[31], view, positions.position[i]);
            PoolSlot_Draw((RenderSlot *)&pool_slots[31], slot_arg); // match: slot_arg is never set
            number = &numbers.number[key_item_slots[i + key_item_row * 3]];
            if (item_counts[*number] != 0) {
                KeyItemNameWords words = key_item_name_pens;
                LevelDef *def = &item_name_defs[*number];

                MenuText_SetColor(menu_draw_color);
                MenuText_SetPen(words.word[i]);
                MenuText_DrawLineCentered((u16 *)def, 0x10);
            }
        }
    }

    if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        key_items_state = 3;
        menu_input_enabled = 0;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    } else if ((menu_buttons & PAD_SQUARE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        key_items_state = 2;
        menu_input_enabled = 0;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    }
    if (key_items_state == 2 && model_load_chain == NULL) {
        key_items_state = 4;
    }
    if (key_items_state == 3 && model_load_chain == NULL) {
        key_items_state = 5;
    }

    switch (key_items_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        for (i = 0; i < keylist_part_count; i++) {
            screen_part_alphas[i] += keylist_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > keylist_parts[i].alpha) {
                for (i = 0; i < keylist_part_count; i++) {
                    full_alpha = keylist_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                menu_input_enabled = key_items_state = 1;
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
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        for (i = 0; i < keylist_part_count; i++) {
            screen_part_alphas[i] -= keylist_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                key_item_row = 0;
                menu_screen_handler = (void *)Menu_UpdateUseItem;
                ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                ((void (*)(s32))Menu_ChainItemListModels)(0); // match: the callee takes nothing
                if (key_items_state == 5) {
                    use_item_state = 3;
                }
                key_items_state = 0;
                key_items_frames = 0;
                break;
            }
        }
        break;
    }
}

/* The Recollection screen (main menu line 4): 50 pages of up to 46 entries, seven shown at a time under a cursor
   bar; picking one opens Menu_UpdateRecollectionPage with its text number in context->unk04. */
static void Menu_UpdateRecollection(HandlerContext *context) {
    GsPackedWord bar_origin;
    MenuCursorBar bar;
    s8 rows[LIST_PAGES];
    f32 view[16];
    f32 local[16];
    ChunkPoolNode node;
    GsPackedWord word;
    s32 i;
    s32 row_count;
    s32 table;
    s32 first_text;
    s32 entry;
    u32 *packet;
    PackageChunk *chunk;
    s32 show_picture;
    s32 row;
    u32 full_alpha;
    s32 text;
    s32 counter = 0;                /* one work variable: the pages tried, then a bit number, then a row */

    bar_origin.raw = 0x0001869F7C307EE0;
    /* left and right step to the next page that has entries */
    if ((menu_buttons & PAD_RIGHT) && menu_input_enabled != 0) {
        do {
            recollection_page += LIST_PAGES + 1; // match: two statements; one expression computes the sum in another register
            recollection_page %= LIST_PAGES;
            counter++; // match: counted before the test, so it sits in the branch's delay slot
            if (recollection_pages[recollection_page] != 0) {
                break;
            }
        } while (counter < LIST_PAGES);
        recollection_top = 0;
        if (recollection_page_before != 0 || recollection_page != 0) {
            recollection_cursor = 0;
        }
        if (recollection_page_before != recollection_page) {
            Sound_PlayMenu(1);
        }
        recollection_page_before = recollection_page;
    } else if ((menu_buttons & PAD_LEFT) && menu_input_enabled != 0) {
        do {
            recollection_page += LIST_PAGES - 1;
            recollection_page %= LIST_PAGES;
            counter++; // match: counted before the test, so it sits in the branch's delay slot
            if (recollection_pages[recollection_page] != 0) {
                break;
            }
        } while (counter < LIST_PAGES);
        recollection_top = 0;
        if (recollection_page_before != 0 || recollection_page != 0) {
            recollection_cursor = 0;
        }
        if (recollection_page_before != recollection_page) {
            Sound_PlayMenu(1);
        }
        recollection_page_before = recollection_page;
    }

    /* the entries the page has: one bit each in its row of recollection_flags, from bit 3 on */
    for (i = 0; i < LIST_PAGES; i++) {
        rows[i] = -1;
    }
    row_count = 0;
    counter = 3;
    for (i = 3; i < LIST_PAGES - 1; i++) {
        if ((recollection_flags[recollection_page][i / 8] >> counter) & 1) {
            rows[row_count] = i;
            row_count++;
        }
        counter++;
        counter %= 8;
    }

    if ((menu_buttons & PAD_UP) && menu_input_enabled != 0) {
        if (row_count >= 2) {
            Sound_PlayMenu(1);
        }
        if (recollection_cursor == recollection_top && row_count >= 8) {
            recollection_top--;
            if (recollection_top < 0) {
                recollection_top = row_count - 1;
            }
        }
        recollection_cursor--;
        if (recollection_cursor < 0) {
            recollection_cursor = row_count - 1;
        }
    } else if ((menu_buttons & PAD_DOWN) && menu_input_enabled != 0) {
        if (row_count >= 2) {
            Sound_PlayMenu(1);
        }
        if (((recollection_top < recollection_cursor && recollection_cursor - recollection_top >= 6) ||
             (recollection_cursor < recollection_top && recollection_cursor + row_count - recollection_top >= 6)) &&
            row_count >= 8) {
            recollection_top++;
            if (recollection_top >= row_count) {
                recollection_top = 0;
            }
        }
        recollection_cursor++;
        if (recollection_cursor >= row_count) {
            recollection_cursor = 0;
        }
    }

    for (i = 0; i < image_part_count; i++) {
        Menu_SetBlendMode(image_parts[i].blend_mode);
        MenuSprite_Draw((int)&image_parts[i], 1000, 0, (int)screen_part_alphas[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    show_picture = (s8)game_options[7] != 0;
    if (npc_package_load.done != 0 && show_picture) {
        memset(view, 0, 0x40);
        view[10] = view[5] = view[0] = 0.8f;
        view[15] = 1.0f;
        sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        sceVu0TransMatrix(local, view, recollection_model_position);
        packet = MenuDraw_NextPacket();
        packet[0] = 0x8002;
        packet[1] = 0x10000000;
        packet[2] = 0xE;
        packet[3] = 0;
        packet[4] = 0x7660;
        packet[5] = 0x7890;
        packet[6] = 0x19;
        packet[7] = 0;
        packet[8] = 0x44;
        packet[9] = 0;
        packet[10] = 0x43;
        packet[11] = 0;
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = D_CHCR_STR;
        chunk = (PackageChunk *)((char *)npc_package_load.pool_node->buffer + 0x10);
        for (i = 0; i < recollection_page; i++) {
            chunk = (PackageChunk *)((char *)chunk + chunk->unk0C + chunk->unk08);
        }
        if (equip_state != 2 && equip_state != 3 && model_load_shown.unk20 != 0) {
            screen_part_alphas[254] += 12.8f;
            if (screen_part_alphas[254] > 128.0f) {
                screen_part_alphas[254] = 128.0f;
            }
            model_draw_color.alpha = (s32)screen_part_alphas[254];
        }
        model_load_shown.pool_node = &node; // match: a local of this function; the node keeps the address after it returns
        model_load_shown.unk20 = 1;
        node.buffer = chunk;
        DRAW_NODE_MODEL(&model_load_shown, local, model_draw_color);
    }

    table = Message_GetFontText(menu_heap);
    first_text = recollection_page * LIST_PAGES + MSG_RECOLLECTION_FIRST + 1;
    entry = GetListTextOffset(table, first_text + LIST_PAGES - 1);
    word.raw = 0x000A875081106E80;
    MenuText_SetPen(word);
    if (entry != 0) {
        text = table + entry;
        MenuText_SetColor(menu_draw_color);
        MenuText_DrawLineCentered((u16 *)text, 0x17);
    }
    entry = GetListTextOffset(table, first_text + recollection_pages[recollection_page] - 1);
    word.raw = 0x000A875083106FE0;
    MenuText_SetPen(word);
    if (entry != 0) {
        text = table + entry;
        MenuText_SetColor(menu_draw_color);
        MenuText_DrawWrappedLong((u16 *)text, 0x33);
    }

    for (i = 0; i < LIST_ROWS && i < row_count; i++) {
        counter = recollection_top + i;
        row = counter % row_count; // match: kept in a variable, which orders the compare's operands
        if (recollection_cursor == row) {
            bar.first.f.lo16 = bar_origin.f.lo16;
            bar.first.f.mid16 = bar_origin.f.mid16 + i * 0xB0;
            bar.first.f.mid24 = bar_origin.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1250;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (recollection_state == 1) {
                screen_part_alphas[253] = Menu_PulseValue(screen_part_alphas[253], 0x10, 0x20);
            }
            bar.alpha = (s32)screen_part_alphas[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }

    word.raw = 0x000AAE607C307F80;
    MenuText_SetPen(word);
    for (i = 0; i < LIST_ROWS && i < row_count; i++) {
        counter = recollection_top + i;
        entry = ((s32 *)table + first_text)[rows[counter % row_count]]; // match: the page's first text is added to the table, not to the row
        if (entry != 0) {
            text = table + entry;
            MenuText_SetColor(menu_draw_color);
            MenuText_DrawLineShortened((u16 *)text, 0x1C);
        }
    }

    if ((menu_buttons & PAD_CROSS) && recollection_state == 1 && menu_input_enabled != 0) {
        Sound_PlayMenu(0);
        menu_screen_handler = (void *)Menu_UpdateRecollectionPage;
        context->unk04 = recollection_page * LIST_PAGES + rows[recollection_cursor] + MSG_RECOLLECTION_FIRST;
        menu_input_enabled = 0;
    } else if ((menu_buttons & PAD_CIRCLE) && recollection_state == 1 && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        menu_input_enabled = 0;
        recollection_state = 2;
    }

    switch (recollection_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        screen_part_alphas[253] += 1.6f;
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        for (i = 0; i < image_part_count; i++) {
            screen_part_alphas[i] += image_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > image_parts[i].alpha) {
                for (i = 0; i < image_part_count; i++) {
                    full_alpha = image_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                menu_input_enabled = recollection_state = 1;
                break;
            }
        }
        break;
    case 2:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        screen_part_alphas[254] -= 12.8f;
        if (screen_part_alphas[254] < 0.0f) {
            screen_part_alphas[254] = 0.0f;
        }
        model_draw_color.alpha = (s32)screen_part_alphas[254];
        screen_part_alphas[253] -= 1.6f;
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < image_part_count; i++) {
            screen_part_alphas[i] -= image_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                recollection_top = 0;
                menu_screen_handler = (void *)Menu_OpenMain;
                recollection_cursor = 0;
                recollection_page = 0;
                if (npc_package_load.done != 0) {
                    npc_package_load.done = 0;
                    npc_package_load.stage = 0;
                    model_load_chain = NULL;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else {
                    sceCdBreak();
                    File_Close(model_read_file);
                    model_read_file = -1;
                    npc_package_load.done = 0;
                    npc_package_load.stage = 0;
                    model_load_chain = NULL;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                }
                recollection_state = 0;
                break;
            }
        }
        break;
    }
}

/* The page of one entry (context->unk04, 3000 and up, 50 to a page): its parts, its model once loaded, and three texts. */
static void Menu_UpdateRecollectionPage(HandlerContext *context) {
    f32 view[16];
    f32 local[16];
    ChunkPoolNode model_node;
    GsPackedWord word;
    PackageChunk *chunk;
    u32 *packet;
    u32 i;
    s32 page;
    s32 number;
    s32 table;
    s32 entry;
    u8 kind;

    page = (context->unk04 - 3000) / 50;
    for (i = 0; i < image_part_count; i++) {
        kind = image_parts[i].handler_index;
        Menu_SetBlendMode(image_parts[i].blend_mode);
        MenuSprite_DrawPlain((int)&image_parts[i], kind, 1);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }
    if (npc_package_load.done != 0) {
        memset(view, 0, 0x40);
        view[10] = view[5] = view[0] = 0.8f;
        view[15] = 1.0f;
        sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        sceVu0TransMatrix(local, view, recollection_page_model_position);
        packet = MenuDraw_NextPacket();
        packet[0] = 0x8002;
        packet[1] = 0x10000000;
        packet[2] = 0xE;
        packet[3] = 0;
        packet[4] = 0x7660;
        packet[5] = 0x7890;
        packet[6] = 0x19;
        packet[7] = 0;
        packet[8] = 0x44;
        packet[9] = 0;
        packet[10] = 0x43;
        packet[11] = 0;
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = D_CHCR_STR;
        chunk = (PackageChunk *)((char *)npc_package_load.pool_node->buffer + 0x10);
        for (i = 0; (s32)i < page; i++) {
            chunk = (PackageChunk *)((char *)chunk + chunk->unk0C + chunk->unk08);
        }
        model_load_shown.pool_node = &model_node; // match: a local of this function; the node keeps the address after it returns
        model_load_shown.unk20 = 1;
        model_node.buffer = chunk;
        DRAW_NODE_MODEL(&model_load_shown, local, menu_draw_color);
    }
    table = Message_GetFontText(menu_heap);
    word.raw = 0x000A875081106E80;
    number = page * 50 + MSG_RECOLLECTION_FIRST + 1;
    entry = GetPageTextOffset(table, number + 49);
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawLineCentered((u16 *)(table + entry), 0x17);
    }
    word.raw = 0x000A875083106FE0;
    entry = GetPageTextOffset(table, number + recollection_pages[page] - 1);
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWrappedLong((u16 *)(table + entry), 0x33);
    }
    word.raw = 0x000AAE607C307F80;
    MenuText_SetPen(word);
    entry = ((s32 *)table)[context->unk04 + 1];
    if (entry != 0) {
        if (page == 0) {
            MenuText_DrawWrappedLong((u16 *)(table + entry), 0x1B);
        } else {
            MenuText_DrawWordWrappedLong((u16 *)(table + entry), 0x1B);
        }
    }
    if (menu_buttons & PAD_CIRCLE) {
        Sound_PlayMenu(2);
        context->unk04 = 0;
        menu_screen_handler = (void *)Menu_UpdateRecollection;
        menu_input_enabled = 1;
    }
}

/* per MenuItem.group, the LevelDef number its records start at */
static const TradeGroupBases item_group_bases = { { 150, 0, 220, 220, 220, 220, 220, 220, 220, 220, 0, 0, 0 } };

/* The Visible Item screen (main menu line 5): everything owned in a list of seven under a cursor bar, the turning
   model, the item's text and, when opened, the panel with its values. The confirm button shows or hides
   the item in the other screens' lists (LevelDef.state bit 0); leaving rebuilds the rings. */
static void Menu_UpdateVisibleItem(HandlerContext *context) {
    f32 view[16];
    f32 local[16];
    TradeGroupBases bases;
    MenuCursorBar bar;
    s32 worn_mask;
    s32 worn_bit;
    SpritePacket *packets;
    u32 count;
    MenuItem *selected;
    MenuItem *item;
    MenuItem *node;
    MenuItem *cursor;
    LevelDef *def;
    GsPackedWord corner;
    GsPackedWord position;
    s32 record;
    s32 number;
    s32 row;
    s32 i;
    s32 kind;
    s32 texts;
    s32 text_index;
    s32 text_offset;
    s32 text;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 0.8f;
    view[15] = 1.0f;
    worn_mask = 0;
    worn_bit = 0;
    bases = item_group_bases;
    if (visible_item_state < 2) {
        ((void (*)(HandlerContext *))Menu_MoveAllItemsCursor)(context); // match: the callee takes nothing
    }
    selected = owned_list_current[0];
    item = owned_list_top[0];
    corner.raw = 0x0A09C4007C306FE0;
    if ((menu_buttons & PAD_CROSS) && menu_input_enabled != 0) {
        {
            // match: the value read first is used only to set the bit; the test and the clearing read the field again
            u8 state = item_name_defs[selected->unk0C].state;
            item_name_defs[selected->unk0C].state =
                (item_name_defs[selected->unk0C].state & 1) ? item_name_defs[selected->unk0C].state & ~1 : state | 1;
        }
        Sound_PlayMenu(0);
        number = selected->unk0C;
        if (number == player_state.weapon + ITEM_WEAPON_FIRST) {
            player_state.weapon = 0;
        } else if (number == player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
        } else if (number == player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_ARMOR] = ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
        } else if (number == player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_HELM] = ITEM_WORN_INDEX(ITEM_HELM_NONE);
        } else if (number == player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_GAUNTLET] = ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
        } else if (number == player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_BOOT] = ITEM_WORN_INDEX(ITEM_BOOT_NONE);
        } else if (number == player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_RING] = ITEM_WORN_INDEX(ITEM_RING_NONE);
        } else if (number == player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_BRACELET] = ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
        } else if (number == player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST) {
            player_state.equipment[EQUIP_AMULET] = ITEM_WORN_INDEX(ITEM_AMULET_NONE);
        } else if (number == player_state.shortcut) {
            player_state.shortcut_type = 0;
            player_state.shortcut = 0;
        }
        Player_RecomputeStats();
        if (player_state.hp_max < player_state.hp_now) {
            player_state.hp_now = player_state.hp_max;
        }
        if (player_state.mp_now > player_state.mp_max) {
            player_state.mp_now = player_state.mp_max;
        }
    }
    if (visible_item_state == 2) {
        item = visible_bar_top;
    } else {
        visible_bar_cursor = selected;
        visible_bar_top = item;
    }

    record = owned_list_current[0]->unk0C - bases.first_item[owned_list_current[0]->group];
    D_00446DF4 = item_counts[owned_list_current[0]->unk0C];
    if (owned_list_current[0]->group == 0) {
        D_00446E30 = weapon_level_records[record].level_index + 1;
    }
    for (i = 0; i < recol_part_count; i++) {
        kind = recol_parts[i].handler_index;
        Menu_SetBlendMode(recol_parts[i].blend_mode);
        if (owned_list_current[0]->group != 12) {
            MenuSprite_Draw((int)&recol_parts[i], kind + 1000, 0, (int)screen_part_alphas[i]);
        } else {
            MenuSprite_Draw((int)&recol_parts[i], kind + 1000, 1, (int)screen_part_alphas[i]);
        }
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    for (i = 0; i < 7; i++, item = item->prev) {
        if (item_name_defs[item->unk0C].state & 1) {
            worn_mask |= 1 << worn_bit;
        }
        worn_bit++;
        if (item == visible_bar_cursor) {
            bar.first.f.lo16 = corner.f.lo16;
            bar.first.f.mid16 = corner.f.mid16 + i * 0xB0;
            bar.first.f.mid24 = corner.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (visible_item_state == 1) {
                screen_part_alphas[253] = Menu_PulseValue(screen_part_alphas[253], 0x10, 0x20);
            }
            bar.alpha = (s32)screen_part_alphas[253];
            MenuDraw_Bar(&bar);
        }
        if (owned_list_page_mark[0]->prev == item) {
            break;
        }
    }

    node = owned_list_top[0];
    cursor = owned_list_current[0];
    position.raw = 0x0A09EB107C3071C0;
    if (visible_item_state == 2) {
        node = visible_names_top;
    } else {
        visible_names_cursor = cursor;
        visible_names_top = node;
    }
    for (row = 0; row < 7; row++, node = node->prev) {
        def = &item_name_defs[node->unk0C];
        MenuText_SetPen(position);
        MenuText_SetColor(menu_draw_color);
        MenuText_DrawLine((u16 *)def, 0x1A);
        position.f.mid16 += 0xB0;
        if (owned_list_page_mark[0]->prev == node) {
            break;
        }
    }

    if (visible_item_state != 2) {
        node = owned_list_current[0];
        visible_text_item = node;
    } else {
        node = visible_text_item;
    }
    texts = Message_GetFontText(menu_heap);
    position.raw = 0x000A875083207080;
    text_index = node->unk0C + MSG_ITEM_TEXT_FIRST + 1;
    text_offset = ((s32 *)texts)[text_index];
    MenuText_SetPen(position);
    if (text_offset != 0) {
        text = texts + text_offset;
        MenuText_SetColor(menu_draw_color);
        MenuText_DrawWordWrappedLong((u16 *)text, 0x33);
    }

    for (i = 0; i < item_sel_part_count; i++) {
        kind = item_sel_parts[i].handler_index;
        if (kind != 2) {
            ((void (*)(u8))Menu_SetBlendMode)(item_sel_parts[i].blend_mode); // match: called as returning nothing
            if (visible_item_state != 1 && ((worn_mask >> item_sel_parts[i].slot) & 1)) {
                MenuSprite_Draw((int)&item_sel_parts[i], kind + 1000, worn_mask, (int)screen_part_alphas[i + 200]);
            } else {
                MenuSprite_DrawPlain((int)&item_sel_parts[i], kind, worn_mask);
            }
            Menu_SetBlendMode(0);
        }
    }

    sceVu0ViewScreenMatrix(menu_view_screen_matrix, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    visible_item_model_angle += 3.1415927f;
    visible_item_model_angle -= (s32)(visible_item_model_angle / 6.2831855f) * 6.2831855f;
    visible_item_model_angle -= 3.1415927f;
    sceVu0RotMatrixY(local, view, visible_item_model_angle);
    sceVu0TransMatrix(local, local, visible_item_model_position);
    Menu_DrawItemModel(local);

    if ((menu_buttons & PAD_TRIANGLE) && menu_input_enabled != 0 && Menu_IsEquipSlotFilled(record, owned_list_current[0]->group)) {
        Sound_PlayMenu(1);
        if (detail_panel_open == 0) {
            detail_panel_open = 1;
        } else if (visible_panel_state != 2) {
            visible_panel_state = 2;
        } else {
            visible_panel_state = 0;
        }
    }

    if (detail_panel_open != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
        switch (owned_list_current[0]->group) {
        case 11:
        case 12:
            packets = recol_g_parts;
            count = recol_g_part_count;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            D_00446DF8 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            D_00446DFC = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            D_00446E00 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            D_00446E18 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            D_00446E24 = worn_item_level_records[record].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[owned_list_current[0]->unk0C].weight_tenths_of_pound;
            if (visible_panel_state == 1) {
                FILL_FADE(panel_part_alphas, recol_g_parts, recol_g_part_count)
            }
            DRAW_PACKETS(recol_g_parts, recol_g_part_count, player_weapon[owned_list_current[0]->group])
            packets = recol_g_parts;
            count = recol_g_part_count;
            break;
        case 7:
        case 8:
        case 9:
            D_00446DF8 = ScaleValue(worn_item_level_records[record].gained_values[0] + worn_defs[record].base_values[0], worn_item_level_records[record].level, 100);
            D_00446DFC = ScaleValue(worn_item_level_records[record].gained_values[1] + worn_defs[record].base_values[1], worn_item_level_records[record].level, 100);
            D_00446E00 = ScaleValue(worn_item_level_records[record].gained_values[2] + worn_defs[record].base_values[2], worn_item_level_records[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].base_values[3], worn_item_level_records[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].base_values[4], worn_item_level_records[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].base_values[5], worn_item_level_records[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].base_values[6], worn_item_level_records[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].base_values[7], worn_item_level_records[record].level, 100);
            D_00446E18 = ScaleValue(worn_defs[record].base_values[8], worn_item_level_records[record].level, 100);
            D_00446E1C = item_name_defs[owned_list_current[0]->unk0C].weight_tenths_of_pound;
            if (visible_panel_state == 1) {
                FILL_FADE(panel_part_alphas, recol_g2_parts, recol_g2_part_count)
            }
            DRAW_PACKETS(recol_g2_parts, recol_g2_part_count, player_weapon[owned_list_current[0]->group])
            packets = recol_g2_parts;
            count = recol_g2_part_count;
            break;
        case 0:
            D_00446DF8 = ScaleValue(weapon_level_records[record].gained_values[0] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[0], weapon_level_records[record].level, 100);
            D_00446DFC = ScaleValue(weapon_level_records[record].gained_values[1] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[1], weapon_level_records[record].level, 100);
            D_00446E00 = ScaleValue(weapon_level_records[record].gained_values[2] + weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[2], weapon_level_records[record].level, 100);
            D_00446E04 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[3], weapon_level_records[record].level, 100);
            D_00446E08 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[4], weapon_level_records[record].level, 100);
            D_00446E0C = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[5], weapon_level_records[record].level, 100);
            D_00446E10 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[6], weapon_level_records[record].level, 100);
            D_00446E14 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[7], weapon_level_records[record].level, 100);
            D_00446E18 = ScaleValue(weapon_defs[record].attack_by_level[weapon_level_records[record].level_index].values[8], weapon_level_records[record].level, 100);
            D_00446E20 = (s32)(weapon_defs[record].weapon_length_cm * 0.03281f * 10.0f);
            D_00446E24 = weapon_level_records[record].level;
            D_00446E1C = item_name_defs[owned_list_current[0]->unk0C].weight_tenths_of_pound;
            D_00446E28 = player_weapon[owned_list_current[0]->group];
            D_00446E2C = 100;
            if (visible_panel_state == 1) {
                FILL_FADE(panel_part_alphas, recol_a_parts, recol_a_part_count)
            }
            DRAW_PACKETS(recol_a_parts, recol_a_part_count, 0)
            packets = recol_a_parts;
            count = recol_a_part_count;
            break;
        }

        switch (visible_panel_state) {
        case 0:
            panel_part_alphas[255] += 12.8f;
            if (panel_part_alphas[255] > 128.0f) {
                panel_part_alphas[255] = 128.0f;
            }
            panel_text_color.alpha = (s32)panel_part_alphas[255];
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] += (f32)packets[i].alpha / 10.0f;
                if ((f32)packets[i].alpha < panel_part_alphas[i]) {
                    FILL_FADE(panel_part_alphas, packets, count)
                    visible_panel_state = 1;
                    break;
                }
            }
            break;
        case 2:
            panel_part_alphas[255] -= 12.8f;
            if (panel_part_alphas[255] < 0.0f) {
                panel_part_alphas[255] = 0.0f;
            }
            panel_text_color.alpha = (s32)panel_part_alphas[255];
            for (i = 0; i < count; i++) {
                panel_part_alphas[i] -= (f32)packets[i].alpha / 10.0f;
                if (panel_part_alphas[i] < 0.0f) {
                    memset(panel_part_alphas, 0, 0x400); // match: the panel's state is left at 2 and detail_panel_open set
                    break;
                }
            }
            break;
        }
    }

    if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        visible_item_state = 2;
        menu_input_enabled = 0;
        if (model_load_chain != NULL) {
            model_load_chain->next = NULL;
        }
    }
    if (visible_item_state == 2 && model_load_chain == NULL) {
        visible_item_state = 3;
    }

    switch (visible_item_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        screen_part_alphas[253] += 1.6f;
        if (screen_part_alphas[253] > 16.0f) {
            screen_part_alphas[253] = 16.0f;
        }
        for (i = 0; i < item_sel_part_count; i++) {
            screen_part_alphas[i + 200] += (f32)item_sel_parts[i].alpha / 10.0f;
        }
        for (i = 0; i < recol_part_count; i++) {
            screen_part_alphas[i] += (f32)recol_parts[i].alpha / 10.0f;
            if ((f32)recol_parts[i].alpha < screen_part_alphas[i]) {
                FILL_FADE(screen_part_alphas, recol_parts, recol_part_count)
                for (i = 0; i < item_sel_part_count; i++) {
                    screen_part_alphas[i + 200] = SPRITE_PACKET_COLOR(&item_sel_parts[i])->alpha;
                }
                menu_input_enabled = visible_item_state = 1;
                break;
            }
        }
        break;
    case 3:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        screen_part_alphas[253] -= 1.6f;
        if (screen_part_alphas[253] < 0.0f) {
            screen_part_alphas[253] = 0.0f;
        }
        for (i = 0; i < item_sel_part_count; i++) {
            screen_part_alphas[i + 200] -= (f32)item_sel_parts[i].alpha / 10.0f;
        }
        for (i = 0; i < recol_part_count; i++) {
            screen_part_alphas[i] -= (f32)recol_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                Menu_BuildItemRings();
                menu_screen_handler = (void *)Menu_OpenMain;
                ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                visible_item_state = 0;
                visible_panel_state = 0;
                detail_panel_open = 0;
                break;
            }
        }
        break;
    }
}

/* The System Config screen (main menu line 6): five lines, the first four open the memory card, options, button
   and brightness screens, the last asks yes or no and on yes ends the menu with GameStatus.bit9 set. */
static void Menu_UpdateSystem(MenuScreenContext *context) {
    u32 i;
    s32 line_count = 5;
    u8 kind;
    MenuCursorBar bar;
    PadReport *report;

    if (system_screen_state != 4) {
        if ((menu_buttons & PAD_UP) && menu_input_enabled != 0) {
            PLAY_MENU_SOUND(1);
            system_menu_line--;
        } else if ((menu_buttons & PAD_DOWN) && menu_input_enabled != 0) {
            PLAY_MENU_SOUND(1);
            system_menu_line++;
        }
    } else {
        if ((menu_buttons & PAD_UP) && menu_input_enabled != 0) {
            PLAY_MENU_SOUND(1);
            quit_choice--;
        } else if ((menu_buttons & PAD_DOWN) && menu_input_enabled != 0) {
            PLAY_MENU_SOUND(1);
            quit_choice++;
        }
        if (quit_choice < 0) {
            quit_choice = 1;
        } else if (quit_choice >= 2) {
            quit_choice = 0;
        }
    }
    system_menu_line = (system_menu_line + 5) % line_count;

    for (i = 0; i < system_part_count; i++) {
        kind = system_parts[i].handler_index;
        SET_PART_MODE(system_parts[i].blend_mode);
        if (system_screen_state == 4 && kind == 2) {
            MenuSprite_Draw((int)&system_parts[i], 0x3EA, 0, screen_part_alphas[i]);
        } else if (kind != 2) {
            if (system_screen_state != 1) {
                MenuSprite_Draw((int)&system_parts[i], kind + 1000, system_menu_line, screen_part_alphas[i]);
            } else {
                MenuSprite_Draw((int)&system_parts[i], kind, system_menu_line, screen_part_alphas[i]);
            }
        }
        SET_PART_MODE(0);
    }

    if (system_screen_state == 4) {
        bar.first.f.lo16 = 0x7710;
        bar.first.f.mid16 = quit_choice * 0xC0 + 0x8010;
        bar.first.f.mid24 = 0x86470;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1250;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xC0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        screen_part_alphas[0xFD] = Menu_PulseValue(screen_part_alphas[0xFD], 0x10, 0x20);
        bar.alpha = screen_part_alphas[0xFD];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    report = Pad_GetReport(0, 0);
    if ((report->pressed & PAD_CROSS) && pad_mode != 0xFF && menu_input_enabled != 0) {
        if (system_screen_state != 4) {
            PLAY_MENU_SOUND(0);
            switch (system_menu_line) {
            case 0:
                card_screen_state.save_loaded = 0;
                card_screen_state.card_saves_found = 0;
                card_screen_state.is_leaving = 0;
                card_screen_state.card_op_running = 0;
                card_screen_state.selected_card = 0;
                card_screen_state.selected_save = 0;
                card_screen_state.input_locked = 1;
                card_screen_state.check_finished = 0;
                card_screen_state.save_state_0 = 0;
                card_screen_state.save_state_1 = 0;
                card_screen_state.save_state_2 = 0;
                card_screen_state.card_update_result = 0;
                card_screen_state.confirm_choice = 1;
                card_screen_state.dialog_id = -1;
                SaveCard_Open();
                SaveCard_StartCheck(1);
                system_screen_state = 2;
                menu_input_enabled = 0;
                for (i = 0; i < system_part_count; i++) {
                    if (system_parts[i].handler_index == 1 && system_menu_line == system_parts[i].slot) {
                        screen_part_alphas[i] = pulse_sprite_alpha;
                    }
                }
                break;
            case 1:
            case 2:
            case 3:
                for (i = 0; i < system_part_count; i++) {
                    if (system_parts[i].handler_index == 1 && system_menu_line == system_parts[i].slot) {
                        screen_part_alphas[i] = pulse_sprite_alpha;
                    }
                }
                menu_input_enabled = 0;
                system_screen_state = 2;
                break;
            case 4:
                for (i = 0; i < system_part_count; i++) {
                    if (system_parts[i].handler_index == 1 && system_menu_line == system_parts[i].slot) {
                        screen_part_alphas[i] = pulse_sprite_alpha;
                    }
                }
                system_screen_state = 4;
                break;
            }
        } else if (quit_choice == 0) {
            PLAY_MENU_SOUND(0);
            menu_input_enabled = 0;
            system_screen_state = 5;
        } else {
            PLAY_MENU_SOUND(2);
            system_screen_state = 1;
        }
    } else if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        PLAY_MENU_SOUND(2);
        if (system_screen_state != 4) {
            menu_input_enabled = 0;
            system_screen_state = 3;
        } else {
            system_screen_state = 1;
            quit_choice = 1;
        }
    }

    if (game_status.bit9) {
        context->unk04 = 0;
        system_menu_line = 0;
        menu_screen_handler = (void *)Menu_OpenMain;
    }

    switch (system_screen_state) {
    case 0:
        screen_part_alphas[0xFD] += 1.6f;
        if (screen_part_alphas[0xFD] > 16.0f) {
            screen_part_alphas[0xFD] = 16.0f;
        }
        for (i = 0; i < system_part_count; i++) {
            screen_part_alphas[i] += SPRITE_PACKET_COLOR_VIEW(&system_parts[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&system_parts[i])->alpha < screen_part_alphas[i]) {
                for (i = 0; i < system_part_count; i++) {
                    screen_part_alphas[i] = SPRITE_PACKET_COLOR_VIEW(&system_parts[i])->alpha;
                }
                system_screen_state = 1; // match: before menu_input_enabled, or every register of the step stores moves
                menu_input_enabled = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 5:
    case 6:
        screen_part_alphas[0xFD] -= 1.6f;
        if (screen_part_alphas[0xFD] < 0.0f) {
            screen_part_alphas[0xFD] = 0.0f;
        }
        for (i = 0; i < system_part_count; i++) {
            screen_part_alphas[i] -= SPRITE_PACKET_COLOR_VIEW(&system_parts[i])->alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                screen_part_alphas[i] = 0.0f;
                if (system_parts[i].handler_index != 1 || system_menu_line != system_parts[i].slot) {
                    memset(screen_part_alphas, 0, 0x400);
                    if (system_screen_state == 2) {
                        brightness_before = brightness_option[0];
                        pulse_sprite_alpha = 128.0f;
                        menu_screen_handler = system_menu_screens[system_menu_line];
                        pulse_value_falling = 0;
                        pulse_sprite_falling = 0;
                        if (system_menu_line == 2) {
                            button_settings_copy[0] = forward_button;
                            button_settings_copy[1] = back_button;
                            button_settings_copy[2] = turn_left_button;
                            button_settings_copy[3] = turn_right_button;
                            button_settings_copy[4] = step_left_button;
                            button_settings_copy[5] = step_right_button;
                            button_settings_copy[6] = attack_button;
                            button_settings_copy[7] = magic_button;
                            button_settings_copy[8] = action_button;
                            button_settings_copy[9] = shortcut_button;
                            button_settings_copy[10] = look_down_button;
                            button_settings_copy[11] = look_up_button;
                            button_settings_copy[12] = menu_button;
                            Menu_ReadButtonSettings();
                        }
                    } else if (system_screen_state == 3) {
                        context->unk04 = 0;
                        system_menu_line = 0;
                        menu_screen_handler = (void *)Menu_OpenMain;
                    } else if (system_screen_state == 5) {
                        menu_step++;
                        context->unk04 = 0;
                        system_menu_line = 0;
                        game_status.bit9 = 1;
                    }
                    system_screen_state = 0;
                    quit_choice = 1;
                    break;
                }
            }
        }
        break;
    }
}

/* Moves the cursor of the item list (item_list_cursor) a row or a page, unless the list is locked, shifting the models
   with it. Returns TRUE when a move button was pressed. */
s32 Menu_MoveItemListCursor(HandlerContext *context, s32 locked) {
    MenuItem *cursor = item_list_cursor[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if ((menu_buttons & PAD_UP) && !locked) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == item_list_top[0] && next != item_list_page_mark[0]->prev) {
                    item_list_top[0] = next;
                    item_list_page_mark[0] = item_list_page_mark[0]->next;
                }
                item_list_cursor[0] = next;
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsNext(0);
        } else if ((menu_buttons & PAD_DOWN) && !locked) {
            if (cursor->prev != NULL) {
                if (item_list_page_mark[0]->prev == item_list_cursor[0] && item_list_top[0]->next != item_list_page_mark[0]->prev) {
                    item_list_page_mark[0] = item_list_page_mark[0]->prev;
                    item_list_top[0] = item_list_top[0]->prev;
                }
                Sound_PlayMenu(1);
                item_list_cursor[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((menu_buttons & PAD_RIGHT) && item_list_count >= 8 && !locked) {
            if (cursor->prev != NULL) {
                if (item_list_top[0]->next != item_list_page_mark[0]->prev) {
                    rows = 0;
                    while (ScrollItemListUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageUp(0);
        } else if ((menu_buttons & PAD_LEFT) && item_list_count >= 8 && !locked) {
            if (cursor->next != NULL) {
                if (item_list_top[0]->next != item_list_page_mark[0]->prev) {
                    rows = 0;
                    while (ScrollItemListDown()) {
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
        Menu_SetItemListModelNames();
        return TRUE;
    }
    return FALSE;
}

/* Moves the cursor of the spell list (castable_spell_cursor) a row, shifting the models with it. Returns 1 when a move
   button was pressed. */
static int Menu_MoveSpellCursor(void) {
    MenuItem *cur = castable_spell_cursor[0];
    MenuItem *n;

    if (cur != NULL) {
        if (menu_buttons & PAD_UP) {
            n = cur->next;
            if (n != NULL) {
                if (cur == castable_spell_top[0] && n != castable_spell_page_mark[0]->prev) {
                    castable_spell_top[0] = n;
                    castable_spell_page_mark[0] = castable_spell_page_mark[0]->next;
                }
                Sound_PlayMenu(1);
                castable_spell_cursor[0] = cur->next;
            }
            Menu_ShiftSpellModelsNext(0);
        } else if (menu_buttons & PAD_DOWN) {
            if (cur->prev != NULL) {
                if (cur == castable_spell_page_mark[0]->prev && castable_spell_top[0]->next != cur) {
                    castable_spell_page_mark[0] = cur;
                    castable_spell_top[0] = castable_spell_top[0]->prev;
                }
                Sound_PlayMenu(1);
                castable_spell_cursor[0] = cur->prev;
            }
            Menu_ShiftSpellModelsPrev(0);
        } else {
            return 0;
        }
        Menu_SetSpellModelNames();
        return 1;
    }
    return 0;
}

/* Moves the cursor inside the equipment list shown (buttons 0x80, 0x20), shifting the models with it. Returns 1
   when it moved. */
static int Menu_MoveEquipCursor(void) {
    MenuItem *item = NULL;

    if (equip_list_shown[0] != NULL) {
        item = equip_list_shown[0]->item;
    }
    if (item != NULL) {
        if (menu_buttons & PAD_LEFT) {
            MenuItem *next = item->next;
            if (next != NULL) {
                equip_list_shown[0]->item = next;
                Menu_ShiftEquipModelsNextItem(0, item);
                Sound_PlayMenu(1);
                return 1;
            }
        }
        if (menu_buttons & PAD_RIGHT) {
            MenuItem *prev = item->prev;
            if (prev != NULL) {
                equip_list_shown[0]->item = prev;
                Menu_ShiftEquipModelsPrevItem(0, item);
                Sound_PlayMenu(1);
                return 1;
            }
        }
    }
    return 0;
}

/* Moves the cursor of the list of everything owned (owned_list_current) one row (0x10, 0x40) or one page (0x20, 0x80,
   lists of 8 and more), scrolling the window and shifting the models. */
static s32 Menu_MoveAllItemsCursor(void) {
    MenuItem *cursor = owned_list_current[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if (menu_buttons & PAD_UP) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == owned_list_top[0] && next != owned_list_page_mark[0]->prev) {
                    owned_list_top[0] = next;
                    owned_list_page_mark[0] = owned_list_page_mark[0]->next;
                }
                Sound_PlayMenu(1);
                owned_list_current[0] = cursor->next;
            }
            Menu_ShiftListModelsNext(0);
        } else if (menu_buttons & PAD_DOWN) {
            if (cursor->prev != NULL) {
                if (owned_list_page_mark[0]->prev == cursor && owned_list_top[0]->next != cursor) {
                    owned_list_page_mark[0] = cursor;
                    owned_list_top[0] = owned_list_top[0]->prev;
                }
                Sound_PlayMenu(1);
                owned_list_current[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((menu_buttons & PAD_RIGHT) && owned_list_count >= 8) {
            if (cursor->prev != NULL) {
                if (owned_list_top[0]->next != owned_list_page_mark[0]->prev) {
                    rows = 0;
                    while (ScrollAllItemsUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsPageUp(0);
        } else if ((menu_buttons & PAD_LEFT) && owned_list_count >= 8) {
            if (cursor->next != NULL) {
                if (owned_list_top[0]->next != owned_list_page_mark[0]->prev) {
                    rows = 0;
                    while (ScrollAllItemsDown()) {
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
        Menu_SetAllItemsModelNames();
        return TRUE;
    }
    return FALSE;
}

/* The memory card screen: draws its parts, picks a card slot and starts the load or save screen (Menu_UpdateSaveList). */
static void Menu_UpdateCardSlot(void) {
    u32 i;
    s32 card_result;
    u8 kind;
    PadReport *report;

    card_result = SaveCard_Update();
    if (card_screen_state.check_finished == 0 && SaveCard_IsChecking() == 0) {
        card_screen_state.input_locked = 0;
        card_screen_state.check_finished = 1;
        if (SaveCard_IsPresent(0) == 0 && SaveCard_IsPresent(1) != 0) {
            card_screen_state.selected_card = 1;
        }
    }

    for (i = 0; i < slot_part_count; i++) {
        kind = slot_parts[i].handler_index;
        Menu_SetBlendMode(slot_parts[i].blend_mode);
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 1, screen_part_alphas[i]);
            break;
        case 29:
            if ((u8)slot_parts[i].slot < 2) {
                MenuSprite_Draw((int)&slot_parts[i], kind + 1000, slot_parts[i].slot, screen_part_alphas[i]);
            }
            break;
        case 3:
            switch (card_screen_state.dialog_id) {
            case 4:
            case 5:
            case 9:
            case 10:
                MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0, screen_part_alphas[i]);
                break;
            }
            break;
        case 31:
            if (card_screen_state.check_finished != 0) {
                if (system_screen_state != 1) {
                    MenuSprite_Draw((int)&slot_parts[i], kind + 1000, card_screen_state.selected_card, screen_part_alphas[i]);
                } else {
                    MenuSprite_Draw((int)&slot_parts[i], kind, card_screen_state.selected_card, screen_part_alphas[i]);
                }
            }
            break;
        case 26:
            if (SaveCard_IsPresent(0) == 0) {
                if (system_screen_state == 0) {
                    MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0x20, screen_part_alphas[i]);
                } else if (system_screen_state < 2) {
                    if (menu_mode != 1 || card_slot_leave_steps == 0) {
                        MenuSprite_DrawPlain((int)&slot_parts[i], kind, 0x20);
                    }
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
                    if (menu_mode != 1 || card_slot_leave_steps == 0) {
                        MenuSprite_DrawPlain((int)&slot_parts[i], kind, 0x20);
                    }
                }
            } else if (system_screen_state != 1) {
                MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0x80, screen_part_alphas[i]);
            } else {
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 0x80);
            }
            break;
        case 28:
            switch (card_screen_state.dialog_id) {
            case 4:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 4);
                break;
            case 5:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 5);
                break;
            case 9:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 9);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&slot_parts[i], kind, 10);
                break;
            }
            /* fallthrough */
        default:
            MenuSprite_Draw((int)&slot_parts[i], kind + 1000, 0, screen_part_alphas[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    if (card_screen_state.card_op_running != 0) {
        if (SaveCard_CheckBusy() == 0) {
            card_save_mask = SaveCard_GetSaveMask();
            if (card_save_mask != 0) {
                card_screen_state.input_locked = 1;
                system_screen_state = 2;
                card_screen_state.card_saves_found = 1;
            } else {
                card_screen_state.dialog_id = 9;
            }
            card_screen_state.card_op_running = 0;
        } else if (card_result < 0) {
            if (SaveCard_IsPresent(card_screen_state.selected_card) != 0) {
                card_screen_state.dialog_id = 9;
            } else {
                card_screen_state.dialog_id = 10;
            }
            card_screen_state.card_op_running = 0;
            card_result = 0;
            SaveCard_StartCheck(1);
        }
    }

    if (card_screen_state.is_leaving != 0) {
        if (card_result < 0) {
            card_screen_state.input_locked = 1;
            system_screen_state = 3;
        }
    } else if (card_result < 0) {
        card_screen_state.card_op_running = 0;
        card_screen_state.dialog_id = 10;
        SaveCard_StartCheck(1);
    } else if (card_screen_state.card_op_running == 0 && card_screen_state.input_locked == 0) {
        if ((menu_buttons & (PAD_LEFT | PAD_RIGHT)) && pad_mode != 0xFF && card_screen_state.dialog_id == -1) {
            Sound_PlayMenu(1);
            card_screen_state.selected_card = (card_screen_state.selected_card + 1) & 1;
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & PAD_CROSS) && pad_mode != 0xFF) {
                if (card_screen_state.dialog_id != -1) {
                    Sound_PlayMenu(0);
                    card_screen_state.dialog_id = -1;
                } else if (SaveCard_IsPresent(card_screen_state.selected_card) != 0) {
                    if (SaveCard_CheckSavesIntact() > 0) {
                        if (SaveCard_StartReadList(card_screen_state.selected_card, 1) == 0) {
                            Sound_PlayMenu(0);
                            card_screen_state.card_op_running = 1;
                        }
                    } else {
                        card_screen_state.dialog_id = 9;
                    }
                } else if (card_screen_state.selected_card == 0) {
                    Sound_PlayMenu(0);
                    card_screen_state.dialog_id = 4;
                } else if (card_screen_state.selected_card == 1) {
                    Sound_PlayMenu(0);
                    card_screen_state.dialog_id = 5;
                }
            } else {
                report = Pad_GetReport(0, 0);
                if (((report->pressed & PAD_CIRCLE) && pad_mode != 0xFF) || game_status.bit9) {
                    Sound_PlayMenu(2);
                    if (card_screen_state.dialog_id != -1) {
                        card_screen_state.dialog_id = -1;
                    } else {
                        card_screen_state.is_leaving = 1;
                        SaveCard_Close();
                    }
                }
            }
        }
    }

    switch (system_screen_state) {
    case 0:
        for (i = 0; i < slot_part_count; i++) {
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
                if (card_screen_state.check_finished != 0) {
                    card_screen_state.input_locked = 0;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < slot_part_count; i++) {
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
                if (menu_mode == 1) {
                    card_slot_leave_steps++;
                } else {
                    card_slot_leave_steps = 2;
                }
                if (card_slot_leave_steps == 2) {
                    pulse_sprite_alpha = 128.0f;
                    card_slot_leave_steps = 0;
                    pulse_sprite_falling = 0;
                    if (system_screen_state == 2) {
                        card_screen_state.list_step = -1;
                        menu_screen_handler = (void *)Menu_UpdateSaveList;
                        save_list_opener = (void *)Menu_UpdateCardSlot;
                        card_screen_state.selected_save = 0;
                        card_screen_state.confirm_press_state = 0;
                        SaveCard_ClearLoadFailed(0);
                        SaveCard_ClearLoadFailed(1);
                    } else if (system_screen_state == 3) {
                        if (menu_mode != 1) {
                            menu_screen_handler = (void *)Menu_UpdateSystem;
                        } else {
                            menu_step++;
                        }
                    }
                    card_screen_state.dialog_id = -1;
                    system_screen_state = 0;
                }
                break;
            }
        }
        break;
    }
}

/* The save list of the memory card screen: shows the three saves, moves between them, asks before
   overwriting and starts the save or the load. */
static void Menu_UpdateSaveList(void) {
    CardSaveInfo *info[3];
    MenuCursorBar bar;
    s32 i;
    s32 first_arg;
    s32 second_arg;
    u8 kind;
    s32 slot;
    s32 group;
    s32 table;
    s32 entry;
    GsPackedWord word;
    PadReport *report;

    info[0] = (CardSaveInfo *)SaveCard_GetSaveInfo(0);
    D_00446DF4 = info[0]->level;
    D_00446DF8 = info[0]->experience;
    D_00446DFC = info[0]->gold;
    D_00446E00 = info[0]->hp_now;
    D_00446E04 = info[0]->hp_max;
    D_00446E08 = info[0]->mp_now;
    D_00446E0C = info[0]->mp_max;
    D_00446E10 = info[0]->power_base;
    D_00446E14 = info[0]->magic_base;
    D_00446E18 = info[0]->play_hours;
    D_00446E1C = info[0]->play_minutes;
    D_00446E20 = info[0]->play_seconds;
    info[1] = (CardSaveInfo *)SaveCard_GetSaveInfo(1);
    D_00446E24 = info[1]->level;
    D_00446E28 = info[1]->experience;
    D_00446E2C = info[1]->gold;
    D_00446E30 = info[1]->hp_now;
    D_00446E34 = info[1]->hp_max;
    D_00446E38 = info[1]->mp_now;
    D_00446E3C = info[1]->mp_max;
    D_00446E40 = info[1]->power_base;
    D_00446E44 = info[1]->magic_base;
    D_00446E48 = info[1]->play_hours;
    D_00446E4C = info[1]->play_minutes;
    D_00446E50 = info[1]->play_seconds;
    info[2] = (CardSaveInfo *)SaveCard_GetSaveInfo(2);
    D_00446E54 = info[2]->level;
    D_00446E58 = info[2]->experience;
    D_00446E5C = info[2]->gold;
    D_00446E60 = info[2]->hp_now;
    D_00446E64 = info[2]->hp_max;
    D_00446E68 = info[2]->mp_now;
    D_00446E6C = info[2]->mp_max;
    D_00446E70 = info[2]->power_base;
    D_00446E74 = info[2]->magic_base;
    D_00446E78 = info[2]->play_hours;
    D_00446E7C = info[2]->play_minutes;
    D_00446E80 = info[2]->play_seconds;

    if (card_screen_state.list_step == -1) {
        for (i = 0; i < 3; i++) {
            MENU_SCREEN_SAVES(&card_screen_state)->save_state[i] = SaveCard_GetSaveState(i);
        }
        for (i = 0; i < 3; i++) {
            if (MENU_SCREEN_SAVES(&card_screen_state)->save_state[i] != 1) {
                switch (i) {
                case 0:
                    if (card_screen_state.save_state_1 != 0 || card_screen_state.save_state_2 != 0) {
                        first_arg = 1;
                        second_arg = 0;
                    } else {
                        first_arg = 0;
                        second_arg = 1;
                    }
                    break;
                case 1:
                    if (card_screen_state.save_state_0 != 0 && card_screen_state.save_state_2 != 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (card_screen_state.save_state_0 != 0 && card_screen_state.save_state_2 == 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (card_screen_state.save_state_0 == 0 && card_screen_state.save_state_2 != 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (card_screen_state.save_state_0 == 0 && card_screen_state.save_state_2 == 0) {
                        second_arg = 1;
                        first_arg = 1;
                    }
                    break;
                case 2:
                    if (card_screen_state.save_state_0 != 0 && card_screen_state.save_state_1 != 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (card_screen_state.save_state_0 != 0 && card_screen_state.save_state_1 == 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (card_screen_state.save_state_0 == 0 && card_screen_state.save_state_1 != 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (card_screen_state.save_state_0 == 0 && card_screen_state.save_state_1 == 0) { // match: a last test, not an else: retail keeps a path that sets neither
                        second_arg = 1;
                        first_arg = 2;
                    }
                    break;
                }
                if (SaveCard_HasRoomFor(card_screen_state.selected_card, first_arg, second_arg) > 0) {
                    MENU_SCREEN_SAVES(&card_screen_state)->save_state[i] = 0;
                } else {
                    MENU_SCREEN_SAVES(&card_screen_state)->save_state[i] = -1;
                }
            }
        }
    }

    for (i = 0; i < data_part_count; i++) {
        kind = data_parts[i].handler_index;
        slot = data_parts[i].slot;
        Menu_SetBlendMode(data_parts[i].blend_mode);
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&data_parts[i], 0x3EA, 1, screen_part_alphas[i]);
            break;
        case 3:
            group = slot / 3;
            if (MENU_SCREEN_SAVES(&card_screen_state)->save_state[group] == 1) {
                MenuSprite_Draw((int)&data_parts[i], 0x3EB, group * 3, screen_part_alphas[i]);
            }
            break;
        case 25:
            if (MENU_SCREEN_SAVES(&card_screen_state)->save_state[slot] == 1) {
                MenuSprite_Draw((int)&data_parts[i], 0x401, card_screen_state.selected_save, screen_part_alphas[i]);
            }
            break;
        default:
            MenuSprite_Draw((int)&data_parts[i], kind + 1000, card_screen_state.selected_save, screen_part_alphas[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    table = Message_GetFontText(menu_heap);
    word.raw = 0x000A87507C107120;
    for (i = 0; i < 3; i++) {
        entry = ((s32 *)table)[info[i]->save_area + 1];
        MenuText_SetPen(word);
        MenuText_SetColor(menu_draw_color);
        if (entry != 0) {
            MenuText_DrawWrapped((u16 *)(table + entry), 0x18);
        }
        word.f.mid16 += 0x340;
    }

    for (i = 0; i < mc_part_count; i++) {
        kind = mc_parts[i].handler_index;
        SET_PART_MODE(mc_parts[i].blend_mode);
        if (kind == 3) {
            switch (card_screen_state.list_step) {
            case 8:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 8);
                break;
            case 1:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 1);
                break;
            case 3:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 3);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 10);
                break;
            case 23:
                MenuSprite_DrawPlain((int)&mc_parts[i], kind, 23);
                break;
            }
        } else {
            switch (card_screen_state.list_step) {
            case 1:
            case 3:
            case 8:
            case 10:
            case 23:
                MenuSprite_Draw((int)&mc_parts[i], kind + 1000, 0, screen_part_alphas[i]);
                break;
            }
        }
        SET_PART_MODE(0);
    }

    if (card_screen_state.list_step == 8) {
        bar.first.f.lo16 = 0x7490;
        bar.first.f.mid16 = card_screen_state.confirm_choice * 0xC0 + 0x80A0;
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1720;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        screen_part_alphas[0xFD] = Menu_PulseValue(screen_part_alphas[0xFD], 0x10, 0x20);
        bar.alpha = screen_part_alphas[0xFD];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    card_screen_state.card_update_result = SaveCard_Update();
    if (SaveCard_HasCard(card_screen_state.selected_card) == 0) {
        card_screen_state.card_update_result = -1;
    }
    if (card_screen_state.list_step == 1 && SaveCard_CheckBusy() == 0) {
        card_screen_state.list_step = 3;
    }
    if (card_screen_state.card_update_result < 0) {
        if (SaveCard_GetLoadFailed(card_screen_state.selected_card) != 0 && !game_status.bit9) {
            card_screen_state.list_step = 23;
            SaveCard_ClearLoadFailed(card_screen_state.selected_card);
            SaveCard_StartCheck(0);
        } else if (card_screen_state.list_step != 3) {
            if (!game_status.bit9) {
                card_screen_state.list_step = 10;
            }
        }
    }
    if (game_status.bit9) {
        if (card_screen_state.list_step == -1 && save_list_leave_steps == 0) {
            SaveCard_Init();
            card_screen_state.input_locked = 1;
            system_screen_state = 2;
            save_list_leave_steps = 1;
        }
    }

    report = Pad_GetReport(0, 0);
    if ((report->pressed & PAD_CROSS) && pad_mode != 0xFF) {
        if (card_screen_state.list_step != 1) {
            report = Pad_GetReport(0, 0);
            if (report->held & (PAD_UP | PAD_DOWN)) {
                card_screen_state.confirm_press_state = 1;
            } else {
                card_screen_state.confirm_press_state = 3;
            }
        }
    }
    if (card_screen_state.confirm_press_state == 2) {
        card_screen_state.confirm_press_state = 3;
    }
    if (card_screen_state.confirm_press_state == 1) {
        report = Pad_GetReport(0, 0);
        if (report->held & (PAD_UP | PAD_DOWN)) {
            if (menu_buttons & (PAD_UP | PAD_DOWN)) {
                card_screen_state.confirm_press_state = 2;
            }
        } else {
            card_screen_state.confirm_press_state = 3;
        }
    }

    if (card_screen_state.list_step != 1 && card_screen_state.input_locked == 0) {
        if (card_screen_state.confirm_press_state == 3) {
            card_screen_state.confirm_press_state = 0;
            switch (card_screen_state.list_step) {
            case -1:
                Sound_PlayMenu(0);
                card_screen_state.list_step = 8;
                break;
            case 8:
                if (card_screen_state.confirm_choice == 0) {
                    Sound_PlayMenu(0);
                    card_screen_state.list_step = 1;
                    memset(save_image, 0, 0x20000);
                    SaveCard_StartLoad(card_screen_state.selected_card, card_screen_state.selected_save, (s32 *)save_image, 0x20000);
                } else {
                    Sound_PlayMenu(2);
                    card_screen_state.list_step = -1;
                    card_screen_state.confirm_choice = 1;
                }
                break;
            case 3:
                Sound_PlayMenu(0);
                card_screen_state.list_step = -1;
                break;
            case 10:
                Sound_PlayMenu(0);
                card_screen_state.input_locked = 1;
                if (!game_status.bit9) {
                    system_screen_state = 3;
                    SaveCard_ResetCard(card_screen_state.selected_card);
                } else {
                    card_screen_state.list_step = -1;
                    system_screen_state = 2;
                    save_list_leave_steps = 1;
                }
                break;
            case 23:
                Sound_PlayMenu(0);
                card_screen_state.list_step = -1;
                card_screen_state.confirm_choice = 1;
                break;
            }
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & PAD_CIRCLE) && pad_mode != 0xFF && card_screen_state.confirm_press_state == 0) {
                Sound_PlayMenu(2);
                switch (card_screen_state.list_step) {
                case 10:
                    card_screen_state.input_locked = 1;
                    if (!game_status.bit9) {
                        system_screen_state = 3;
                        SaveCard_ResetCard(card_screen_state.selected_card);
                    } else {
                        card_screen_state.list_step = -1;
                        system_screen_state = 2;
                        save_list_leave_steps = 1;
                    }
                    break;
                case -1:
                    card_screen_state.input_locked = 1;
                    system_screen_state = 3;
                    break;
                case 3:
                    card_screen_state.list_step = -1;
                    break;
                case 8:
                case 23:
                    card_screen_state.list_step = -1;
                    card_screen_state.confirm_choice = 1;
                    break;
                }
            } else if ((menu_buttons & PAD_DOWN) && card_screen_state.input_locked == 0) {
                switch (card_screen_state.list_step) {
                case -1:
                    card_screen_state.selected_save++;
                    for (i = 0; i < 3; i++) {
                        if (SaveCard_GetSaveState((card_screen_state.selected_save + i) % 3) != 0) {
                            break;
                        }
                    }
                    if (i != 2) {
                        Sound_PlayMenu(1);
                    }
                    card_screen_state.selected_save = (card_screen_state.selected_save + i) % 3;
                    break;
                case 8:
                    Sound_PlayMenu(1);
                    card_screen_state.confirm_choice = (card_screen_state.confirm_choice + 1) & 1;
                    break;
                }
            } else if ((menu_buttons & PAD_UP) && card_screen_state.input_locked == 0) {
                switch (card_screen_state.list_step) {
                case -1:
                    card_screen_state.selected_save--;
                    for (i = 0; i < 3; i++) {
                        if (SaveCard_GetSaveState((card_screen_state.selected_save - i + 3) % 3) != 0) {
                            break;
                        }
                    }
                    if (i != 2) {
                        Sound_PlayMenu(1);
                    }
                    card_screen_state.selected_save = (card_screen_state.selected_save - i + 3) % 3;
                    break;
                case 8:
                    Sound_PlayMenu(1);
                    card_screen_state.confirm_choice = (card_screen_state.confirm_choice + 1) & 1;
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
                card_screen_state.input_locked = 0;
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
                save_list_leave_steps++;
                pulse_sprite_falling = 0;
                pulse_sprite_alpha = 128.0f;
                if (system_screen_state == 2) {
                    if (save_list_leave_steps != 2) {
                        card_screen_state.save_loaded = 1;
                        system_screen_state = 0;
                        menu_screen_handler = (void *)Menu_UpdateCardSlot;
                        save_list_leave_steps = 0;
                        MenuHeap_FreeToMark();
                        menu_step++;
                    }
                } else if (system_screen_state == 3) {
                    card_screen_state.confirm_choice = 1;
                    menu_screen_handler = (void *)Menu_UpdateCardSlot;
                    system_screen_state = 0;
                    save_list_leave_steps = 0;
                    SaveCard_StartCheck(0);
                }
                break;
            }
        }
        break;
    }
}

/* The options screen: moves the cursor over its seven lines, changes the option under it, draws the parts, and
   fades the screen in and out. */
static void Menu_UpdateOptions(MenuScreenContext *context) {
    u32 i;
    u8 kind;
    u32 full_alpha;

    if ((menu_buttons & PAD_UP) && menu_input_enabled != 0) {
        context->unk04--;
        Sound_PlayMenu(1);
    } else if ((menu_buttons & PAD_DOWN) && menu_input_enabled != 0) {
        context->unk04++;
        Sound_PlayMenu(1);
    }
    context->unk04 += 7;
    context->unk04 %= 7;

    switch (context->unk04) {
    case 0:
        if ((menu_buttons & (PAD_LEFT | PAD_RIGHT)) && menu_input_enabled != 0) {
            Sound_PlayMenu(1);
            if (sound_mode_change != 1) {
                sound_mode_before = game_options[0];
            }
            sound_mode_change = 1;
            game_options[0]++;
            game_options[0] %= 2;
        } else if (sound_mode_change == 1 && !(menu_buttons & (PAD_LEFT | PAD_RIGHT)) && menu_input_enabled != 0) {
            if (game_options[0] == 0) {
                if (sound_mode_before != 0) {
                    sound_mode_change = 2;
                    SoundDriver_SetMono();
                } else {
                    sound_mode_change = 4;
                }
            } else {
                if (sound_mode_before == 0) {
                    sound_mode_change = 3;
                    SoundDriver_SetStereo();
                } else {
                    sound_mode_change = 4;
                }
            }
        }
        break;
    case 1:
        if ((menu_buttons & PAD_RIGHT) && game_options[1] < 0x7F && menu_input_enabled != 0) {
            game_options[1] += 4;
            Sound_PlayMenu(1);
        } else if ((menu_buttons & PAD_LEFT) && game_options[1] != 0 && menu_input_enabled != 0) {
            game_options[1] -= 4;
            Sound_PlayMenu(1);
        }
        Sound_SetBgmVolume(game_options[1]);
        Sound_SyncBgmLevel();
        break;
    case 2:
        if ((menu_buttons & PAD_RIGHT) && game_options[2] < 0x7F && menu_input_enabled != 0) {
            game_options[2] += 4;
            Sound_PlayMenu(1);
        } else if ((menu_buttons & PAD_LEFT) && game_options[2] != 0 && menu_input_enabled != 0) {
            game_options[2] -= 4;
            Sound_PlayMenu(1);
        }
        break;
    case 3:
        if ((menu_buttons & (PAD_LEFT | PAD_RIGHT)) && menu_input_enabled != 0) {
            game_options[5]++;
            Sound_PlayMenu(1);
        }
        game_options[5] = (s8)game_options[5] % 2;
        break;
    case 4:
        if ((menu_buttons & (PAD_LEFT | PAD_RIGHT)) && menu_input_enabled != 0) {
            game_options[6]++;
            Sound_PlayMenu(1);
        }
        game_options[6] = (s8)game_options[6] % 2;
        break;
    case 5:
        if ((menu_buttons & (PAD_LEFT | PAD_RIGHT)) && menu_input_enabled != 0) {
            Sound_PlayMenu(1);
            game_options[9]++;
        }
        game_options[9] = (s8)game_options[9] % 2;
        break;
    case 6:
        if ((menu_buttons & PAD_RIGHT) && menu_input_enabled != 0) {
            Sound_PlayMenu(1);
            game_options[8]++;
        } else if ((menu_buttons & PAD_LEFT) && menu_input_enabled != 0) {
            Sound_PlayMenu(1);
            game_options[8]--;
        }
        game_options[8] = (s8)(game_options[8] + 3) % 3;
        break;
    }

    for (i = 0; i < option_part_count; i++) {
        kind = option_parts[i].handler_index;
        Menu_SetBlendMode(option_parts[i].blend_mode);
        MenuSprite_Draw((int)&option_parts[i], kind + 1000, context->unk04, (int)screen_part_alphas[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        menu_input_enabled = 0;
        system_screen_state = 2;
    }

    switch (system_screen_state) {
    case 0:
        screen_part_alphas[255] += 12.8f;
        if (screen_part_alphas[255] > 128.0f) {
            screen_part_alphas[255] = 128.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        for (i = 0; i < option_part_count; i++) {
            screen_part_alphas[i] += option_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > option_parts[i].alpha) {
                for (i = 0; i < option_part_count; i++) {
                    full_alpha = option_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                menu_input_enabled = system_screen_state = 1;
                break;
            }
        }
        break;
    case 2:
        screen_part_alphas[255] -= 12.8f;
        if (screen_part_alphas[255] < 0.0f) {
            screen_part_alphas[255] = 0.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[255];
        for (i = 0; i < option_part_count; i++) {
            screen_part_alphas[i] -= option_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                if (menu_mode == 2) {
                    options_leave_steps++;
                } else {
                    options_leave_steps = 2;
                }
                if (options_leave_steps == 2) {
                    options_leave_steps = 0;
                    system_screen_state = 0;
                    context->unk04 = 0;
                    if (sound_mode_change == 1) {
                        if (game_options[0] == 0) {
                            if (sound_mode_before != 0) {
                                sound_mode_change = 2;
                                SoundDriver_SetMono();
                            } else {
                                sound_mode_change = 4;
                            }
                        } else {
                            if (sound_mode_before == 0) {
                                sound_mode_change = 3;
                                SoundDriver_SetStereo();
                            } else {
                                sound_mode_change = 4;
                            }
                        }
                    }
                    if (menu_mode != 2) {
                        menu_screen_handler = (void *)Menu_UpdateSystem;
                    } else {
                        menu_step++;
                    }
                    pulse_sprite_falling = 0;
                    pulse_sprite_alpha = 128.0f;
                }
                break;
            }
        }
        break;
    }
}

/* the choice each action of the button screen starts with */
static const s8 default_button_choices[12] = { 0, 1, 3, 2, 4, 10, 9, 6, 7, 5, 11, 8 };
/* the twelve button settings, in the order of the button screen's lines */
static u16 *const button_settings[12] = {
    &forward_button, &back_button, &turn_left_button, &turn_right_button, &step_left_button, &step_right_button, &attack_button, &magic_button, &action_button, &look_down_button, &look_up_button, (u16 *)&menu_button,
};

/* Fills both 12-entry tables with the index that goes with each slot's pad button bit. */
static void Menu_ReadButtonSettings(void) {
    s32 i;

    for (i = 0; i < 12; i++) {
        switch (*button_settings[i]) {
        case 0x10:
            button_choices_shown[i] = button_choices[i] = 0;
            break;
        case 0x40:
            button_choices_shown[i] = button_choices[i] = 1;
            break;
        case 0x20:
            button_choices_shown[i] = button_choices[i] = 2;
            break;
        case 0x80:
            button_choices_shown[i] = button_choices[i] = 3;
            break;
        case 0x400:
            button_choices_shown[i] = button_choices[i] = 4;
            break;
        case 0x100:
            button_choices_shown[i] = button_choices[i] = 5;
            break;
        case 0x1000:
            button_choices_shown[i] = button_choices[i] = 6;
            break;
        case 0x4000:
            button_choices_shown[i] = button_choices[i] = 7;
            break;
        case 0x2000:
            button_choices_shown[i] = button_choices[i] = 8;
            break;
        case 0x8000:
            button_choices_shown[i] = button_choices[i] = 9;
            break;
        case 0x800:
            button_choices_shown[i] = button_choices[i] = 10;
            break;
        case 0x200:
            button_choices_shown[i] = button_choices[i] = 11;
            break;
        }
    }
}

/* the pad button bit of each of the twelve choices of the button screen */
static const MenuPadBitTable pad_bit_table = { { 0x0010, 0x0040, 0x0020, 0x0080, 0x0400, 0x0100, 0x1000, 0x4000, 0x2000, 0x8000, 0x0800, 0x0200 } };

/* The button screen: picks a button for each of twelve actions, refuses to leave while two actions share one, and fades its parts. */
static void Menu_UpdateButtonConfig(void) {
    MenuPadBitTable pad_bits = pad_bit_table;
    MenuCursorBar bar;
    s32 has_duplicate;
    s32 last_is_shared;
    s32 bar_drawn;
    s32 i;
    s32 j;
    s32 choice_count = 12; // match: the loop bounds are this variable, not a literal (see the note on the first loop)
    s32 bar_mid24 = 0x493E0; // match: a variable; with the literal in the assignment the bar's 64-bit constants are loaded differently
    s32 line_count;
    s32 wrap;
    u8 kind;
    s8 slot;
    s32 uses;
    s32 is_shared;
    s32 other;
    u32 full_alpha;

    if ((menu_buttons & PAD_UP) && menu_input_enabled != 0) {
        if (button_prompt == 0) {
            Sound_PlayMenu(1);
            // match: only this loop, in the function's first run of code, is loop-optimised in retail; the others keep their entry tests until late
            for (i = 0; i < choice_count; i++) {
                FOR_PAD_BIT(*button_settings[i], SET_CHOICE)
            }
            button_line--;
        } else {
            button_prompt_choice = (button_prompt_choice + 1) & 1;
        }
    } else if ((menu_buttons & PAD_DOWN) && menu_input_enabled != 0) {
        if (button_prompt == 0) {
            Sound_PlayMenu(1);
            for (i = 0; i < choice_count; i++) {
                FOR_PAD_BIT(*button_settings[i], SET_CHOICE)
            }
            button_line++;
        } else {
            button_prompt_choice = (button_prompt_choice + 1) & 1;
        }
    }
    line_count = choice_count + 1;
    button_line += line_count;
    button_line %= line_count;

    wrap = choice_count; // match: a second variable, set here, is what retail keeps in $16 for the two divisions
    for (i = 0; i < wrap; i++) {
        if (button_choices[i] == -1) {
            FOR_PAD_BIT(*button_settings[i], SET_SHOWN)
        } else {
            button_choices_shown[i] = button_choices[i];
        }
    }

    if (button_line != choice_count && button_prompt == 0) {
        if ((menu_buttons & PAD_RIGHT) && menu_input_enabled != 0) {
            Sound_PlayMenu(1);
            button_choices[button_line] = (s8)(button_choices[button_line] + 1) % wrap;
        } else if ((menu_buttons & PAD_LEFT) && menu_input_enabled != 0) {
            Sound_PlayMenu(1);
            button_choices[button_line] = (s8)(button_choices[button_line] + 11) % wrap;
        }
    }

    has_duplicate = FALSE;
    for (i = 0; i < choice_count - 1; i++) {
        for (j = i + 1; j < choice_count; j++) {
            if (*button_settings[i] == *button_settings[j]) {
                has_duplicate = TRUE;
                i = choice_count;
                break;
            }
        }
    }

    last_is_shared = FALSE;
    j = 0;
    for (i = 0; i < choice_count; i++) {
        if (*button_settings[11] == *button_settings[i]) {
            j++;
            if (j != 1) {
                last_is_shared = TRUE;
                break;
            }
        }
    }

    D_00446DFC = 0;
    bar_drawn = FALSE;
    D_00446E00 = 0;
    for (i = 0; i < keyconf_part_count; i++) {
        kind = keyconf_parts[i].handler_index;
        slot = keyconf_parts[i].slot;
        if (!bar_drawn && keyconf_parts[i].has_cursor_bar == 1) {
            bar_drawn = TRUE;
            if (button_prompt == 0) {
                bar.first.f.lo16 = 0x6FE0;
                bar.first.f.mid16 = button_line * 0xB0 + 0x7BE0;
                bar.first.f.mid24 = bar_mid24;
                bar.second.f.lo16 = bar.first.f.lo16 + 0xE60;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xA0;
                bar.second.f.mid24 = bar.first.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                if (system_screen_state == 1) {
                    screen_part_alphas[0xFD] = Menu_PulseValue(screen_part_alphas[0xFD], 0x10, 0x20);
                }
                bar.alpha = screen_part_alphas[0xFD];
                MenuDraw_Bar(&bar);
            }
        }
        ((void (*)(u8))Menu_SetBlendMode)(keyconf_parts[i].blend_mode); // match: called as returning nothing, which frees $2 for the switch
        switch (kind) {
        case 2:
            if (button_prompt != 0) {
                MenuSprite_Draw((int)&keyconf_parts[i], kind + 1000, 0, screen_part_alphas[i]);
            }
            break;
        case 3:
            if (button_prompt == 1) {
                MenuSprite_Draw((int)&keyconf_parts[i], kind + 1000, 0, screen_part_alphas[i]);
            } else if (button_prompt == 2) {
                MenuSprite_Draw((int)&keyconf_parts[i], kind + 1000, 1, screen_part_alphas[i]);
            }
            break;
        case 0x18:
            is_shared = FALSE;
            for (other = 0, uses = 0; other < choice_count; other++) {
                if (pad_bits.bits[slot] == *button_settings[other]) {
                    uses++;
                    if (uses != 1) {
                        is_shared = TRUE;
                    }
                }
            }
            if (slot == button_choices_shown[button_line]) {
                D_00446DF4 = 0x20400;
                D_00446DF8 = 0x80FF00;
            } else if (is_shared) {
                D_00446DF4 = 0x4020000;
                D_00446DF8 = 0xFF800000;
                D_00446DFC++;
            } else {
                D_00446DF4 = 0;
                D_00446DF8 = 0;
            }
            MenuSprite_Draw((int)&keyconf_parts[i], kind + 1000, slot, screen_part_alphas[i]);
            break;
        case 0x21:
            if (D_00446E00 == 0) {
                j = i;
            }
            D_00446E04 = button_choices_shown[D_00446E00];
            // match: called as returning a value, which keeps $2 out of the count's increment
            ((int (*)(int, int, int, int))MenuSprite_Draw)((int)&(&keyconf_parts[j])[11 - D_00446E04], kind + 1000, D_00446E00, screen_part_alphas[i]);
            D_00446E00++;
            break;
        default:
            MenuSprite_Draw((int)&keyconf_parts[i], kind + 1000, slot, screen_part_alphas[i]);
            break;
        }
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        if (button_prompt != 0) {
            button_prompt = 0;
            button_prompt_choice = 0;
        } else if (!has_duplicate && !last_is_shared) {
            menu_input_enabled = 0;
            system_screen_state = 2;
        } else if (has_duplicate && last_is_shared) { // match: both arms set the same prompt; retail still tests last_is_shared here
            button_prompt = 1;
        } else {
            button_prompt = 1;
        }
    } else if ((menu_buttons & PAD_CROSS) && menu_input_enabled != 0) {
        Sound_PlayMenu(0);
        if (button_prompt == 0) {
            if (button_line != choice_count) {
                *button_settings[button_line] = pad_bits.bits[button_choices[button_line]];
                button_settings_copy[button_line] = pad_bits.bits[button_choices[button_line]];
            } else {
                for (i = 0; i < choice_count; i++) {
                    button_choices[i] = default_button_choices[i];
                    *button_settings[i] = pad_bits.bits[button_choices[i]];
                }
            }
        } else if (button_prompt == 1) {
            button_prompt = 0;
        } else if (button_prompt == 2) {
            if (button_prompt_choice == 0) {
                system_screen_state = 2;
                menu_input_enabled = 0;
            } else {
                button_prompt = 0;
                button_prompt_choice = 0;
            }
        }
    }

    switch (system_screen_state) {
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
        for (i = 0; i < keyconf_part_count; i++) {
            screen_part_alphas[i] += (f32)keyconf_parts[i].alpha / 10.0f;
            if ((f32)keyconf_parts[i].alpha < screen_part_alphas[i]) {
                for (i = 0; i < keyconf_part_count; i++) {
                    full_alpha = keyconf_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                menu_input_enabled = system_screen_state = 1;
                break;
            }
        }
        break;
    case 2:
        screen_part_alphas[0xFF] -= 12.8f;
        if (screen_part_alphas[0xFF] < 0.0f) {
            screen_part_alphas[0xFF] = 0.0f;
        }
        menu_draw_color.alpha = (s32)screen_part_alphas[0xFF];
        screen_part_alphas[0xFD] -= 1.6f;
        if (screen_part_alphas[0xFD] < 0.0f) {
            screen_part_alphas[0xFD] = 0.0f;
        }
        for (i = 0; i < keyconf_part_count; i++) {
            screen_part_alphas[i] -= (f32)keyconf_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                if (menu_mode == 3) {
                    buttons_leave_steps++;
                } else {
                    buttons_leave_steps = 2;
                }
                if (buttons_leave_steps == 2) {
                    buttons_leave_steps = 0;
                    system_screen_state = 0;
                    if (menu_mode != 3) {
                        menu_screen_handler = (void *)Menu_UpdateSystem;
                    } else {
                        menu_step++;
                    }
                    pulse_sprite_alpha = 128.0f;
                    pulse_sprite_falling = 0;
                    button_line = 0;
                    button_prompt_choice = 0;
                    button_prompt = 0;
                }
                break;
            }
        }
        break;
    }
}

/* Opens the menu screen Menu_UpdateCardCheck runs, unless SaveCard_Open fails. */
static void Menu_OpenCardCheck(void) {
    if (SaveCard_Open() >= 0) {
        SaveCard_StartCheck(1);
        SaveCard_Update();
        memset(screen_part_alphas, 0, 0x400);
        menu_screen_handler = (void *)Menu_UpdateCardCheck;
        card_screen_state.check_step = 0;
        system_screen_state = 0;
        pulse_value_falling = 0;
        card_screen_state.is_leaving = 0;
        card_screen_state.selected_card = 0;
        card_screen_state.check_finished = 0;
        card_screen_state.skip_drawing = 0;
        card_screen_state.card_problem_codes = 0;
        card_screen_state.card0_saves_state = 0;
        card_screen_state.card1_saves_state = 0;
        card_screen_state.dialog_id = -1;
    }
}

/* The menu screen that first checks the two slots, then asks yes or no with a cursor bar, and fades its parts in and out. */
static void Menu_UpdateCardCheck(void) {
    u32 i;
    u8 kind;
    MenuCursorBar bar;
    PadReport *report;
    u32 full_alpha;
    s32 result = ((s32 (*)(void))SaveCard_Update)(); // match: its result is used here

    switch (card_screen_state.check_step) {
    case 0:
        if (SaveCard_IsCheckDone() == 0) {
            return;
        }
        if (SaveCard_IsPresent(0) != 0 && SaveCard_IsFormatted(0) != 0) {
            SaveCard_StartReadList(0, 0);
            card_screen_state.check_step = 1;
        } else if (SaveCard_IsPresent(1) != 0 && SaveCard_IsFormatted(1) != 0) {
            card_screen_state.check_step = 2;
            SaveCard_StartReadList(1, 0);
        } else {
            card_screen_state.check_step = 3;
            SaveCard_StartRescan();
        }
        return;
    case 1:
        if (SaveCard_CheckBusy() == 0) {
            card_screen_state.card0_saves_state = SaveCard_GetSavesState();
            if (SaveCard_IsPresent(1) != 0 && SaveCard_IsFormatted(1) != 0) {
                card_screen_state.check_step = 2;
                SaveCard_StartReadList(1, 0);
            } else {
                card_screen_state.check_step = 3;
                SaveCard_StartRescan();
            }
        } else if (result < 0) {
            card_screen_state.card0_saves_state = 0;
            if (SaveCard_IsPresent(1) != 0 && SaveCard_IsFormatted(1) != 0) {
                card_screen_state.check_step = 2;
                SaveCard_StartReadList(1, 0);
            } else {
                card_screen_state.check_step = 3;
                SaveCard_StartRescan();
            }
        }
        return;
    case 2:
        if (SaveCard_CheckBusy() == 0) {
            card_screen_state.card1_saves_state = SaveCard_GetSavesState();
            card_screen_state.check_step = 3;
            SaveCard_StartRescan();
        } else if (result < 0) {
            card_screen_state.card1_saves_state = 0;
            card_screen_state.check_step = 3;
            SaveCard_StartRescan();
        }
        return;
    case 3:
        if (result > 0) {
            card_screen_state.check_step = 4;
            for (i = 0; (s32)i < 2; i++) {
                switch (SaveCard_GetCardProblem(i)) {
                case 0:
                    card_screen_state.card_problem_codes |= 1 << (i * 4);
                    break;
                case 8:
                    card_screen_state.card_problem_codes |= 2 << (i * 4);
                    break;
                default:
                    card_screen_state.card_problem_codes |= 4 << (i * 4);
                    break;
                }
            }
            if (card_screen_state.card_problem_codes == 0x44) {
                card_screen_state.dialog_id = 0x10;
            } else if (card_screen_state.card_problem_codes == 0x22) {
                card_screen_state.dialog_id = 0x13;
            } else if (card_screen_state.card_problem_codes == 0x42) {
                card_screen_state.dialog_id = 0x11;
            } else if (card_screen_state.card_problem_codes == 0x24) {
                card_screen_state.dialog_id = 0x12;
            } else {
                card_screen_state.skip_drawing = card_screen_state.is_leaving = 1;
                SaveCard_Close();
            }
        }
        return;
    }

    if (card_screen_state.skip_drawing == 0) {
        for (i = 0; i < mc_mess_part_count; i++) {
            kind = mc_mess_parts[i].handler_index;
            ((void (*)(u8))Menu_SetBlendMode)(mc_mess_parts[i].blend_mode); // match: called as returning nothing, which frees $2 for the switch
            switch (card_screen_state.dialog_id) {
            case 0x10:
                MenuSprite_Draw((int)&mc_mess_parts[i], kind + 1000, 0x10, screen_part_alphas[i]);
                break;
            case 0x11:
                MenuSprite_Draw((int)&mc_mess_parts[i], kind + 1000, 0x11, screen_part_alphas[i]);
                break;
            case 0x12:
                MenuSprite_Draw((int)&mc_mess_parts[i], kind + 1000, 0x12, screen_part_alphas[i]);
                break;
            case 0x13:
                MenuSprite_Draw((int)&mc_mess_parts[i], kind + 1000, 0x13, screen_part_alphas[i]);
                break;
            }
            Menu_SetBlendMode(0);
        }
    }

    if (card_screen_state.skip_drawing == 0) {
        bar.first.f.lo16 = 0x75D0;
        bar.first.f.mid16 = card_screen_state.selected_card * 0xE0 + 0x8250;
        bar.first.f.mid24 = 0xDBBA0;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x14A0;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        if (system_screen_state == 1) {
            screen_part_alphas[0xFD] = Menu_PulseValue(screen_part_alphas[0xFD], 0x10, 0x20);
        } else {
            screen_part_alphas[0xFD] = 0.0f;
        }
        bar.alpha = screen_part_alphas[0xFD];
        MenuDraw_Bar(&bar);
    }

    if (card_screen_state.is_leaving != 0) {
        if (result < 0) {
            if (system_screen_state == 2) {
                system_screen_state = 4;
            } else if (system_screen_state == 3) {
                system_screen_state = 5;
            } else if (system_screen_state == 0) {
                MenuHeap_FreeToMark();
                menu_step++;
            }
        }
    } else if (result < 0) {
        card_screen_state.is_leaving = 1;
        SaveCard_StartCheck(1);
        SaveCard_Close();
    } else {
        report = Pad_GetReport(0, 0);
        if ((report->pressed & PAD_CROSS) && pad_mode != 0xFF) {
            Sound_PlayMenu(0);
            card_screen_state.is_leaving = 1;
            SaveCard_StartCheck(1);
            SaveCard_Close();
            if (card_screen_state.selected_card == 0) {
                system_screen_state = 2;
            } else {
                system_screen_state = 3;
            }
        } else if ((menu_buttons & (PAD_UP | PAD_DOWN)) && pad_mode != 0xFF) {
            Sound_PlayMenu(1);
            card_screen_state.selected_card = (card_screen_state.selected_card + 1) & 1;
        }
    }

    if (card_screen_state.skip_drawing != 0) {
        return;
    }
    switch (system_screen_state) {
    case 0:
        for (i = 0; i < mc_mess_part_count; i++) {
            screen_part_alphas[i] += (f32)mc_mess_parts[i].alpha / 10.0f;
            if ((f32)mc_mess_parts[i].alpha < screen_part_alphas[i]) {
                for (i = 0; i < mc_mess_part_count; i++) {
                    full_alpha = mc_mess_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                system_screen_state = 1;
                break;
            }
        }
        break;
    case 4:
    case 5:
        for (i = 0; i < mc_mess_part_count; i++) {
            screen_part_alphas[i] -= (f32)mc_mess_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                card_screen_state.check_finished++;
                if (card_screen_state.check_finished == 1) {
                    break;
                }
                if (system_screen_state == 4) {
                    MenuHeap_FreeToMark();
                    menu_step++;
                } else {
                    menu_step--;
                }
                system_screen_state = 0;
                break;
            }
        }
        break;
    }
}

/* TRUE when the card check (Menu_UpdateCardCheck) found a save on the card of either slot. */
s32 Menu_CardsHaveSaves(void) {
    if (card_screen_state.card0_saves_state > 0 || card_screen_state.card1_saves_state > 0) {
        return TRUE;
    }
    return FALSE;
}

/* The brightness screen: moves option 10 between 0x20 and 0xFC in steps of 4, draws the parts with the slider at
   its place, and fades in and out. */
static void Menu_UpdateBrightness(void) {
    u32 i;
    u8 kind;
    u32 full_alpha;

    if ((menu_buttons & PAD_RIGHT) && menu_input_enabled != 0) {
        Sound_PlayMenu(1);
        if (game_options[10] < 0xFC) {
            game_options[10] += 4;
        } else {
            game_options[10] = 0x20;
        }
    } else if ((menu_buttons & PAD_LEFT) && menu_input_enabled != 0) {
        Sound_PlayMenu(1);
        if (game_options[10] > 0x20) {
            game_options[10] -= 4;
        } else {
            game_options[10] = 0xFC;
        }
    } else if ((menu_buttons & PAD_CROSS) && menu_input_enabled != 0) {
        Sound_PlayMenu(0);
        menu_input_enabled = 0;
        system_screen_state = 2;
    } else if ((menu_buttons & PAD_CIRCLE) && menu_input_enabled != 0) {
        Sound_PlayMenu(2);
        game_options[10] = brightness_before;
        system_screen_state = 3;
        menu_input_enabled = 0;
    }
    brightness_slider = (game_options[10] - 32.0f) * 402.0f / 220.0f;
    for (i = 0; i < bright_part_count; i++) {
        kind = bright_parts[i].handler_index;
        Menu_SetBlendMode(bright_parts[i].blend_mode);
        MenuSprite_Draw((int)&bright_parts[i], kind + 1000, (int)brightness_slider + 0x72C, (int)screen_part_alphas[i]);
        Menu_SetBlendMode(0);
    }
    switch (system_screen_state) {
    case 0:
        for (i = 0; i < bright_part_count; i++) {
            screen_part_alphas[i] += bright_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] > bright_parts[i].alpha) {
                for (i = 0; i < bright_part_count; i++) {
                    full_alpha = bright_parts[i].alpha;
                    screen_part_alphas[i] = full_alpha;
                }
                system_screen_state = 1;
                menu_input_enabled = 1;
                return;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < bright_part_count; i++) {
            screen_part_alphas[i] -= bright_parts[i].alpha / 10.0f;
            if (screen_part_alphas[i] < 0.0f) {
                memset(screen_part_alphas, 0, 0x400);
                pulse_sprite_alpha = 128.0f;
                system_screen_state = 0;
                pulse_sprite_falling = 0;
                if (menu_mode != 5) {
                    menu_screen_handler = (void *)Menu_UpdateSystem;
                } else {
                    menu_step++;
                }
                return;
            }
        }
        break;
    }
}

/* The fifth entry of system_menu_screens: goes back to the main menu on the cancel button. The System Config
   screen never installs it. */
static void Menu_WaitForCancel(void) {
    if (Pad_GetReport(0, 0)->pressed & PAD_CIRCLE) {
        if (pad_mode != 0xFF) {
            menu_screen_handler = (void *)Menu_OpenMain;
            Sound_PlayMenu(2);
        }
    }
}

/* Draws message `index` + the cursor item's number (MSG_ITEM_TEXT_FIRST or MSG_MAGIC_TEXT_FIRST) of the font's text table, if there is one. Returns what the
   text drawer returns, nothing without a text. */
static int Menu_DrawEquipItemText(s32 index) {
    DataNode *node;
    s32 table;
    s32 slot;
    s32 entry;
    GsPackedWord word;

    node = (DataNode *)equip_list_shown[0]->item;
    table = Message_GetFontText(menu_heap);
    slot = node->unk0C + 1;
    entry = ((s32 *)table)[index + slot];
    word.raw = 0x000A87507E9080C0;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuDraw_SetGsRegister(GS_REG_TEST_1, 0x30000);
        return MenuText_DrawWrappedLong((u16 *)(table + entry), 0x1B);
    }
}

/* If the item list has anything: names its five models and chains the five load nodes for them. */
void Menu_ChainItemListModels(void) {
    if (item_list_cursor[0] != 0) {
        Menu_SetItemListModelNames();
        Menu_TakeLoadBuffer(&model_load_shown);
        Menu_TakeLoadBuffer(&model_load_up);
        Menu_TakeLoadBuffer(&model_load_down);
        Menu_TakeLoadBuffer(&model_load_left);
        Menu_TakeLoadBuffer(&model_load_right);
        model_load_shown.next = &model_load_up;
        model_load_shown.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_shown.unk20 = 0;
        model_load_up.next = &model_load_down;
        model_load_up.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_up.unk20 = 0;
        model_load_up.unk1C = 0;
        model_load_down.next = &model_load_left;
        model_load_down.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_down.unk20 = 0;
        model_load_down.unk1C = 0;
        model_load_left.next = &model_load_right;
        model_load_left.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_left.unk20 = 0;
        model_load_left.unk1C = 0;
        model_load_right.next = NULL;
        model_load_right.callback = (void (*)(void))Menu_StepModelLoad;
        model_load_right.unk20 = 0;
        model_load_shown.unk1C = model_load_right.unk1C = 0;
        model_load_chain = &model_load_shown;
    }
}

/* Builds the model file names of the five link nodes: the selected item, its two neighbours, and the items that
   come into view next on either side (found by walking both ways from the page mark, seven steps in all). */
static void Menu_SetItemListModelNames(void) {
    MenuItem *backward;
    MenuItem *forward;
    s32 i;

    forward = backward = item_list_top[0];
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
    sprintf(model_name_shown, "menu/select2/i%04d.mod", item_name_defs[item_list_cursor[0]->unk0C].file_number);
    sprintf(model_name_down, "menu/select2/i%04d.mod", item_name_defs[item_list_cursor[0]->prev->unk0C].file_number);
    sprintf(model_name_up, "menu/select2/i%04d.mod", item_name_defs[item_list_cursor[0]->next->unk0C].file_number);
    sprintf(model_name_left, "menu/select2/i%04d.mod", item_name_defs[forward->unk0C].file_number);
    sprintf(model_name_right, "menu/select2/i%04d.mod", item_name_defs[backward->unk0C].file_number);
}

/* A list cursor went on one row: the model shown moves to node 2, node 1's becomes the one shown, nodes 1, 3
   and 4 are made fresh and the five are chained again. */
void Menu_ShiftListModelsNext(s32 value) {
    LinkNode *pending = NULL;

    if (model_load_chain == &model_load_up || model_load_chain == &model_load_shown) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_down);
    model_load_down = model_load_shown;
    model_load_down.next = NULL;
    if (model_load_down.unk20 == 0) {
        pending = &model_load_down;
    }
    model_load_shown = model_load_up;
    model_load_shown.next = NULL;
    if (model_load_shown.unk20 == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_up);
    model_load_up.unk20 = 0;
    model_load_up.unk1C = 0;
    model_load_up.unk08 = -1;
    model_load_up.next = NULL;
    if (pending == NULL) {
        pending = &model_load_up;
    }
    model_load_right.unk20 = 0;
    model_load_right.unk1C = 0;
    model_load_right.unk08 = -1;
    model_load_right.next = NULL;
    model_load_left.unk20 = 0;
    model_load_left.unk1C = 0;
    model_load_left.unk08 = -1;
    model_load_left.next = NULL;
    if (model_load_chain != NULL) {
        model_load_chain->next = pending;
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = &model_load_right;
    model_load_right.next = &model_load_left;
    model_load_left.next = NULL;
}

/* A list cursor went back one row: the model shown moves to node 1, node 2's becomes the one shown, nodes 2, 3
   and 4 are made fresh and the five are chained again. */
void Menu_ShiftListModelsPrev(s32 value) {
    LinkNode *pending = NULL;

    if (model_load_chain == &model_load_down || model_load_chain == &model_load_shown) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_up);
    model_load_up = model_load_shown;
    model_load_up.next = NULL;
    if (model_load_up.unk20 == 0) {
        pending = &model_load_up;
    }
    model_load_shown = model_load_down;
    model_load_shown.next = NULL;
    if (model_load_shown.unk20 == 0) {
        pending = &model_load_shown;
    }
    Menu_TakeLoadBuffer(&model_load_down);
    model_load_down.unk20 = 0;
    model_load_down.unk1C = 0;
    model_load_down.unk08 = -1;
    model_load_down.next = NULL;
    if (pending == NULL) {
        pending = &model_load_down;
    }
    model_load_right.unk20 = 0;
    model_load_right.unk1C = 0;
    model_load_right.unk08 = -1;
    model_load_right.next = NULL;
    model_load_left.unk20 = 0;
    model_load_left.unk1C = 0;
    model_load_left.unk08 = -1;
    model_load_left.next = NULL;
    if (model_load_chain != NULL) {
        model_load_chain->next = pending;
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = &model_load_left;
    model_load_left.next = &model_load_right;
    model_load_right.next = NULL;
}

/* A list went up a page: node 4's model becomes the one shown, the others are made fresh and the five are
   chained again. */
void Menu_ShiftListModelsPageUp(s32 value) {
    LinkNode *pending = NULL;

    if (model_load_chain == &model_load_right) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_shown);
    model_load_shown = model_load_right;
    model_load_shown.next = NULL;
    if (model_load_shown.unk20 == 0) {
        pending = &model_load_shown;
    }
    model_load_up.unk20 = 0;
    model_load_up.unk1C = 0;
    model_load_up.unk08 = -1;
    model_load_up.next = NULL;
    model_load_down.unk20 = 0;
    model_load_down.unk1C = 0;
    model_load_down.unk08 = -1;
    model_load_down.next = NULL;
    Menu_TakeLoadBuffer(&model_load_right);
    model_load_right.unk20 = 0;
    model_load_right.unk1C = 0;
    model_load_right.unk08 = -1;
    model_load_right.next = NULL;
    model_load_left.unk20 = 0;
    model_load_left.unk1C = 0;
    model_load_left.unk08 = -1;
    model_load_left.next = NULL;
    if (pending == NULL) {
        pending = &model_load_left;
    }
    if (model_load_chain != NULL) {
        model_load_chain->next = pending;
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_left;
    model_load_left.next = &model_load_right;
    model_load_right.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = NULL;
}

/* A list went down a page: node 3's model becomes the one shown, the others are made fresh and the five are
   chained again. */
void Menu_ShiftListModelsPageDown(s32 value) {
    LinkNode *pending = NULL;

    if (model_load_chain == &model_load_left) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&model_load_shown);
    model_load_shown = model_load_left;
    model_load_shown.next = NULL;
    if (model_load_shown.unk20 == 0) {
        pending = &model_load_shown;
    }
    model_load_up.unk20 = 0;
    model_load_up.unk1C = 0;
    model_load_up.unk08 = -1;
    model_load_up.next = NULL;
    model_load_down.unk20 = 0;
    model_load_down.unk1C = 0;
    model_load_down.unk08 = -1;
    model_load_down.next = NULL;
    model_load_right.unk20 = 0;
    model_load_right.unk1C = 0;
    model_load_right.unk08 = -1;
    model_load_right.next = NULL;
    Menu_TakeLoadBuffer(&model_load_left);
    model_load_left.unk20 = 0;
    model_load_left.unk1C = 0;
    model_load_left.unk08 = -1;
    model_load_left.next = NULL;
    if (pending == NULL) {
        pending = &model_load_down;
    }
    if (model_load_chain != NULL) {
        model_load_chain->next = pending;
    } else {
        model_load_chain = pending;
    }
    model_load_shown.next = &model_load_right;
    model_load_right.next = &model_load_left;
    model_load_left.next = &model_load_up;
    model_load_up.next = &model_load_down;
    model_load_down.next = NULL;
}

/* Resets the memory card screens' state, starts the card check and makes Menu_UpdateCardSlot the current handler. */
static void Menu_OpenCardSlot(void) {
    card_screen_state.save_loaded = 0;
    card_screen_state.card_saves_found = 0;
    card_screen_state.is_leaving = 0;
    card_screen_state.card_op_running = 0;
    card_screen_state.selected_card = 0;
    card_screen_state.selected_save = 0;
    card_screen_state.input_locked = 1;
    card_screen_state.check_finished = 0;
    card_screen_state.save_state_0 = 0;
    card_screen_state.save_state_1 = 0;
    card_screen_state.save_state_2 = 0;
    card_screen_state.card_update_result = 0;
    card_screen_state.confirm_choice = 1;
    card_screen_state.dialog_id = -1;
    SaveCard_Open();
    SaveCard_ClearLoadFailed(0);
    SaveCard_ClearLoadFailed(1);
    SaveCard_StartCheck(1);
    menu_screen_handler = (void *)Menu_UpdateCardSlot;
}

/* Tells whether value (an equipment slot's number) differs from the "None" of the slot the given step stands for. */
int Menu_IsEquipSlotFilled(s32 value, u32 step) {
    switch (step) {
        case 0:
        case 1:
        case 2:
        case 11:
            return value != 0;
        case 3:
            return value != ITEM_WORN_INDEX(ITEM_HELM_NONE);
        case 4:
            return value != ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
        case 5:
            return value != ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
        case 6:
            return value != ITEM_WORN_INDEX(ITEM_BOOT_NONE);
        case 7:
            return value != ITEM_WORN_INDEX(ITEM_RING_NONE);
        case 8:
            return value != ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
        case 9:
            return value != ITEM_WORN_INDEX(ITEM_AMULET_NONE);
        default:
            return FALSE;
    }
}

/* Sends a five-quadword packet that draws one sprite in the grey level given, into the frame buffer context1_buffer_half selects. */
void Menu_DrawBrightness(s32 level) {
    GsPacketWord *packet;
    GsPacketWord *cursor;
    u32 colour;

    packet = (GsPacketWord *)MenuDraw_NextPacket();
    packet[0].as_int = 0x8001;
    packet[1].as_int = 0x740B4000;
    packet[2].as_int = 0xF5252160;
    packet[3].as_int = 0;
    packet[4].as_int = 0x16;
    packet[5].as_int = 0;
    // match: the do/while (a macro in the original, most likely) keeps the stores above ahead of the load below
    do {
        cursor = packet + 8;
        if (context1_buffer_half == 0) {
            *(u64 *)&packet[6] = 0x668028000;
        } else {
            *(u64 *)&packet[6] = 0x6680288C0;
        }
    } while (0);
    colour = level | 0x80000000;
    colour |= (level << 16) | (level << 8);
    (cursor++)->as_int = colour;
    (cursor++)->as_float = 1.0f;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x79006C00;
    (cursor++)->as_int = 0xFFFFF;
    (cursor++)->as_float = 0.625f;
    (cursor++)->as_float = 0.4375f;
    cursor[0].as_int = 0x86F89400;
    cursor[1].as_int = 0xFFFFF;
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH));
    *D2_QWC = 5;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN;
}

/* Sets the alpha test and the blend mode of ALPHA_1 from mode and writes them; the value is never seeded. */
int Menu_SetBlendMode(u8 mode) {
    GsAlphaReg reg;

    if (mode == 0) {
        SET_TEST( 0x30000);
        reg.f.a_source = 0;
        reg.f.b_source = 1;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 1) {
        SET_TEST( 0x30000);
        reg.f.a_source = 0;
        reg.f.b_source = 2;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 2) {
        SET_TEST( 0x30000);
        reg.f.a_source = 2;
        reg.f.b_source = 0;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 3) {
        SET_TEST( 3);
        reg.f.a_source = 0;
        reg.f.b_source = 2;
        reg.f.c_source = 0;
        reg.f.d_source = 0;
        reg.f.fix = 0;
    }
    return MenuDraw_SetGsRegister(GS_REG_ALPHA_1, reg.raw);
}

/* Writes TEST_1 (Z test GREATER when flag is set, ALWAYS when not), then sets the blend mode of ALPHA_1 from
   mode (0 to 2) and writes it; the value is never seeded. Nothing calls it. */
s32 Menu_SetBlendModeAndTest(u8 mode, char flag) {
    GsAlphaReg reg;

    if (flag == 0) {
        MenuDraw_SetGsRegister(GS_REG_TEST_1, 0x30000);
    } else {
        MenuDraw_SetGsRegister(GS_REG_TEST_1, 0x70000);
    }
    if (mode == 0) {
        reg.f.a_source = 0;
        reg.f.b_source = 1;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 1) {
        reg.f.a_source = 0;
        reg.f.b_source = 2;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 2) {
        reg.f.a_source = 2;
        reg.f.b_source = 0;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    }
    return MenuDraw_SetGsRegister(GS_REG_ALPHA_1, reg.raw);
}

/* Empties one equipment slot: its menu entry goes back to the first item of its list and the player's number to the "nothing" one. */
void Menu_ClearEquipSlot(s8 slot) {
    switch (slot) {
        case 0:
            *equipped_item_refs[0] = weapon_entries;
            player_state.weapon = 0;
            break;
        case 1:
            *equipped_item_refs[1] = spell_entries;
            player_state.equipped_magic = 0;
            break;
        case 2:
            *equipped_item_refs[2] = shield_entries;
            player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
            break;
        case 3:
            *equipped_item_refs[3] = helm_entries;
            player_state.equipment[EQUIP_HELM] = ITEM_WORN_INDEX(ITEM_HELM_NONE);
            break;
        case 4:
            *equipped_item_refs[4] = armor_entries;
            player_state.equipment[EQUIP_ARMOR] = ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
            break;
        case 5:
            *equipped_item_refs[5] = gauntlet_entries;
            player_state.equipment[EQUIP_GAUNTLET] = ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
            break;
        case 6:
            *equipped_item_refs[6] = boot_entries;
            player_state.equipment[EQUIP_BOOT] = ITEM_WORN_INDEX(ITEM_BOOT_NONE);
            break;
        case 7:
            *equipped_item_refs[7] = ring_entries;
            player_state.equipment[EQUIP_RING] = ITEM_WORN_INDEX(ITEM_RING_NONE);
            break;
        case 8:
            *equipped_item_refs[8] = bracelet_entries;
            player_state.equipment[EQUIP_BRACELET] = ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
            break;
        case 9:
            *equipped_item_refs[9] = amulet_entries;
            player_state.equipment[EQUIP_AMULET] = ITEM_WORN_INDEX(ITEM_AMULET_NONE);
            break;
        case 11:
            *equipped_item_refs[10] = shortcut_entries;
            player_state.shortcut_type = 0;
            player_state.shortcut = 0;
            break;
    }
}

/* Tells whether the item numbered value is the one the player has in the given equipment slot. */
int Menu_IsEquipped(u32 slot, s32 kind, s32 value) {
    switch (slot) {
        case 0:
            if (value == player_state.weapon + ITEM_WEAPON_FIRST) {
                return TRUE;
            }
            break;
        case 1:
            if (value == player_state.equipped_magic) {
                return TRUE;
            }
            break;
        case 2:
            if (value == player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 3:
            if (value == player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 4:
            if (value == player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 5:
            if (value == player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 6:
            if (value == player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 7:
            if (value == player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 8:
            if (value == player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 9:
            if (value == player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST) {
                return TRUE;
            }
            break;
        case 10:
            if ((kind == 11 && player_state.shortcut_type < 2) || (kind == 1 && player_state.shortcut_type == 2)) {
                if (value == player_state.shortcut) {
                    return TRUE;
                }
            }
            break;
    }
    return FALSE;
}
