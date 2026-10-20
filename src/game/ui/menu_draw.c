/* What the menus draw with. One source file holds the menus' heap, the packets sent straight to the GS (sprites,
   words, numbers, the cursor bar and text), the loader and drawer of the menus' models (parts in pool slots, posed
   by a node tree), the texture upload, the sprite handlers that alter a sprite before it is drawn, and the menus'
   reading of the pad. */
#include <libgraph.h>
#include <libvu0.h>
#include <eeregs.h>
#include "ui/menu_draw.h"
#include "player/player.h"
#include "ui/message.h"
#include "model/model.h"
#include "ui/splash.h"
#include "effect/effect.h"
#include "input/pad.h"
#include "gfx/gs.h"

void FlushCache(int); // match: an int result would keep $2 busy after the call (MenuModel_Draw, MenuModel_DrawTinted)
void sceVu0TransMatrix(void *, void *, void *);
int sceOpen(const char *, int);
int sceLseek(int, int, int);
int sceRead(int, void *, int);
int sceClose(int);
int File_Open(char *, int, int);
int File_Close(int);
int File_Read(int, void *, int);
void Vector_WeightedSum4(void *, void *, void *, void *, void *, float *);

/* The module's own variables are defined where the original has them: the initialised ones in front of the
   function that first uses them (retail keeps them in that order), the others here. */
static s64 *color_data;             /* 0x447344 set by MenuSprite_ColorFromNumber and never read */
static s32 heap_mark;               /* 0x447348 what MenuHeap_FreeToMark goes back to */
static s32 heap_top;                /* 0x44734C where the next block of the menus' heap comes from */
static u32 long_text_packets[2][0x3700]; /* 0x5DDF00 the two packet buffers of the long text drawers */

extern s32 menu_heap;              /* the base of the menus' heap */

/* the menus' words and font */
extern u64 *menu_font_page_tex0[];  /* where the TEX0 of each font page is kept */

/* the numbers a menu sprite can stand for (MenuDraw_GetNumber) */
extern PlayerState player_state;
extern ClockTime wall_clock_time;
extern ClockTime current_play_time;
extern s32 menu_number_44;              /* numbers 44 to 54: set by the screen that shows them */
extern s32 menu_number_45;
extern s32 menu_number_46;
extern s32 menu_number_47;
extern s32 menu_number_48;
extern s32 menu_number_49;
extern s32 menu_number_50;
extern s32 menu_number_51;
extern s32 menu_number_52;
extern s32 menu_number_53;
extern s32 menu_number_54;
extern s32 menu_number_70;              /* numbers 70 to 94, likewise */
extern s32 menu_number_71;
extern s32 menu_number_72;              /* number 72; the weapon MenuSprite_TintWeaponStat compares with */
extern s32 menu_number_73;
extern s32 menu_number_74;
extern s32 menu_number_75;
extern s32 menu_number_76;
extern s32 menu_number_77;
extern s32 menu_number_78;
extern s32 menu_number_79;
extern s32 menu_number_80;
extern s32 menu_number_81;
extern s32 menu_number_82;
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
extern s32 menu_number_94;
extern int menu_file_block_count;              /* how many */
extern u16 menu_buttons;              /* this frame's buttons, as the menus read them (MenuPad_Update) */

/* the models: 32 pool slots, each showing one part */
extern void *pool_slot_free_list;   /* the head of the slots' free list */

/* the loaded menu file */

/* what the sprite handlers look at */
extern LevelRecord weapon_level_records[];    /* the weapons' records */
extern PlayerItemDef weapon_defs[];
extern LevelRecord worn_item_level_records[];    /* the worn items' records */
extern LevelRecordDef worn_defs[];
extern FxTableEntry fx_table[];   /* the magic table */
extern u8 game_options[];             /* the options block, a byte per setting */
extern u8 music_volume[];
extern u8 sfx_volume[];
extern u8 game_option_third_bar[];
extern s8 head_bob_option[];
extern s8 rumble_option[];
extern s8 menu_backdrop_option[];
extern s8 hud_option[];
extern f32 pulse_sprite_alpha;              /* the alpha of the pulsing sprite, 80 to 128 */
extern s8 pulse_sprite_falling;               /* TRUE while that alpha falls */


/* 0x428BD0 the matrix the menu model drawer multiplies the light by */
f32 menu_model_light_colors[16] = {
    0.4f, 0.4f, 0.4f, 0.0f,
    0.4f, 0.4f, 0.4f, 0.0f,
    0.4f, 0.4f, 0.4f, 0.0f,
    0.3f, 0.3f, 0.3f, 0.8f,
};

/* 0x428C10 the identity matrix the model drawer uses as it is */
f32 menu_model_light_directions[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

/* 0x428C50 the glyph codes of the menus' words, one after the other */
u16 menu_word_glyphs[2580] = {
    0x0041, 0x001C, 0x0020, 0x0018, 0xFFFF, 0x0040, 0x0018, 0x001F, 0x001F, 0xFFFF, 0x0032, 0x002B, 0x0016, 0x001B,
    0x0014, 0x0021, 0x001A, 0x0018, 0xFFFF, 0x002E, 0x0029, 0x0014, 0x001C, 0x001F, 0x0014, 0x0015, 0x001F, 0x0018,
    0xFFFF, 0x0020, 0x001C, 0x0021, 0x000D, 0xFFFF, 0x0031, 0x0028, 0x0023, 0x001F, 0x001C, 0x0016, 0x0014, 0x0027,
    0x0018, 0x0048, 0x001E, 0x0018, 0x002C, 0x0048, 0x0014, 0x0026, 0x0026, 0x001C, 0x001A, 0x0021, 0x0020, 0x0018,
    0x0021, 0x0027, 0x0026, 0xFFFF, 0x0017, 0x0018, 0x0027, 0x0018, 0x0016, 0x0027, 0x0018, 0x0017, 0x00EC, 0xFFFF,
    0x00BC, 0x00BD, 0x00C2, 0x00F4, 0x00A0, 0x00F4, 0x00D8, 0x0436, 0x02EE, 0x022A, 0xFFFF, 0x0032, 0x002B, 0x0023,
    0xFFFF, 0x0039, 0x0018, 0x0029, 0x0018, 0x001F, 0xFFFF, 0x00A2, 0x00C1, 0x00A7, 0xFFFF, 0x0035, 0x003D, 0xFFFF,
    0x003A, 0x003D, 0xFFFF, 0x003D, 0x001B, 0x002C, 0x0026, 0x001C, 0x0016, 0x0014, 0x001F, 0xFFFF, 0x003A, 0x0014,
    0x001A, 0x001C, 0x0016, 0x0014, 0x001F, 0xFFFF, 0x0034, 0x0022, 0x001F, 0x0017, 0xFFFF, 0x0030, 0x0022, 0x0021,
    0x0017, 0x001C, 0x0027, 0x001C, 0x0022, 0x0021, 0xFFFF, 0x002E, 0x0027, 0x0027, 0x0014, 0x0016, 0x001E, 0xFFFF,
    0x0031, 0x0018, 0x0019, 0x0018, 0x0021, 0x0017, 0xFFFF, 0x011F, 0x014E, 0x0122, 0xFFFF, 0x0040, 0x001F, 0x0014,
    0x0026, 0x001B, 0xFFFF, 0x0035, 0x001C, 0x0027, 0xFFFF, 0x0040, 0x0027, 0x0014, 0x0015, 0xFFFF, 0x0033, 0x001C,
    0x0025, 0x0018, 0xFFFF, 0x0032, 0x0014, 0x0025, 0x0027, 0x001B, 0xFFFF, 0x0044, 0x001C, 0x0021, 0x0017, 0xFFFF,
    0x0044, 0x0014, 0x0027, 0x0018, 0x0025, 0xFFFF, 0x0135, 0xFFFF, 0x0039, 0x0018, 0x0021, 0x001A, 0x0027, 0x001B,
    0xFFFF, 0x0044, 0x0018, 0x001C, 0x001A, 0x001B, 0x0027, 0xFFFF, 0x0041, 0x0022, 0x0027, 0x0014, 0x001F, 0x0048,
    0x0044, 0x0018, 0x001C, 0x001A, 0x001B, 0x0027, 0xFFFF, 0x0033, 0x0041, 0xFFFF, 0x0039, 0x002F, 0xFFFF, 0x00BC,
    0x00BD, 0x00C2, 0x00F4, 0x00A0, 0x00F4, 0x00D8, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0x0077, 0xFFFF, 0x00AB,
    0x00E6, 0x00EB, 0x00A2, 0x0056, 0x005D, 0x004C, 0x0069, 0x0057, 0xFFFF, 0x0110, 0x010D, 0x0146, 0x0063, 0x00C5,
    0x00CE, 0x0079, 0x0173, 0x004E, 0x0085, 0x0071, 0x004C, 0x014C, 0x013C, 0x0060, 0xFFFF, 0x003E, 0x0028, 0x001C,
    0x0027, 0xFFFF, 0x0040, 0x001F, 0x0022, 0x0027, 0x0001, 0xFFFF, 0x0040, 0x001F, 0x0022, 0x0027, 0x0002, 0xFFFF,
    0x0040, 0x0014, 0x0029, 0x0018, 0x0048, 0x0031, 0x0014, 0x0027, 0x0014, 0xFFFF, 0x0039, 0x0022, 0x0014, 0x0017,
    0x0048, 0x0031, 0x0014, 0x0027, 0x0014, 0xFFFF, 0x00A8, 0x00F4, 0x00DB, 0x00D7, 0x00F4, 0x00AA, 0xFFFF, 0x0032,
    0x0020, 0x0023, 0x0027, 0x002C, 0xFFFF, 0x003C, 0x0021, 0xFFFF, 0x003C, 0x0019, 0x0019, 0xFFFF, 0x0040, 0x0022,
    0x0028, 0x0021, 0x0017, 0xFFFF, 0x002F, 0x0034, 0x003A, 0x0048, 0x0043, 0x0022, 0x001F, 0x0028, 0x0020, 0x0018,
    0xFFFF, 0x0040, 0x0032, 0x0048, 0x0043, 0x0022, 0x001F, 0x0028, 0x0020, 0x0018, 0xFFFF, 0x0111, 0x0106, 0x00FB,
    0x0043, 0x003C, 0x0039, 0xFFFF, 0x0111, 0x0106, 0x00AD, 0x00A1, 0x00A7, 0x00AE, 0x00FC, 0x0101, 0xFFFF, 0x0044,
    0x0014, 0x001F, 0x001E, 0x0048, 0x0032, 0x0019, 0x0019, 0x0018, 0x0016, 0x0027, 0xFFFF, 0x0141, 0x0148, 0x0056,
    0x005D, 0x0217, 0x0052, 0x005E, 0x00EE, 0x048B, 0x0149, 0x016F, 0x0063, 0x0249, 0x01AC, 0x0057, 0x0073, 0xFFFF,
    0x01E1, 0x0073, 0x0055, 0x0060, 0x005F, 0x0072, 0x0069, 0x0057, 0xFFFF, 0x002F, 0x0014, 0x0016, 0x001E, 0x001A,
    0x0025, 0x0022, 0x0028, 0x0021, 0x0017, 0xFFFF, 0x0034, 0x0014, 0x0020, 0x0018, 0x0048, 0x0040, 0x0016, 0x0025,
    0x0018, 0x0018, 0x0021, 0xFFFF, 0x010A, 0x0108, 0xFFFF, 0x0033, 0x0022, 0x0025, 0x002A, 0x0014, 0x0025, 0x0017,
    0xFFFF, 0x002F, 0x0014, 0x0016, 0x001E, 0x002A, 0x0014, 0x0025, 0x0017, 0xFFFF, 0x0112, 0x0118, 0x0103, 0x0102,
    0xFFFF, 0x0146, 0x0137, 0x0133, 0x0114, 0xFFFF, 0x0139, 0x0109, 0x0103, 0x0102, 0xFFFF, 0x0129, 0x0119, 0x0103,
    0x0102, 0xFFFF, 0x002E, 0x0027, 0x0027, 0x0014, 0x0016, 0x001E, 0xFFFF, 0x003A, 0x0014, 0x001A, 0x001C, 0x0016,
    0xFFFF, 0x0033, 0x001C, 0x001F, 0x0018, 0x0048, 0x001C, 0x0026, 0x0048, 0x0016, 0x0022, 0x0025, 0x0025, 0x0028,
    0x0023, 0x0027, 0x000D, 0xFFFF, 0x0039, 0x0022, 0x0014, 0x0017, 0x0048, 0x0019, 0x0014, 0x001C, 0x001F, 0x0018,
    0x0017, 0x000D, 0xFFFF, 0x0032, 0x0029, 0x0018, 0x0021, 0x0027, 0xFFFF, 0x0040, 0x001B, 0x0022, 0x0025, 0x0027,
    0x0016, 0x0028, 0x0027, 0xFFFF, 0x0030, 0x0022, 0x0021, 0x0019, 0x001C, 0x0025, 0x0020, 0xFFFF, 0x0040, 0x0027,
    0x0014, 0x0027, 0x0028, 0x0026, 0xFFFF, 0x002F, 0x0014, 0x0016, 0x001E, 0xFFFF, 0x002F, 0x0014, 0x0016, 0x001E,
    0xFFFF, 0x0035, 0x001C, 0x0017, 0x0018, 0x00F3, 0x0040, 0x001B, 0x0022, 0x002A, 0xFFFF, 0x0040, 0x0018, 0x001F,
    0x0018, 0x0016, 0x0027, 0xFFFF, 0x000E, 0xFFFF, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0xFFFF, 0x0009,
    0x0009, 0xFFFF, 0x0009, 0x0009, 0x000D, 0x0009, 0xFFFF, 0x0009, 0x0009, 0x0009, 0xFFFF, 0x011A, 0x0130, 0x011B,
    0x013B, 0x0123, 0x012D, 0x0127, 0x0135, 0xFFFF, 0x011A, 0x0130, 0x011B, 0x013B, 0x0123, 0x012D, 0x0127, 0x0187,
    0x017E, 0xFFFF, 0x0040, 0x001F, 0x0014, 0x0026, 0x001B, 0xFFFF, 0x0035, 0x001C, 0x0027, 0xFFFF, 0x0040, 0x0027,
    0x0014, 0x0015, 0xFFFF, 0x0033, 0x001C, 0x0025, 0x0018, 0xFFFF, 0x0032, 0x0014, 0x0025, 0x0027, 0x001B, 0xFFFF,
    0x0044, 0x001C, 0x0021, 0x0017, 0xFFFF, 0x0044, 0x0014, 0x0027, 0x0018, 0x0025, 0xFFFF, 0x0135, 0xFFFF, 0x0039,
    0x001C, 0x001A, 0x001B, 0x0027, 0xFFFF, 0x0031, 0x0014, 0x0025, 0x001E, 0xFFFF, 0x0044, 0x0018, 0x0014, 0x0023,
    0x0022, 0x0021, 0xFFFF, 0x003A, 0x0014, 0x001A, 0x001C, 0x0016, 0xFFFF, 0x0040, 0x001B, 0x001C, 0x0018, 0x001F,
    0x0017, 0xFFFF, 0x0035, 0x0018, 0x0014, 0x0017, 0xFFFF, 0x0030, 0x001B, 0x0018, 0x0026, 0x0027, 0xFFFF, 0x0035,
    0x0014, 0x0021, 0x0017, 0x0026, 0xFFFF, 0x0039, 0x0018, 0x001A, 0x0026, 0xFFFF, 0x0044, 0x0025, 0x001C, 0x0026,
    0x0027, 0xFFFF, 0x003F, 0x001C, 0x0021, 0x001A, 0xFFFF, 0x003B, 0x0018, 0x0016, 0x001E, 0xFFFF, 0x0040, 0x001B,
    0x0022, 0x0025, 0x0027, 0x0016, 0x0028, 0x0027, 0xFFFF, 0x019D, 0x0186, 0xFFFF, 0x009B, 0x00BA, 0x00E9, 0x00C4,
    0x00EB, 0x00AE, 0xFFFF, 0x00F3, 0xFFFF, 0x021B, 0x0207, 0xFFFF, 0x0043, 0x001C, 0x0015, 0x0025, 0x0014, 0x0027,
    0x001C, 0x0022, 0x0021, 0xFFFF, 0x0125, 0x0149, 0x00A1, 0x00F4, 0x010F, 0x0105, 0xFFFF, 0x002F, 0x0028, 0x002C,
    0xFFFF, 0x0040, 0x0018, 0x001F, 0x001F, 0xFFFF, 0x0202, 0xFFFF, 0x01D7, 0x01D1, 0xFFFF, 0x0164, 0x01DE, 0x01D7,
    0x01D1, 0xFFFF, 0x0041, 0x0022, 0x0027, 0x0014, 0x001F, 0xFFFF, 0x006E, 0x006C, 0x0073, 0xFFFF, 0x003F, 0x0018,
    0x0023, 0x0014, 0x001C, 0x0025, 0xFFFF, 0x0042, 0x0023, 0x001A, 0x0025, 0x0014, 0x0017, 0x0018, 0xFFFF, 0x003F,
    0x0018, 0x0016, 0x0018, 0x001C, 0x0029, 0x0018, 0xFFFF, 0x003B, 0x0032, 0x0044, 0xFFFF, 0x0039, 0x003C, 0x002E,
    0x0031, 0xFFFF, 0x0034, 0x002E, 0x003A, 0x0032, 0xFFFF, 0x003C, 0x003D, 0x0041, 0x0036, 0x003C, 0x003B, 0xFFFF,
    0x0038, 0x0032, 0x0046, 0xFFFF, 0x002E, 0x0040, 0x0040, 0x0036, 0x0034, 0x003B, 0xFFFF, 0x003C, 0x003D, 0x0032,
    0x003B, 0xFFFF, 0x0030, 0x0039, 0x003C, 0x0040, 0x0032, 0xFFFF, 0x0039, 0x0036, 0x0040, 0x0041, 0xFFFF, 0x003E,
    0x0042, 0x0036, 0x0041, 0xFFFF, 0x0041, 0x003C, 0xFFFF, 0x0041, 0x0036, 0x0041, 0x0039, 0x0032, 0xFFFF, 0x003C,
    0x003D, 0x0032, 0x003B, 0xFFFF, 0x003B, 0x0032, 0x0044, 0x0048, 0x0034, 0x002E, 0x003A, 0x0032, 0xFFFF, 0x0039,
    0x003C, 0x002E, 0x0031, 0x0048, 0x0034, 0x002E, 0x003A, 0x0032, 0xFFFF, 0x0038, 0x0032, 0x0046, 0x0048, 0x0039,
    0x0036, 0x0040, 0x0041, 0xFFFF, 0x003E, 0x0042, 0x0036, 0x0041, 0x0048, 0x0041, 0x003C, 0x0048, 0x0041, 0x0036,
    0x0041, 0x0039, 0x0032, 0xFFFF, 0x003C, 0x003D, 0x0032, 0x003B, 0x0048, 0x0038, 0x0032, 0x0046, 0x0048, 0x0039,
    0x0036, 0x0040, 0x0041, 0xFFFF, 0x0030, 0x0039, 0x003C, 0x0040, 0x0032, 0x0048, 0x0038, 0x0032, 0x0046, 0x0048,
    0x0039, 0x0036, 0x0040, 0x0041, 0xFFFF, 0x0034, 0x0042, 0x0036, 0x0031, 0x0032, 0x0048, 0x003C, 0x0033, 0x0033,
    0xFFFF, 0x021B, 0xFFFF, 0x031A, 0x0304, 0x0069, 0x0086, 0x0060, 0x0264, 0x017F, 0x0079, 0x0050, 0x0050, 0x0072,
    0x0069, 0x0057, 0xFFFF, 0x0042, 0x0023, 0x001A, 0x0025, 0x0014, 0x0017, 0x0018, 0xFFFF, 0x0031, 0x0028, 0x0025,
    0x0014, 0xFFFF, 0x003A, 0x003D, 0x0048, 0x0042, 0x0026, 0x0018, 0x0017, 0xFFFF, 0x0032, 0x002B, 0x0016, 0x001B,
    0x0014, 0x0021, 0x001A, 0x0018, 0xFFFF, 0x003B, 0x0018, 0x0018, 0x0017, 0xFFFF, 0x0040, 0x0014, 0x0029, 0x0018,
    0x0048, 0x0031, 0x0014, 0x0027, 0x0014, 0xFFFF, 0x0039, 0x0022, 0x0014, 0x0017, 0x0048, 0x0031, 0x0014, 0x0027,
    0x0014, 0xFFFF, 0x00A7, 0x00C5, 0x00EB, 0x00AE, 0x0001, 0xFFFF, 0x00A7, 0x00C5, 0x00EB, 0x00AE, 0x0002, 0xFFFF,
    0x0044, 0x0014, 0x001F, 0x001F, 0xFFFF, 0x0040, 0x002C, 0x0020, 0x0015, 0x0022, 0x001F, 0xFFFF, 0x0040, 0x0027,
    0x0018, 0x0025, 0x0018, 0x0022, 0xFFFF, 0x003A, 0x0022, 0x0021, 0x0022, 0xFFFF, 0x0041, 0x0028, 0x0025, 0x0021,
    0x0048, 0x0039, 0x0018, 0x0019, 0x0027, 0xFFFF, 0x0041, 0x0028, 0x0025, 0x0021, 0x0048, 0x003F, 0x001C, 0x001A,
    0x001B, 0x0027, 0xFFFF, 0x0040, 0x0027, 0x0025, 0x0014, 0x0019, 0x0018, 0x0048, 0x0039, 0xFFFF, 0x0040, 0x0027,
    0x0025, 0x0014, 0x0019, 0x0018, 0x0048, 0x003F, 0xFFFF, 0x0039, 0x0022, 0x0022, 0x001E, 0x0048, 0x0042, 0x0023,
    0xFFFF, 0x0039, 0x0022, 0x0022, 0x001E, 0x0048, 0x0031, 0x0022, 0x002A, 0x0021, 0xFFFF, 0x0031, 0x0018, 0x0019,
    0x0014, 0x0028, 0x001F, 0x0027, 0xFFFF, 0x002E, 0x0025, 0x0018, 0x0014, 0xFFFF, 0x0041, 0x001C, 0x0020, 0x0018,
    0xFFFF, 0x0040, 0x0014, 0x0029, 0x001C, 0x0021, 0x001A, 0x0048, 0x0017, 0x0014, 0x0027, 0x0014, 0x000D, 0x000D,
    0x000D, 0xFFFF, 0x0031, 0x0022, 0x0048, 0x0021, 0x0022, 0x0027, 0x0048, 0x0025, 0x0018, 0x0020, 0x0022, 0x0029,
    0x0018, 0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A,
    0x003D, 0x0040, 0x0002, 0x000B, 0xFFFF, 0x0039, 0x0022, 0x0014, 0x0017, 0x001C, 0x0021, 0x001A, 0x0048, 0x0017,
    0x0014, 0x0027, 0x0014, 0x000D, 0x000D, 0x000D, 0xFFFF, 0x0031, 0x0022, 0x0048, 0x0021, 0x0022, 0x0027, 0x0048,
    0x0025, 0x0018, 0x0020, 0x0022, 0x0029, 0x0018, 0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048,
    0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0xFFFF, 0x0040, 0x0014, 0x0029, 0x0018,
    0x0048, 0x0016, 0x0022, 0x0020, 0x0023, 0x001F, 0x0018, 0x0027, 0x0018, 0x000D, 0xFFFF, 0x0039, 0x0022, 0x0014,
    0x0017, 0x0048, 0x0016, 0x0022, 0x0020, 0x0023, 0x001F, 0x0018, 0x0027, 0x0018, 0x000D, 0xFFFF, 0x003B, 0x0022,
    0x0048, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D,
    0x0040, 0x0002, 0x000B, 0x0048, 0x001C, 0x0021, 0x0026, 0x0018, 0x0025, 0x0027, 0x0018, 0x0017, 0xFFFF, 0x001C,
    0x0021, 0x0048, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x0048,
    0x0040, 0x001F, 0x0022, 0x0027, 0x0048, 0x0001, 0x000D, 0xFFFF, 0x003B, 0x0022, 0x0048, 0x003A, 0x0018, 0x0020,
    0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0x0048,
    0x001C, 0x0021, 0x0026, 0x0018, 0x0025, 0x0027, 0x0018, 0x0017, 0xFFFF, 0x001C, 0x0021, 0x0048, 0x003A, 0x0018,
    0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x0048, 0x0040, 0x001F, 0x0022, 0x0027,
    0x0048, 0x0002, 0x000D, 0xFFFF, 0x0040, 0x0014, 0x0029, 0x0018, 0x0048, 0x001A, 0x0014, 0x0020, 0x0018, 0x0048,
    0x0017, 0x0014, 0x0027, 0x0014, 0x00ED, 0xFFFF, 0x003C, 0x0029, 0x0018, 0x0025, 0x002A, 0x0025, 0x001C, 0x0027,
    0x0018, 0x00ED, 0xFFFF, 0x0039, 0x0022, 0x0014, 0x0017, 0x0048, 0x0027, 0x001B, 0x001C, 0x0026, 0x0048, 0x0019,
    0x001C, 0x001F, 0x0018, 0x00ED, 0xFFFF, 0x0031, 0x0022, 0x0048, 0x002C, 0x0022, 0x0028, 0x0048, 0x002A, 0x001C,
    0x0026, 0x001B, 0x0048, 0x0027, 0x0022, 0xFFFF, 0x0046, 0x0018, 0x0026, 0xFFFF, 0x003B, 0x0022, 0xFFFF, 0x0032,
    0x0020, 0x0023, 0x0027, 0x002C, 0xFFFF, 0x003B, 0x0022, 0x0048, 0x0038, 0x001C, 0x0021, 0x001A, 0x0499, 0x0026,
    0x0048, 0x0033, 0x001C, 0x0018, 0x001F, 0x0017, 0xFFFF, 0x0017, 0x0014, 0x0027, 0x0014, 0x0048, 0x0019, 0x0022,
    0x0028, 0x0021, 0x0017, 0x000D, 0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014,
    0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0xFFFF, 0x001B, 0x0014, 0x0026, 0x0048, 0x0015, 0x0018,
    0x0018, 0x0021, 0x0048, 0x0025, 0x0018, 0x0020, 0x0022, 0x0029, 0x0018, 0x0017, 0x000D, 0xFFFF, 0x003A, 0x0018,
    0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B,
    0x0048, 0x002A, 0x0014, 0x0026, 0xFFFF, 0x0025, 0x0018, 0x0020, 0x0022, 0x0029, 0x0018, 0x0017, 0x0048, 0x0017,
    0x0028, 0x0025, 0x001C, 0x0021, 0x001A, 0x0048, 0x0026, 0x0014, 0x0029, 0x0018, 0x000D, 0xFFFF, 0x0041, 0x001B,
    0x0018, 0x0048, 0x0029, 0x0014, 0x001F, 0x001C, 0x0017, 0x001C, 0x0027, 0x002C, 0x0048, 0x0022, 0x0019, 0x0048,
    0x0027, 0x001B, 0x0018, 0xFFFF, 0x0026, 0x0014, 0x0029, 0x0018, 0x0017, 0x0048, 0x0038, 0x001C, 0x0021, 0x001A,
    0x0499, 0x0026, 0x0048, 0x0033, 0x001C, 0x0018, 0x001F, 0x0017, 0xFFFF, 0x0017, 0x0014, 0x0027, 0x0014, 0x0048,
    0x001C, 0x0026, 0x0048, 0x0021, 0x0022, 0x0027, 0x0048, 0x001A, 0x0028, 0x0014, 0x0025, 0x0014, 0x0021, 0x0027,
    0x0018, 0x0018, 0x0017, 0x000D, 0xFFFF, 0x0041, 0x001B, 0x001C, 0x0026, 0x0048, 0x003A, 0x0018, 0x0020, 0x0022,
    0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0xFFFF, 0x001C,
    0x0026, 0x0048, 0x0021, 0x0022, 0x0027, 0x0048, 0x0019, 0x0022, 0x0025, 0x0020, 0x0014, 0x0027, 0x0027, 0x0018,
    0x0017, 0x000D, 0xFFFF, 0x0031, 0x0022, 0x0048, 0x002C, 0x0022, 0x0028, 0x0048, 0x002A, 0x001C, 0x0026, 0x001B,
    0x0048, 0x0027, 0x0022, 0x0048, 0x0033, 0x0022, 0x0025, 0x0020, 0x0014, 0x0027, 0x00ED, 0xFFFF, 0x0033, 0x0022,
    0x0025, 0x0020, 0x0014, 0x0027, 0x0027, 0x001C, 0x0021, 0x001A, 0x000D, 0x000D, 0x000D, 0xFFFF, 0x0031, 0x0022,
    0x0048, 0x0021, 0x0022, 0x0027, 0x0048, 0x0025, 0x0018, 0x0020, 0x0022, 0x0029, 0x0018, 0xFFFF, 0x003A, 0x0018,
    0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B,
    0x000D, 0xFFFF, 0x0033, 0x0022, 0x0025, 0x0020, 0x0014, 0x0027, 0x0048, 0x0030, 0x0022, 0x0020, 0x0023, 0x001F,
    0x0018, 0x0027, 0x0018, 0x000D, 0xFFFF, 0x0033, 0x0022, 0x0025, 0x0020, 0x0014, 0x0027, 0x0048, 0x0033, 0x0014,
    0x001C, 0x001F, 0x0018, 0x0017, 0x000D, 0xFFFF, 0x0070, 0x0075, 0x0056, 0x004C, 0x0086, 0x0057, 0x0050, 0x00ED,
    0xFFFF, 0x002E, 0x0027, 0x0027, 0x0025, 0x001C, 0x0015, 0xFFFF, 0x003B, 0x0018, 0x0018, 0x0017, 0x000A, 0x0048,
    0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x000B, 0xFFFF, 0x003C, 0x002A, 0x0021, 0x0018, 0x0017, 0x000A,
    0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x000B, 0xFFFF, 0x0046, 0x0022, 0x0028, 0x0048, 0x001B, 0x0014,
    0x0029, 0x0018, 0x0048, 0x0021, 0x0022, 0xFFFF, 0x0032, 0x0014, 0x0025, 0x0027, 0x001B, 0x0048, 0x0040, 0x0027,
    0x0022, 0x0021, 0x0018, 0x0026, 0x000D, 0xFFFF, 0x002E, 0x0025, 0x0018, 0x0048, 0x002C, 0x0022, 0x0028, 0x0048,
    0x0026, 0x0028, 0x0025, 0x0018, 0x0048, 0x002C, 0x0022, 0x0028, 0x0048, 0x002A, 0x001C, 0x0026, 0x001B, 0x0048,
    0x0027, 0x0022, 0x0048, 0x0024, 0x0028, 0x001C, 0x0027, 0x00ED, 0xFFFF, 0x003B, 0x0022, 0x0048, 0x003A, 0x0018,
    0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B,
    0x0048, 0x001C, 0x0021, 0x0026, 0x0018, 0x0025, 0x0027, 0x0018, 0x0017, 0xFFFF, 0x001C, 0x0021, 0x0048, 0x003A,
    0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x0048, 0x0040, 0x001F, 0x0022,
    0x0027, 0x000D, 0xFFFF, 0x0048, 0xFFFF, 0x003A, 0x001C, 0x0021, 0x000D, 0x0001, 0x0005, 0x0000, 0x0038, 0x002F,
    0x0048, 0x0026, 0x0023, 0x0014, 0x0016, 0x0018, 0x0048, 0x001C, 0x0026, 0x0048, 0x0025, 0x0018, 0x0024, 0x0028,
    0x001C, 0x0025, 0x0018, 0x0017, 0xFFFF, 0x0027, 0x0022, 0x0048, 0x0026, 0x0014, 0x0029, 0x0018, 0x0048, 0x0038,
    0x001C, 0x0021, 0x001A, 0x0499, 0x0026, 0x0048, 0x0033, 0x001C, 0x0018, 0x001F, 0x0017, 0x0048, 0x0017, 0x0014,
    0x0027, 0x0014, 0x000D, 0xFFFF, 0x01C9, 0x0051, 0x0495, 0x010E, 0x0079, 0x03A5, 0x03C4, 0x0086, 0x0057, 0xFFFF,
    0x003B, 0x0022, 0x0027, 0x0048, 0x0018, 0x0021, 0x0022, 0x0028, 0x001A, 0x001B, 0x0048, 0x0026, 0x0023, 0x0014,
    0x0016, 0x0018, 0x0048, 0x0022, 0x0021, 0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030,
    0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0x0048, 0x001C, 0x0021, 0xFFFF, 0x003A, 0x0018,
    0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x0048, 0x0040, 0x001F, 0x0022, 0x0027,
    0x0048, 0x0001, 0x000D, 0xFFFF, 0x0048, 0xFFFF, 0x003A, 0x001C, 0x0021, 0x000D, 0x0001, 0x0005, 0x0000, 0x0038,
    0x002F, 0x0048, 0x0026, 0x0023, 0x0014, 0x0016, 0x0018, 0x0048, 0x001C, 0x0026, 0x0048, 0x0025, 0x0018, 0x0024,
    0x0028, 0x001C, 0x0025, 0x0018, 0x0017, 0xFFFF, 0x0027, 0x0022, 0x0048, 0x0026, 0x0014, 0x0029, 0x0018, 0x0048,
    0x0038, 0x001C, 0x0021, 0x001A, 0x0499, 0x0026, 0x0048, 0x0033, 0x001C, 0x0018, 0x001F, 0x0017, 0x0048, 0x0017,
    0x0014, 0x0027, 0x0014, 0x000D, 0xFFFF, 0x003B, 0x0022, 0x0027, 0x0048, 0x0018, 0x0021, 0x0022, 0x0028, 0x001A,
    0x001B, 0x0048, 0x0026, 0x0023, 0x0014, 0x0016, 0x0018, 0x0048, 0x0022, 0x0021, 0xFFFF, 0x003A, 0x0018, 0x0020,
    0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0x0048,
    0x001C, 0x0021, 0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017,
    0x0048, 0x0040, 0x001F, 0x0022, 0x0027, 0x0048, 0x0002, 0x000D, 0xFFFF, 0x0048, 0xFFFF, 0x003A, 0x001C, 0x0021,
    0x000D, 0x0001, 0x0005, 0x0000, 0x0038, 0x002F, 0x0048, 0x0026, 0x0023, 0x0014, 0x0016, 0x0018, 0x0048, 0x001C,
    0x0026, 0x0048, 0x0025, 0x0018, 0x0024, 0x0028, 0x001C, 0x0025, 0x0018, 0x0017, 0xFFFF, 0x0027, 0x0022, 0x0048,
    0x0026, 0x0014, 0x0029, 0x0018, 0x0048, 0x0038, 0x001C, 0x0021, 0x001A, 0x0499, 0x0026, 0x0048, 0x0033, 0x001C,
    0x0018, 0x001F, 0x0017, 0x0048, 0x0017, 0x0014, 0x0027, 0x0014, 0x000D, 0xFFFF, 0x003B, 0x0022, 0x0027, 0x0048,
    0x0018, 0x0021, 0x0022, 0x001A, 0x001B, 0x0048, 0x0026, 0x0023, 0x0014, 0x0016, 0x0018, 0x0048, 0x0022, 0x0021,
    0x0048, 0x0018, 0x001C, 0x0027, 0x001B, 0x0018, 0x0025, 0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C,
    0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0x0048, 0x001C, 0x0021, 0xFFFF,
    0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0x0048, 0x0040, 0x001F,
    0x0022, 0x0027, 0x0048, 0x0001, 0x0048, 0x0022, 0x0025, 0x0048, 0x0040, 0x001F, 0x0022, 0x0027, 0x0048, 0x0002,
    0x000D, 0xFFFF, 0x0048, 0xFFFF, 0x003A, 0x001C, 0x0021, 0x000D, 0x0001, 0x0005, 0x0000, 0x0038, 0x002F, 0x0048,
    0x0026, 0x0023, 0x0014, 0x0016, 0x0018, 0x0048, 0x001C, 0x0026, 0x0048, 0x0025, 0x0018, 0x0024, 0x0028, 0x001C,
    0x0025, 0x0018, 0x0017, 0xFFFF, 0x0027, 0x0022, 0x0048, 0x0026, 0x0014, 0x0029, 0x0018, 0x0048, 0x0038, 0x001C,
    0x0021, 0x001A, 0x0499, 0x0026, 0x0048, 0x0033, 0x001C, 0x0018, 0x001F, 0x0017, 0x0048, 0x0017, 0x0014, 0x0027,
    0x0014, 0x000D, 0xFFFF, 0x01C9, 0x0051, 0x0495, 0x010E, 0x0079, 0x03A5, 0x03C4, 0x0086, 0x0057, 0xFFFF, 0x0034,
    0x0014, 0x0028, 0x001A, 0x0018, 0xFFFF, 0x003C, 0x002A, 0x0021, 0x0018, 0x0017, 0xFFFF, 0x003A, 0x003D, 0x0048,
    0x0039, 0x0018, 0x0019, 0x0027, 0xFFFF, 0x002E, 0x0029, 0x0014, 0x001C, 0x001F, 0x0014, 0x0015, 0x001F, 0x0018,
    0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014, 0x0025, 0x0017, 0xFFFF, 0x0040,
    0x001F, 0x0022, 0x0027, 0x0048, 0x0001, 0xFFFF, 0x0040, 0x001F, 0x0022, 0x0027, 0x0048, 0x0002, 0xFFFF, 0x003B,
    0x0022, 0x0027, 0x0048, 0x0018, 0x0021, 0x0022, 0x0028, 0x001A, 0x001B, 0x0048, 0x0026, 0x0023, 0x0014, 0x0016,
    0x0018, 0x0048, 0x0022, 0x0021, 0xFFFF, 0x003A, 0x0018, 0x0020, 0x0022, 0x0025, 0x002C, 0x0048, 0x0030, 0x0014,
    0x0025, 0x0017, 0x000A, 0x003D, 0x0040, 0x0002, 0x000B, 0x000D, 0xFFFF, 0x0048, 0xFFFF, 0x003A, 0x001C, 0x0021,
    0x000D, 0x0009, 0x0000, 0x0038, 0x002F, 0x0048, 0x0026, 0x0023, 0x0014, 0x0016, 0x0018, 0x0048, 0x001C, 0x0026,
    0xFFFF, 0x0025, 0x0018, 0x0024, 0x0028, 0x001C, 0x0025, 0x0018, 0x0017, 0x0048, 0x0027, 0x0022, 0x0048, 0x0026,
    0x0014, 0x0029, 0x0018, 0x0048, 0x0014, 0x0048, 0x0021, 0x0018, 0x002A, 0x0048, 0x0019, 0x001C, 0x001F, 0x0018,
    0x000D, 0xFFFF, 0x003A, 0x0018, 0x0021, 0x0028, 0xFFFF, 0x003F, 0x0018, 0x0016, 0x001B, 0x0018, 0x0016, 0x001E,
    0xFFFF, 0x003C, 0x0038, 0xFFFF, 0x00BC, 0x00B0, 0x00E9, 0x00F4, 0x0063, 0x00A1, 0x00F4, 0x0125, 0x0149, 0x0079,
    0xFFFF, 0x0388, 0x0063, 0x00A1, 0x00F4, 0x0125, 0x0149, 0x005E, 0x0107, 0x0340, 0x0056, 0x005D, 0x004C, 0x0069,
    0x0057, 0xFFFF, 0x002F, 0x0022, 0x0021, 0x0018, 0x0026, 0xFFFF, 0x0040, 0x0016, 0x0014, 0x001F, 0x0018, 0x0026,
    0xFFFF, 0x0030, 0x001F, 0x0014, 0x002A, 0x0026, 0xFFFF, 0x0054, 0x0063, 0x0069, 0x0069, 0x01FA, 0x0148, 0x0056,
    0x0069, 0x0057, 0x0050, 0x00ED, 0xFFFF, 0x01FA, 0x0148, 0xFFFF, 0x00A1, 0x00E8, 0x00C8, 0x00A8, 0x00C3, 0xFFFF,
    0x01C9, 0x0051, 0x0495, 0x010E, 0x0079, 0x01FF, 0x019A, 0x0056, 0x005D, 0x004C, 0x0069, 0x0057, 0xFFFF, 0x002F,
    0x0028, 0x002C, 0x0000, 0x0000,
};

/* 0x42A078 where each word starts in menu_word_glyphs */
u16 menu_word_starts[268] = {
    0x0000, 0x0005, 0x000A, 0x0013, 0x001D, 0x0022, 0x003C, 0x0046, 0x0051, 0x0055, 0x005B, 0x005F, 0x0062, 0x0065,
    0x006E, 0x0076, 0x007B, 0x0085, 0x008C, 0x0093, 0x0097, 0x009D, 0x00A1, 0x00A6, 0x00AB, 0x00B1, 0x00B6, 0x00BC,
    0x00BE, 0x00C5, 0x00CC, 0x00D9, 0x00DC, 0x00DF, 0x00ED, 0x00F7, 0x0107, 0x010C, 0x0112, 0x0118, 0x0122, 0x012C,
    0x0133, 0x0139, 0x013C, 0x0140, 0x0146, 0x0151, 0x015B, 0x0162, 0x016B, 0x0177, 0x0188, 0x0191, 0x019C, 0x01A8,
    0x01AB, 0x01B3, 0x01BC, 0x01C1, 0x01C6, 0x01CB, 0x01D0, 0x01D7, 0x01DD, 0x01EE, 0x01FB, 0x0201, 0x020A, 0x0212,
    0x0219, 0x021E, 0x0223, 0x022D, 0x0234, 0x0236, 0x023D, 0x0240, 0x0245, 0x0249, 0x0252, 0x025C, 0x0262, 0x0266,
    0x026B, 0x0270, 0x0276, 0x027B, 0x0281, 0x0283, 0x0289, 0x028E, 0x0295, 0x029B, 0x02A2, 0x02A7, 0x02AD, 0x02B3,
    0x02B8, 0x02BE, 0x02C3, 0x02C8, 0x02D1, 0x02D4, 0x02DB, 0x02DD, 0x02E0, 0x02EA, 0x02F1, 0x02F5, 0x02FA, 0x02FC,
    0x02FF, 0x0304, 0x030A, 0x030E, 0x0315, 0x031D, 0x0325, 0x0329, 0x032E, 0x0333, 0x033A, 0x033E, 0x0345, 0x034A,
    0x0350, 0x0355, 0x035A, 0x035D, 0x0363, 0x0368, 0x0371, 0x037B, 0x0384, 0x0392, 0x03A0, 0x03AF, 0x03B9, 0x03BB,
    0x03C9, 0x03D1, 0x03D6, 0x03DE, 0x03E7, 0x03EC, 0x03F6, 0x0400, 0x0406, 0x040C, 0x0411, 0x0418, 0x041F, 0x0424,
    0x042E, 0x0439, 0x0442, 0x044B, 0x0453, 0x045D, 0x0465, 0x046A, 0x046F, 0x047E, 0x048C, 0x049D, 0x04AD, 0x04BB,
    0x04CC, 0x04DB, 0x04EA, 0x0507, 0x051E, 0x053B, 0x0552, 0x0562, 0x056D, 0x057D, 0x058C, 0x0590, 0x0593, 0x0599,
    0x05A9, 0x05B5, 0x05C6, 0x05D8, 0x05ED, 0x0602, 0x0616, 0x0629, 0x0641, 0x0657, 0x0669, 0x0680, 0x068E, 0x069C,
    0x06AE, 0x06BF, 0x06CE, 0x06D7, 0x06DE, 0x06EC, 0x06FA, 0x0706, 0x0714, 0x0733, 0x0750, 0x0765, 0x0767, 0x0783,
    0x079E, 0x07A8, 0x07BC, 0x07D0, 0x07E4, 0x07E6, 0x0802, 0x081D, 0x0831, 0x0845, 0x0859, 0x085B, 0x0877, 0x0892,
    0x08AC, 0x08C0, 0x08DE, 0x08E0, 0x08FC, 0x0917, 0x0921, 0x0927, 0x092D, 0x0935, 0x093F, 0x094B, 0x0952, 0x0959,
    0x096D, 0x097F, 0x0981, 0x0993, 0x09B0, 0x09B5, 0x09BD, 0x09C0, 0x09CB, 0x09DA, 0x09E0, 0x09E7, 0x09ED, 0x09F9,
    0x09FC, 0x0A02, 0x0A0F, 0x0A12, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000,
};

/* 0x42A290 the four ways the pool's slots are drawn */
void (*pool_slot_drawers[4])(RenderSlot *, RenderSlotArg) = {
    PoolSlot_DrawAll,
    PoolSlot_DrawFirst20,
    PoolSlot_DrawAllBut15,
    PoolSlot_DrawFirst20But15,
};

/* 0x42A2A0 one entry per block of the loaded menu file (menu_file_block_count of them) */
LoadedBlockRef menu_file_blocks[66] = { { 0 } };

/* 0x42A8D0 the loaded menu font and the two words MenuTexture_GetRegisters reads from it; the second
   (menu_font_second_tex0) is the TEX0 of the font's second page of glyphs */
LoadedTexture menu_font_texture = { 0 };
asm(".globl menu_font_second_tex0\nmenu_font_second_tex0 = menu_font_texture + 8\n");

/* 0x42A8E8 8 zero bytes after the loaded font the code never refers to */
static u64 menu_font_texture_pad __attribute__((section(".data"))) = 0;

/* 0x42A8F0 the 32 pool slots, chained into a free list through their next pointers */
PoolSlot pool_slots[32] = { { 0 } };

/* 0x42B6F0 0x50 zero bytes after the pool the code never refers to */
static PoolPart pool_slots_tail_pad = { 0 };

/* 0x42B740 where PoolSlot_InitFreeList keeps the address of the slot at pool_slot_spare_buffer */
void *pool_slot_buffer_table[8] = { 0 };

/* 0x42B760 the slot-shaped buffer pool_slot_buffer_table points at */
char pool_slot_spare_buffer[0x70] = { 0 };

/* 0x42B7D0 the view-to-screen matrix the model drawer fills in */
f32 menu_view_screen_matrix[16] = { 0 };

/* the two packet buffers in the scratchpad */
#define PACKET_BUFFER_0 ((void *)((char *)POOL_DRAW_SCRATCH + 0x800))
#define PACKET_BUFFER_1 ((void *)((char *)POOL_DRAW_SCRATCH + 0x2400))
#define POOL(slot) ((PoolSlot *)(slot))
#define HEADER_TEX0(header) ((BootTex0 *)&(header)->tex0)
#define ANGLE_PI 3.1415927f
#define ANGLE_TURN 6.283184f
/* an angle brought into -PI..PI */
#define WRAP_ANGLE(angle) ((angle) + ANGLE_PI - (f32)(s32)(((angle) + ANGLE_PI) / ANGLE_TURN) * ANGLE_TURN - ANGLE_PI)

/* The colour number of a colour change code in a text. */
static inline s32 text_color(u16 code) {
    return (code & 0xFFF) >> 8;
}

/* Hands a packet in the scratchpad to the GIF once it is idle. */
static inline void send_packet(u32 *packet, s32 size) {
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
    *D2_QWC = size;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN;
}

/* Waits until the GIF is idle. */
static inline void wait_gif(void) {
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
}

/* Splits a colour word (red in bits 24-31, green 16-23, blue 8-15) into three bytes. */
static inline void UnpackColour(u8 *rgb, u32 *colour) {
    rgb[0] = *colour >> 24;
    rgb[1] = *colour >> 16;
    rgb[2] = *colour >> 8;
}

static s32 MenuHeap_GetTop(void);
static void MenuHeap_Clear(void);
static void MenuArena_AllocWorkBuffer(void);
static void MenuDraw_TextAt(u16 *text, GsPackedWord origin);
static void MenuDraw_TextRightAligned(u16 *text, GsPackedWord origin);
static int MenuText_ResetPen(void);
static int MenuText_ResetGlyphSize(void);
static s32 MenuText_ResetColor(void);
static s32 MenuText_DrawWordWrapped(u16 *text, s32 limit);
static void PoolSlot_SetAllParts(PoolPartFile *file);
static void PoolSlot_InitFreeList(void);
static void PoolNode_BlendKeys(void *out, void *start, void *end, void *start_tangent, void *end_tangent, float phase);
static void *MenuFile_LoadFromHost(const char *name, void *buffer);
static s32 MenuFile_LoadHostBlock(const char *name, void *buffer);
static void MenuTexture_UploadInStrips(TextureHeader *header);
static SpritePacket *MenuSprite_Keep(SpritePacket *packet);
static SpritePacket *MenuSprite_Hide(void);
static SpritePacket *MenuSprite_OnlySlot(SpritePacket *packet, s32 slot);
static SpritePacket *MenuSprite_NudgeSlot(SpritePacket *packet, int slot);
static SpritePacket *MenuSprite_OnlySlotNudged(SpritePacket *packet, int slot);
static SpritePacket *MenuSprite_ExceptSlotNudged(SpritePacket *packet, int slot);
static SpritePacket *MenuSprite_SlideLeft(SpritePacket *packet, int amount);
static SpritePacket *MenuSprite_SlideRight(SpritePacket *src_packet, int amount);
static SpritePacket *MenuSprite_SlideUp(SpritePacket *src_packet, int amount);
static SpritePacket *MenuSprite_SlideDown(SpritePacket *packet, int amount);
static SpritePacket *MenuSprite_Fade(SpritePacket *src_packet, int alpha);
static SpritePacket *MenuSprite_SlideRightAlt(SpritePacket *src_packet, int amount);
static void MenuSprite_Stub12(void);
static void MenuSprite_Stub15(void);
static void MenuSprite_Stub14(void);
static SpritePacket *MenuSprite_TintWeaponStat(SpritePacket *packet);
static SpritePacket *MenuSprite_TintWornStat(SpritePacket *packet, int index);
static SpritePacket *MenuSprite_TintMagicStat(SpritePacket *packet);
static void MenuSprite_Stub19(void);
static SpritePacket *MenuSprite_ShowIfBitSet(SpritePacket *packet, s32 slot_mask);
static SpritePacket *MenuSprite_ShowIfOptionChosen(SpritePacket *packet);
static SpritePacket *MenuSprite_ShowIfFlagSet(SpritePacket *packet);
static SpritePacket *MenuSprite_SizeToLevel(SpritePacket *packet);
static SpritePacket *MenuSprite_ColorFromNumber(SpritePacket *packet);
static SpritePacket *MenuSprite_PulseYellow(SpritePacket *packet, int slot);
static SpritePacket *MenuSprite_PulseBlue(SpritePacket *packet, int slot);
static SpritePacket *MenuSprite_SetBlendNormal(SpritePacket *packet);
static SpritePacket *MenuSprite_DimSlot(SpritePacket *packet, int slot);
static SpritePacket *MenuSprite_SetColumn(SpritePacket *packet, int column);
static SpritePacket *MenuSprite_MoveDownRows(SpritePacket *source, int row);
static SpritePacket *MenuSprite_TintWeaponStatAlt(SpritePacket *packet);
static SpritePacket *MenuSprite_TintWornStatAlt(SpritePacket *packet, int index);
static SpritePacket *MenuSprite_TintShortfall(SpritePacket *packet);
static int MenuPad_StepHorizontal(int repeat_start);
static int MenuPad_StepVertical(int repeat_start);
static int MenuPad_StepHorizontalFrom(int repeat_start, u16 buttons);
static int MenuPad_StepVerticalFrom(int repeat_start, u16 buttons);
static SpritePacket *MenuSprite_SetAlpha(SpritePacket *packet, int alpha);

/* the sprite handlers, by the number MenuSprite_Draw is given: each one is handed a sprite and one value and
   gives back the sprite to draw (often an altered copy), or NULL to draw nothing */
static const SpriteHandler sprite_handlers[50] = {
    (SpriteHandler)MenuSprite_Keep, /* 0 */
    (SpriteHandler)MenuSprite_PulseYellow, /* 1 */
    (SpriteHandler)MenuSprite_OnlySlot, /* 2 */
    (SpriteHandler)MenuSprite_OnlySlot, /* 3 */
    (SpriteHandler)MenuSprite_OnlySlotNudged, /* 4 */
    (SpriteHandler)MenuSprite_ExceptSlotNudged, /* 5 */
    (SpriteHandler)MenuSprite_Hide, /* 6 */
    (SpriteHandler)MenuSprite_Fade, /* 7 */
    (SpriteHandler)MenuSprite_SlideRight, /* 8 */
    (SpriteHandler)MenuSprite_SlideLeft, /* 9 */
    (SpriteHandler)MenuSprite_SlideDown, /* 10 */
    (SpriteHandler)MenuSprite_SlideUp, /* 11 */
    (SpriteHandler)MenuSprite_Stub12, /* 12 */
    (SpriteHandler)MenuSprite_SlideRightAlt, /* 13 */
    (SpriteHandler)MenuSprite_Stub14, /* 14 */
    (SpriteHandler)MenuSprite_Stub15, /* 15 */
    (SpriteHandler)MenuSprite_TintWeaponStat, /* 16 */
    (SpriteHandler)MenuSprite_TintWornStat, /* 17 */
    (SpriteHandler)MenuSprite_TintMagicStat, /* 18 */
    (SpriteHandler)MenuSprite_Stub19, /* 19 */
    (SpriteHandler)MenuSprite_ShowIfBitSet, /* 20 */
    (SpriteHandler)MenuSprite_ShowIfOptionChosen, /* 21 */
    (SpriteHandler)MenuSprite_SizeToLevel, /* 22 */
    (SpriteHandler)MenuSprite_ShowIfFlagSet, /* 23 */
    (SpriteHandler)MenuSprite_ColorFromNumber, /* 24 */
    (SpriteHandler)MenuSprite_DimSlot, /* 25 */
    (SpriteHandler)MenuSprite_Fade, /* 26 */
    (SpriteHandler)MenuSprite_Fade, /* 27 */
    (SpriteHandler)MenuSprite_OnlySlot, /* 28 */
    (SpriteHandler)MenuSprite_OnlySlot, /* 29 */
    (SpriteHandler)MenuSprite_SetBlendNormal, /* 30 */
    (SpriteHandler)MenuSprite_PulseBlue, /* 31 */
    (SpriteHandler)MenuSprite_SetColumn, /* 32 */
    (SpriteHandler)MenuSprite_MoveDownRows, /* 33 */
    (SpriteHandler)MenuSprite_TintWeaponStatAlt, /* 34 */
    (SpriteHandler)MenuSprite_TintWornStatAlt, /* 35 */
    (SpriteHandler)MenuSprite_TintShortfall, /* 36 */
};

/* Empties the menus' heap: its mark and its top both go back to the base. */
void MenuHeap_Reset(void) {
    heap_mark = heap_top = menu_heap;
}

/* Sets the heap's mark at its top: what is allocated so far survives MenuHeap_FreeToMark. */
void MenuHeap_Mark(void) {
    heap_mark = heap_top;
}

/* Bump-allocates size bytes from the heap top heap_top, leaving the top 16-byte aligned. */
void *MenuHeap_Alloc(int size) {
    void *block = (void *)heap_top;
    u8 *top;
    s32 misalign;

    heap_top += size;
    // match: a pointer top keeps gcc from folding "- (misalign - 16)" into "+ 16 - misalign"
    top = (u8 *)heap_top;
    misalign = (s32)top & 0xF;
    if (misalign) {
        top -= misalign - 16;
    }
    heap_top = (s32)top;
    return block;
}

/* Returns the heap's top. */
static s32 MenuHeap_GetTop(void) {
    return heap_top;
}

/* Frees everything allocated since the mark was set. */
void MenuHeap_FreeToMark(void) {
    heap_top = heap_mark;
}

/* Empties the heap like MenuHeap_Reset (the same two stores, the other way round). */
static void MenuHeap_Clear(void) {
    heap_top = heap_mark = menu_heap;
}

/* the second allocator */
static s32 arena_next = 0;                  /* 0x446D38 where its next block comes from */
static s32 arena_start = 0;                 /* 0x446D3C */
static f32 (*part_work_buffer)[4] = NULL;   /* 0x446D40 the part drawers' work buffer: 0x500 vectors each for points,
                                               screen points and normals */

/* Starts the second allocator at `value` and takes the part drawers' work buffer from it. */
s32 MenuArena_Init(s32 value) { // match: declared with a result that no path sets (Menu_InitLoadBuffers only matches so)
    arena_start = value;
    arena_next = value;
    MenuArena_AllocWorkBuffer();
}

/* Takes `size` bytes at a multiple of `align` from the second allocator; returns the address, or 0 when
   that would end more than 0x280000 bytes from its start. */
void *MenuArena_Alloc(s32 size, s32 align) {
    s32 aligned;
    s32 end;

    aligned = ((arena_next + align - 1) / align) * align;
    end = aligned + size;
    arena_next = aligned;
    if (end - arena_start > 0x280000) {
        return NULL;
    }
    arena_next = end;
    return (void *)aligned;
}

/* Allocates the 0xF000-byte buffer the part drawers transform points and normals into. */
static void MenuArena_AllocWorkBuffer(void) {
    part_work_buffer = MenuArena_Alloc(0xF000, 0x10);
}

/* Flips between the two packet buffers and returns the one now current. */
void *MenuDraw_NextPacket(void) {
    void *buffers[2] = { PACKET_BUFFER_0, PACKET_BUFFER_1 };
    static s32 current = 0; /* 0x446D50 which of the two was handed out last */

    current = current == 0;
    return buffers[current];
}

/* Sends a seven-quadword packet that draws a textured rectangle as a four-corner strip. */
void MenuDraw_Sprite(KindObject *object) {
    TexturedRectDef *rect = (TexturedRectDef *)object;
    u32 *packet;

    if (rect != NULL) {
        packet = (u32 *)MenuDraw_NextPacket();
        packet[0] = 0x8001;
        packet[1] = 0xC4000000;
        packet[2] = 0x25252106;
        packet[3] = 0xFFFFF525;
        *(s64 *)&packet[4] = rect->tex0;
        packet[6] = 0x54;
        packet[7] = 0;
        packet[8] = rect->red | (rect->green << 8) | (rect->blue << 16) | (rect->alpha << 24);
        *(f32 *)&packet[9] = rect->scale;
        *(f32 *)&packet[10] = rect->first_s;
        *(f32 *)&packet[11] = rect->second_t;
        packet[12] = rect->first.pos_x | ((s32)rect->second.pos_y << 16);
        packet[13] = rect->first.depth;
        *(f32 *)&packet[14] = rect->first_s;
        *(f32 *)&packet[15] = rect->first_t;
        packet[16] = rect->first.pos_x | ((s32)rect->first.pos_y << 16);
        packet[17] = rect->first.depth;
        *(f32 *)&packet[18] = rect->second_s;
        *(f32 *)&packet[19] = rect->second_t;
        packet[20] = rect->second.pos_x | ((s32)rect->second.pos_y << 16);
        packet[21] = rect->first.depth;
        *(f32 *)&packet[22] = rect->second_s;
        *(f32 *)&packet[23] = rect->first_t;
        packet[24] = rect->second.pos_x | ((s32)rect->first.pos_y << 16);
        packet[25] = rect->first.depth;
        while (*D2_CHCR & D_CHCR_STR);
        *D2_QWC = 7;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
        *D2_CHCR = D_CHCR_STR;
    }
}

/* Returns where word `index` starts in the glyph codes MenuDraw_Word draws from (its low byte: callers only
   subtract two starts to measure a word). */
s8 MenuDraw_GetWordStart(s32 index) {
    return ((s8 *)menu_word_starts)[index * 2]; // match: only the low byte of the 16-bit start is read
}

/* Draws a sprite of kind 1, a word: the glyph codes menu_word_glyphs holds from menu_word_starts[unk32] up to the next
   entry (less one), one textured quad per glyph, spread evenly between the sprite's two corners, in the
   sprite's colour; then sends the packet to the GS over DMA channel 2. */
void MenuDraw_Word(KindObject *object) {
    SpritePacket *sprite = (SpritePacket *)object;
    f32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 cell[2] = { 0.0390625f, 0.0390625f }; /* how much of a font page one glyph cell covers, across and down */
    GsPackedWord first;
    GsPackedWord second;
    GsPackedWord step;
    u32 *packet;
    u32 *cursor;
    s32 start;
    s32 count;
    s32 index;
    s32 code;
    s32 column;
    s32 row;

    if (sprite == NULL) {
        return;
    }
    packet = MenuDraw_NextPacket();
    count = menu_word_starts[sprite->word_index + 1] - menu_word_starts[sprite->word_index] - 1; // match: before `start`; 0 traps in the divisions below
    start = menu_word_starts[sprite->word_index];
    first.f.lo16 = sprite->xyz0.X;
    second.f.lo16 = sprite->xyz1.X;
    first.f.mid16 = sprite->xyz0.Y;
    second.f.mid16 = sprite->xyz1.Y;
    first.f.mid24 = sprite->xyz0.Z;
    second.f.mid24 = sprite->xyz0.Z; // match: the first corner's depth, not the second's
    step.f.lo16 = ((s32)second.f.lo16 - (s32)first.f.lo16) / count;
    step.f.mid16 = ((s32)second.f.mid16 - (s32)first.f.mid16) / count; // match: worked out, never used
    cursor = packet;
    *cursor++ = count | GIF_TAG_EOP;
    *cursor++ = 0xC4000000;
    *cursor++ = 0x25252106;
    *cursor++ = 0xFFFFF525;
    for (index = 0; index < count && (code = menu_word_glyphs[start + index]) != 0xFFFF; index++) {
        *((u64 *)cursor)++ = *menu_font_page_tex0[code / 625];
        code %= 625;
        column = code % 25;
        row = code / 25;
        *cursor++ = 0x54;
        *cursor++ = 0;
        *cursor++ = sprite->red | (sprite->green << 8) | (sprite->blue << 16) | (sprite->alpha << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = (f32)column * cell[0] + 0.009765625f;
        *((f32 *)cursor)++ = (f32)(row + 1) * cell[1];
        *cursor++ = ((s32)first.f.lo16 + index * (s32)step.f.lo16) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)column * cell[0] + 0.009765625f;
        *((f32 *)cursor)++ = (f32)row * cell[1];
        *cursor++ = ((s32)first.f.lo16 + index * (s32)step.f.lo16) | ((s32)first.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell[0] - 0.009765625f;
        *((f32 *)cursor)++ = (f32)(row + 1) * cell[1];
        *cursor++ = ((index + 1) * (s32)step.f.lo16 + (s32)first.f.lo16) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell[0] - 0.009765625f;
        *((f32 *)cursor)++ = (f32)row * cell[1];
        *cursor++ = ((index + 1) * (s32)step.f.lo16 + (s32)first.f.lo16) | ((s32)first.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = count * 6 + 1;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
}

/* Sends a four-quadword packet that draws a flat rectangle as a four-corner strip. */
void MenuDraw_Bar(MenuCursorBar *bar) {
    FlatRectDef *rect = (FlatRectDef *)bar;
    GsCornerBits first;
    GsCornerBits second;
    u32 *packet;

    if (rect != NULL) {
        packet = (u32 *)MenuDraw_NextPacket();
        first.pos_x = rect->first.pos_x;
        first.pos_y = rect->first.pos_y;
        first.depth = rect->first.depth;
        second.pos_x = rect->second.pos_x;
        second.pos_y = rect->second.pos_y;
        second.depth = rect->first.depth; // match: the first corner's depth, and neither copy is read again
        packet[0] = 0x8001;
        packet[1] = 0x64000000;
        packet[2] = 0xFF555510;
        packet[3] = 0;
        packet[4] = 0x44;
        packet[5] = 0;
        packet[6] = rect->red | (rect->green << 8) | (rect->blue << 16) | (rect->alpha << 24);
        *(f32 *)&packet[7] = rect->scale;
        packet[8] = (s32)first.pos_x | ((s32)second.pos_y << 16);
        packet[9] = rect->first.depth;
        packet[10] = (s32)first.pos_x | ((s32)first.pos_y << 16);
        packet[11] = rect->first.depth;
        packet[12] = (s32)second.pos_x | ((s32)second.pos_y << 16);
        packet[13] = rect->first.depth;
        packet[14] = (s32)second.pos_x | ((s32)first.pos_y << 16);
        packet[15] = rect->first.depth;
        while (*D2_CHCR & D_CHCR_STR);
        *D2_QWC = 4;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
        *D2_CHCR = D_CHCR_STR;
    }
}

/* Gives the current value of the number a menu sprite stands for (SpritePacket.number_id). */
int MenuDraw_GetNumber(int number) {
    switch (number) {
    case 0:
    case 64:
    case 65:
        return 0;
    case 1:
        return player_state.hp_max;
    case 2:
        return player_state.mp_max;
    case 3:
        return player_state.power_base;
    case 4:
        return player_state.magic_base;
    case 5:
        return player_state.attack_stats.values[0];
    case 6:
        return player_state.attack_stats.values[1];
    case 7:
        return player_state.attack_stats.values[2];
    case 8:
        return player_state.attack_stats.values[3];
    case 9:
        return player_state.attack_stats.values[4];
    case 10:
        return player_state.attack_stats.values[5];
    case 11:
        return player_state.attack_stats.values[6];
    case 12:
        return player_state.attack_stats.values[7];
    case 13:
        return player_state.attack_stats.values[8];
    case 14:
    case 15:
    case 16:
        return player_state.defence_stats.values[0];
    case 17:
        return player_state.defence_stats.values[1];
    case 18:
        return player_state.defence_stats.values[2];
    case 19:
        return player_state.defence_stats.values[3];
    case 20:
        return player_state.defence_stats.values[4];
    case 21:
        return player_state.defence_stats.values[5];
    case 22:
        return player_state.defence_stats.values[6];
    case 23:
        return player_state.defence_stats.values[7];
    case 24:
        return player_state.defence_stats.values[8];
    case 25:
    case 26:
    case 27:
        return player_state.gold;
    case 28:
        return player_state.experience;
    case 29:
        return player_state.level;
    case 30:
        return player_state.hp_now;
    case 31:
        return player_state.mp_now;
    case 32:
        return player_state.power;
    case 33:
        return player_state.magic;
    case 34:
        return current_play_time.mday;
    case 35:
        return current_play_time.hour;
    case 36:
        return current_play_time.min;
    case 37:
        return current_play_time.sec;
    case 38:
        return wall_clock_time.year + 2000;
    case 39:
        return wall_clock_time.mon;
    case 40:
        return wall_clock_time.mday;
    case 41:
        return wall_clock_time.hour;
    case 42:
        return wall_clock_time.min;
    case 43:
        return wall_clock_time.sec;
    case 44:
        return menu_number_44;
    case 45:
        return menu_number_45;
    case 46:
        return menu_number_46;
    case 47:
        return menu_number_47;
    case 48:
        return menu_number_48;
    case 49:
        return menu_number_49;
    case 50:
        return menu_number_50;
    case 51:
        return menu_number_51;
    case 52:
        return menu_number_52;
    case 53:
        return menu_number_53;
    case 54:
        return menu_number_54;
    case 55:
        return player_state.worn_item_stats.values[0];
    case 56:
        return player_state.worn_item_stats.values[1];
    case 57:
        return player_state.worn_item_stats.values[2];
    case 58:
        return player_state.worn_item_stats.values[3];
    case 59:
        return player_state.worn_item_stats.values[4];
    case 60:
        return player_state.worn_item_stats.values[5];
    case 61:
        return player_state.worn_item_stats.values[6];
    case 62:
        return player_state.worn_item_stats.values[7];
    case 63:
        return player_state.worn_item_stats.values[8];
    case 66:
        return player_state.weight;
    case 67:
        return player_state.max_weight;
    case 70:
        return menu_number_70;
    case 71:
        return menu_number_71;
    case 72:
        return menu_number_72;
    case 73:
        return menu_number_73;
    case 74:
        return menu_number_74;
    case 75:
        return menu_number_75;
    case 76:
        return menu_number_76;
    case 77:
        return menu_number_77;
    case 78:
        return menu_number_78;
    case 79:
        return menu_number_79;
    case 80:
        return menu_number_80;
    case 81:
        return menu_number_81;
    case 82:
        return menu_number_82;
    case 83:
        return menu_number_83;
    case 84:
        return menu_number_84;
    case 85:
        return menu_number_85;
    case 86:
        return menu_number_86;
    case 87:
        return menu_number_87;
    case 88:
        return menu_number_88;
    case 89:
        return menu_number_89;
    case 90:
        return menu_number_90;
    case 91:
        return menu_number_91;
    case 92:
        return menu_number_92;
    case 93:
        return menu_number_93;
    case 94:
        return menu_number_94;
    default:
        return 0;
    }
}

/* Draws a sprite of kind 2 or 3, a number: unk34 (kind 2: what MenuDraw_GetNumber makes of it) in decimal,
   right-aligned in unk33 glyphs and filled with blanks (glyph 0x49), or, when unk33 is 0, as three digits,
   glyph 0xD and one more digit; one textured quad per glyph between the sprite's two corners, each 6 pixels
   narrower than its share. Nothing is drawn for a negative number. */
int MenuDraw_Number(KindObject *object) { // match: returns int, though no path sets a value
    SpritePacket *sprite = (SpritePacket *)object;
    f32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 cell[2] = { 0.0390625f, 0.0390625f }; /* how much of a font page one glyph cell covers, across and down */
    u16 glyphs[16];
    GsPackedWord first;
    GsPackedWord second;
    GsPackedWord step;
    u32 *packet;
    u32 *cursor;
    s32 value;
    s32 count;
    s32 index;
    s32 code;
    s32 column;
    s32 row;
    s32 base;
    s32 left;
    s32 right;

    if (sprite == NULL) {
        return;
    }
    if (sprite->kind == 2) {
        value = MenuDraw_GetNumber(sprite->number_id);
    } else {
        value = sprite->number_id;
    }
    if (value < 0) {
        return;
    }
    if (value <= 999999999) {
        if (sprite->digit_count != 0) {
            index = sprite->digit_count;
            do {
                glyphs[index - 1] = value % 10;
                value /= 10;
                index--;
                if (value == 0) {
                    break;
                }
            } while (index > 0);
            while (index > 0) {
                glyphs[index - 1] = 0x49;
                index--;
            }
        } else {
            glyphs[4] = value % 10;
            value /= 10;
            glyphs[3] = 0xD;
            index = 2;
            do {
                glyphs[index] = value % 10;
                index--;
                value /= 10;
                if (value == 0) {
                    break;
                }
            } while (index >= 0);
            while (index >= 0) {
                glyphs[index] = 0x49;
                index--;
            }
        }
    } else {
        for (index = sprite->digit_count; index > 0; index--) {
            glyphs[index] = 0; // match: one too far up; glyphs[0] stays unset
        }
    }
    packet = MenuDraw_NextPacket();
    count = 5;
    if (sprite->digit_count != 0) {
        count = sprite->digit_count;
    }
    first.f.lo16 = sprite->xyz0.X;
    second.f.lo16 = sprite->xyz1.X;
    first.f.mid16 = sprite->xyz0.Y;
    second.f.mid16 = sprite->xyz1.Y;
    first.f.mid24 = sprite->xyz0.Z;
    second.f.mid24 = sprite->xyz0.Z; // match: the first corner's depth, not the second's
    step.f.lo16 = ((s32)second.f.lo16 - (s32)first.f.lo16) / count;
    step.f.mid16 = ((s32)second.f.mid16 - (s32)first.f.mid16) / count; // match: worked out, never used
    cursor = packet;
    *cursor++ = count | GIF_TAG_EOP;
    *cursor++ = 0xC4000000;
    *cursor++ = 0x25252106;
    *cursor++ = 0xFFFFF525;
    for (index = 0; index < count; index++) {
        code = glyphs[index];
        column = code % 25;
        row = code / 25;
        *((u64 *)cursor)++ = *menu_font_page_tex0[0];
        base = (s32)first.f.lo16 + (count << 5);
        left = index * (s32)step.f.lo16 - ((index * 6) << 4) + (count << 6);
        right = (index + 1) * (s32)step.f.lo16 - ((index * 6) << 4) + (count << 6);
        *cursor++ = 0x54;
        *cursor++ = 0;
        *cursor++ = sprite->red | (sprite->green << 8) | (sprite->blue << 16) | (sprite->alpha << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = (f32)column * cell[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * cell[1];
        *cursor++ = (base + left) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)column * cell[0];
        *((f32 *)cursor)++ = (f32)row * cell[1];
        *cursor++ = (base + left) | ((s32)first.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * cell[1];
        *cursor++ = (base + right) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell[0];
        *((f32 *)cursor)++ = (f32)row * cell[1];
        *cursor++ = (base + right) | ((s32)first.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = count * 6 + 1;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
}

/* Draws a line of text at a screen position (origin: x, y, z and the line height in 16ths): one textured
   quad per glyph code, built in a packet buffer and sent to the GS over DMA channel 2. The text ends at
   0xFFFF or at any code with bit 0x8000. Codes with bit 0x800 are half-width glyphs of the first font page
   (50 columns of 5/256, from cell 0x47E on); the others are full-width ones, 625 to a page, 25 to a row. */
static void MenuDraw_TextAt(u16 *text, GsPackedWord origin) {
    f32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    GsPackedWord top;
    GsPackedWord bottom;
    s32 glyph_count;
    u32 *packet;
    u32 *cursor;
    s32 index;
    s32 count;
    s32 column;
    s32 row;
    s32 code;
    u16 glyph;

    packet = MenuDraw_NextPacket();
    top.f.lo16 = bottom.f.lo16 = origin.f.lo16;
    top.f.mid16 = origin.f.mid16;
    top.f.mid24 = origin.f.mid24;
    bottom.f.mid16 = origin.f.mid16 + (u8)origin.f.hi8 * 16;
    bottom.f.mid24 = origin.f.mid24;
    cursor = packet;
    *cursor++ = glyph_count | GIF_TAG_EOP; // match: never set; the real count is stored after the loop
    *cursor++ = 0xC4000000;
    *cursor++ = 0x25252106;
    *cursor++ = 0xFFFFF525;
    index = 0;
    count = 0;
    while (text[index] != 0xFFFF && !(text[index] & 0x8000)) {
        if (text[index] & GLYPH_HALF_WIDTH) {
            glyph = text[index]; // match: read again into a 16-bit local
            code = glyph & GLYPH_CODE_MASK; // match: two statements
            code += 0x47E;
            column = code % 50;
            row = code / 50;
            *((u64 *)cursor)++ = *menu_font_page_tex0[0];
            top.f.lo16 = bottom.f.lo16;
            bottom.f.lo16 += 0xA0;
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
        } else {
            code = text[index];
            *((u64 *)cursor)++ = *menu_font_page_tex0[code / 625];
            code %= 625;
            top.f.lo16 = bottom.f.lo16;
            bottom.f.lo16 += 0x140;
            column = code % 25;
            row = code / 25;
            scale[1] = scale[0] = 0.0390625f;
        }
        *cursor++ = 0x54;
        *cursor++ = 0;
        *cursor++ = 0x80808080;
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = (f32)column * scale[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * scale[1];
        *cursor++ = (s32)top.f.lo16 | ((s32)bottom.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *((f32 *)cursor)++ = (f32)column * scale[0];
        *((f32 *)cursor)++ = (f32)row * scale[1];
        *cursor++ = (s32)top.f.lo16 | ((s32)top.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * scale[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * scale[1];
        *cursor++ = (s32)bottom.f.lo16 | ((s32)bottom.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * scale[0];
        *((f32 *)cursor)++ = (f32)row * scale[1];
        *cursor++ = (s32)bottom.f.lo16 | ((s32)top.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *cursor++ = 0;
        *cursor++ = 0;
        index++;
        count++;
    }
    *packet = count | GIF_TAG_EOP;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = count * 6 + 1;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
}

/* Draws a line of text like MenuDraw_TextAt, but first moves the origin right by 160 for every glyph the text
   is short of 13, so that it ends where a 13-glyph line would (counted in full-width glyphs). */
static void MenuDraw_TextRightAligned(u16 *text, GsPackedWord origin) {
    f32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    GsPackedWord top;
    GsPackedWord bottom;
    s32 glyph_count;
    u32 *packet;
    u32 *cursor;
    s32 index;
    s32 length;
    s32 count;
    s32 column;
    s32 row;
    s32 code;
    u16 glyph;

    packet = MenuDraw_NextPacket();
    cursor = packet;
    for (length = 0; text[length] != 0xFFFF; length++) {
    }
    length = 13 - length; // match: the variable is reused for the number of missing glyphs
    origin.f.lo16 += (length * 20) << 3;
    top.f.lo16 = bottom.f.lo16 = origin.f.lo16;
    top.f.mid16 = origin.f.mid16;
    top.f.mid24 = origin.f.mid24;
    bottom.f.mid16 = origin.f.mid16 + (u8)origin.f.hi8 * 16;
    bottom.f.mid24 = origin.f.mid24;
    *cursor++ = glyph_count | GIF_TAG_EOP; // match: never set; the real count is stored after the loop
    *cursor++ = 0xC4000000;
    *cursor++ = 0x25252106;
    *cursor++ = 0xFFFFF525;
    index = 0;
    count = 0;
    while (text[index] != 0xFFFF && !(text[index] & 0x8000)) {
        if (text[index] & GLYPH_HALF_WIDTH) {
            glyph = text[index]; // match: read again into a 16-bit local
            code = glyph & GLYPH_CODE_MASK; // match: two statements
            code += 0x47E;
            column = code % 50;
            row = code / 50;
            *((u64 *)cursor)++ = *menu_font_page_tex0[0];
            top.f.lo16 = bottom.f.lo16;
            bottom.f.lo16 += 0xA0;
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
        } else {
            code = text[index];
            *((u64 *)cursor)++ = *menu_font_page_tex0[code / 625];
            code %= 625;
            top.f.lo16 = bottom.f.lo16;
            bottom.f.lo16 += 0x140;
            column = code % 25;
            row = code / 25;
            scale[1] = scale[0] = 0.0390625f;
        }
        *cursor++ = 0x54;
        *cursor++ = 0;
        *cursor++ = 0x80808080;
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = (f32)column * scale[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * scale[1];
        *cursor++ = (s32)top.f.lo16 | ((s32)bottom.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *((f32 *)cursor)++ = (f32)column * scale[0];
        *((f32 *)cursor)++ = (f32)row * scale[1];
        *cursor++ = (s32)top.f.lo16 | ((s32)top.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * scale[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * scale[1];
        *cursor++ = (s32)bottom.f.lo16 | ((s32)bottom.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * scale[0];
        *((f32 *)cursor)++ = (f32)row * scale[1];
        *cursor++ = (s32)bottom.f.lo16 | ((s32)top.f.mid16 << 16);
        *cursor++ = top.f.mid24;
        *cursor++ = 0;
        *cursor++ = 0;
        index++;
        count++;
    }
    *packet = count | GIF_TAG_EOP;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = count * 6 + 1;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
}

/* the text pen */
static GsCornerBits text_pen = { 0x6C00, 0x7900, 0xFFFFFF, 0 };  /* 0x446D60 where the next glyph goes */
static GsCornerBits text_glyph_size = { 0x140, 0xB0, 0, 0 };     /* 0x446D68 the size of a full-width glyph's cell */
static ColorBytes text_ink = { 0x80, 0x80, 0x80, 0x80 };          /* 0x446D70 the colour */

/* Sets the text pen: x and y in 12.4 fixed point, the depth and a top byte. Returns 1. */
int MenuText_SetPen(GsPackedWord word) {
    text_pen.pos_x = word.f.lo16;
    text_pen.pos_y = word.f.mid16;
    text_pen.depth = word.f.mid24;
    text_pen.top_byte = word.f.hi8;
    return 1;
}

/* Puts the pen back at the top left of the screen, at the greatest depth. Returns 1. */
static int MenuText_ResetPen(void) {
    text_pen.pos_x = 0x6C00;
    text_pen.pos_y = 0x7900;
    text_pen.depth = 0xFFFFFF;
    text_pen.top_byte = 0;
    return 1;
}

/* Sets the size of the cell a full-width glyph is drawn in (the word's x and y fields). Returns 1. */
int MenuText_SetGlyphSize(GsPackedWord word) {
    text_glyph_size.pos_x = word.f.lo16;
    text_glyph_size.pos_y = word.f.mid16;
    text_glyph_size.depth = word.f.mid24;
    text_glyph_size.top_byte = word.f.hi8;
    return 1;
}

/* Puts the glyph size back at its default, 20 by 11 pixels. Returns 1. */
static int MenuText_ResetGlyphSize(void) {
    text_glyph_size.pos_x = 0x140;
    text_glyph_size.pos_y = 0xB0;
    text_glyph_size.depth = 0;
    text_glyph_size.top_byte = 0;
    return 1;
}

/* Sets the colour text is drawn in. Returns 1. */
s32 MenuText_SetColor(ColorBytes bytes) {
    text_ink.red = bytes.red;
    text_ink.green = bytes.green;
    text_ink.blue = bytes.blue;
    text_ink.alpha = bytes.alpha;
    return 1;
}

/* Puts the text colour back at its default, 0x80 for red, green, blue and alpha. Returns 1. */
static s32 MenuText_ResetColor(void) {
    text_ink.red = 0x80;
    text_ink.green = 0x80;
    text_ink.blue = 0x80;
    text_ink.alpha = 0x80;
    return 1;
}

/* Draws a string of glyphs at the pen position text_pen into the next shared packet buffer, wrapping at
   `limit` glyphs a line and at the line breaks in the text, and sends it to the GS; returns the number of
   glyphs drawn. */
s32 MenuText_DrawWrapped(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count;
    s32 i;
    s32 column = 0; // match: cleared again in the loop's init, which makes it a copy of count's register
    s32 next_column;
    s32 glyph;
    s32 glyph_row;
    s32 glyph_column;
    s32 color_index;

    packet = MenuDraw_NextPacket();
    color[0] = text_ink.red;
    color[1] = text_ink.green;
    color[2] = text_ink.blue;
    color[3] = text_ink.alpha;
    top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
    top_left.pos_y = text_pen.pos_y;
    bottom_right.pos_y = text_pen.pos_y + text_glyph_size.pos_y;
    top_left.depth = bottom_right.depth = text_pen.depth;
    limit++;
    cursor = packet + 4;
    packet[0] = 0x8000;
    packet[1] = 0x84000000;
    packet[2] = 0xF5252106;
    packet[3] = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & GLYPH_COLOR) {
            /* a colour change, not a glyph */
            count--;
            color_index = text[i];
            palette = Message_GetColor((color_index & 0xFFF) >> 8);
            color[0] = palette[0];
            color[1] = palette[4];
            color[2] = palette[8];
            color[3] = palette[12];
            continue;
        }
        next_column = column + 1;
        if ((next_column == limit
             && !((text[i] >= 10 && text[i] <= 16) || (text[i] >= 0xE3 && text[i] <= 0xF9) || text[i] == 0x80E))
            || next_column > limit) {
            /* line feed; the glyph is drawn on the next pass */
            top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
            top_left.pos_y += text_glyph_size.pos_y;
            bottom_right.pos_y += text_glyph_size.pos_y;
            count--;
            i--;
            column = 0;
            continue;
        }
        if ((text[i] & 0x8000) && column != 0) {
            /* a line break in the text */
            top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
            top_left.pos_y += text_glyph_size.pos_y;
            bottom_right.pos_y += text_glyph_size.pos_y;
            count--;
            column = 0;
            continue;
        }
        column = next_column;
        if (text[i] & GLYPH_HALF_WIDTH) {
            /* a half-width glyph */
            glyph = text[i]; // match: three statements on the one variable, not one expression
            glyph &= GLYPH_CODE_MASK;
            glyph += 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
        } else {
            glyph = text[i];
            *((u64 *)cursor)++ = *menu_font_page_tex0[(glyph / 625) % 2]; // match: % 2, not & 1
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += text_glyph_size.pos_x;
            scale[0] = 0.0390625f;
            scale[1] = 0.0390625f;
        }
        *cursor++ = 0x56;
        *cursor++ = 0;
        *cursor++ = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = glyph_column * scale[0];
        *((f32 *)cursor)++ = glyph_row * scale[1];
        *cursor++ = (s32)top_left.pos_x | ((s32)top_left.pos_y << 16);
        *cursor++ = top_left.depth;
        *((f32 *)cursor)++ = (glyph_column + 1) * scale[0];
        *((f32 *)cursor)++ = (glyph_row + 1) * scale[1];
        *cursor++ = (s32)bottom_right.pos_x | ((s32)bottom_right.pos_y << 16);
        *cursor++ = top_left.depth;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    packet[0] = count | GIF_TAG_EOP;
    text_pen.pos_y = bottom_right.pos_y;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    return count;
}

static s32 wrapped_long_buffer = 0;         /* 0x446D78 which of the two large buffers MenuText_DrawWrappedLong uses next */

/* Draws a string of glyphs at the pen position text_pen, wrapping at `limit` glyphs a line and at the line
   breaks in the text, and sends the packet to the GS; returns the number of glyphs drawn. MenuText_DrawWrapped
   for a long text: the packet is built in one of the two large buffers long_text_packets, not in the scratchpad. */
s32 MenuText_DrawWrappedLong(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count;
    s32 i;
    s32 column = 0; // match: cleared again in the loop's init, which makes it a copy of count's register
    s32 next_column;
    s32 line_end;
    s32 glyph;
    s32 glyph_row;
    s32 glyph_column;
    s32 color_index;

    packet = long_text_packets[wrapped_long_buffer];
    wrapped_long_buffer = wrapped_long_buffer == 0;
    color[0] = text_ink.red;
    color[1] = text_ink.green;
    color[2] = text_ink.blue;
    color[3] = text_ink.alpha;
    top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
    top_left.pos_y = text_pen.pos_y;
    bottom_right.pos_y = text_pen.pos_y + text_glyph_size.pos_y;
    top_left.depth = bottom_right.depth = text_pen.depth;
    line_end = limit + 1;
    cursor = packet + 4;
    packet[0] = 0x8000;
    packet[1] = 0x84000000;
    packet[2] = 0xF5252106;
    packet[3] = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & GLYPH_COLOR) {
            /* a colour change, not a glyph */
            count--;
            color_index = text[i];
            palette = Message_GetColor((color_index & 0xFFF) >> 8);
            color[0] = palette[0];
            color[1] = palette[4];
            color[2] = palette[8];
            color[3] = palette[12];
            continue;
        }
        next_column = column + 1;
        if ((next_column == line_end
             && !((text[i] >= 10 && text[i] <= 16) || (text[i] >= 0xE3 && text[i] <= 0xF9) || text[i] == 0x80E))
            || next_column > line_end
            || (column == 0 && text[i] == 0x8000)) {
            if (column != 0 || text[i] != 0x8000) {
                /* line feed; the glyph is drawn on the next pass */
                top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
                top_left.pos_y += text_glyph_size.pos_y;
                bottom_right.pos_y += text_glyph_size.pos_y;
                i--;
            }
            count--;
            column = 0;
            continue;
        }
        if ((text[i] & 0x8000) && column != 0) {
            /* a line break in the text */
            top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
            top_left.pos_y += text_glyph_size.pos_y;
            bottom_right.pos_y += text_glyph_size.pos_y;
            count--;
            column = 0;
            continue;
        }
        column = next_column;
        if (text[i] & GLYPH_HALF_WIDTH) {
            /* a half-width glyph */
            glyph = text[i]; // match: three statements on the one variable, not one expression
            glyph &= GLYPH_CODE_MASK;
            glyph += 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
        } else {
            glyph = text[i];
            *((u64 *)cursor)++ = *menu_font_page_tex0[(glyph / 625) % 2]; // match: % 2, not & 1
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += text_glyph_size.pos_x;
            scale[0] = 0.0390625f;
            scale[1] = 0.0390625f;
        }
        *cursor++ = 0x56;
        *cursor++ = 0;
        *cursor++ = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = glyph_column * scale[0];
        *((f32 *)cursor)++ = glyph_row * scale[1];
        *cursor++ = (s32)top_left.pos_x | ((s32)top_left.pos_y << 16);
        *cursor++ = top_left.depth;
        *((f32 *)cursor)++ = (glyph_column + 1) * scale[0];
        *((f32 *)cursor)++ = (glyph_row + 1) * scale[1];
        *cursor++ = (s32)bottom_right.pos_x | ((s32)bottom_right.pos_y << 16);
        *cursor++ = top_left.depth;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    packet[0] = count | GIF_TAG_EOP;
    text_pen.pos_y = bottom_right.pos_y;
    FlushCache(0);
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = (u32)packet & 0x7FFFFFFF;
    *D2_CHCR = D_CHCR_STR;
    return count;
}

/* Draws one line of text at the pen position text_pen with glyphs of the size text_glyph_size: one sprite per
   glyph code, built in a packet buffer and sent to the GS over DMA channel 2. The text ends at 0xFFFF, at a
   code with bit 0x8000, or when `limit` glyphs are drawn. A code with bit 0x4000 only changes the colour.
   Codes with bit 0x800 are half-width glyphs (50 to a row, from cell 0x47E of the first font page on), the
   others full-width ones, 625 to a page and 25 to a row. Returns the number of glyphs drawn. */
s32 MenuText_DrawLine(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count;
    s32 i;
    s32 column = 0; // match: cleared here and again in the loop's initialiser
    s32 next_column;
    s32 glyph;
    s32 glyph_row;
    s32 glyph_column;

    packet = MenuDraw_NextPacket();
    color[0] = text_ink.red;
    color[1] = text_ink.green;
    color[2] = text_ink.blue;
    color[3] = text_ink.alpha;
    top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
    top_left.pos_y = text_pen.pos_y;
    bottom_right.pos_y = text_pen.pos_y + text_glyph_size.pos_y;
    top_left.depth = bottom_right.depth = text_pen.depth;
    limit++;
    cursor = packet + 4;
    packet[0] = 0x8000;
    packet[1] = 0x84000000;
    packet[2] = 0xF5252106;
    packet[3] = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & GLYPH_COLOR) {
            count--;
            palette = Message_GetColor(text_color(text[i]));
            color[0] = palette[0];
            color[1] = palette[4];
            color[2] = palette[8];
            color[3] = palette[12];
            continue;
        }
        next_column = column + 1;
        if (next_column % limit == 0) {
            break;
        }
        if (text[i] & 0x8000) {
            break;
        }
        column = next_column;
        if (text[i] & GLYPH_HALF_WIDTH) {
            glyph = text[i]; // match: read again as an int, then two statements
            glyph = (glyph & GLYPH_CODE_MASK) + 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
        } else {
            glyph = text[i];
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[glyph / 625];
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            bottom_right.pos_x += text_glyph_size.pos_x;
            scale[1] = scale[0] = 0.0390625f;
        }
        *cursor++ = 0x56;
        *cursor++ = 0;
        *cursor++ = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = glyph_column * scale[0];
        *((f32 *)cursor)++ = glyph_row * scale[1];
        *cursor++ = (s32)top_left.pos_x | ((s32)top_left.pos_y << 16);
        *cursor++ = top_left.depth;
        *((f32 *)cursor)++ = (glyph_column + 1) * scale[0];
        *((f32 *)cursor)++ = (glyph_row + 1) * scale[1];
        *cursor++ = (s32)bottom_right.pos_x | ((s32)bottom_right.pos_y << 16);
        *cursor++ = top_left.depth;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    packet[0] = count | GIF_TAG_EOP;
    text_pen.pos_y = bottom_right.pos_y;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    return count;
}

/* Like MenuText_DrawLine (one line of text at the pen position text_pen, at most `limit` glyphs), but shortened to
   three words: `word_breaks` counts the glyphs that follow a space (0x800) or a code with bit 0x8000, and from
   the third on every half-width glyph is drawn as glyph 0xE, three of them at most. A code with bit 0x8000 as
   the last one of the text ends it; elsewhere it is drawn as a blank. Returns the number of glyphs drawn. */
s32 MenuText_DrawLineShortened(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count;
    s32 i;
    s32 column = 0; // match: cleared here and again in the loop's initialiser
    s32 word_breaks = 0;
    s32 dots = 0;
    s32 next_column;
    s32 glyph;
    s32 code;
    s32 glyph_row;
    s32 glyph_column;

    packet = MenuDraw_NextPacket();
    color[0] = text_ink.red;
    color[1] = text_ink.green;
    color[2] = text_ink.blue;
    color[3] = text_ink.alpha;
    top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
    top_left.pos_y = text_pen.pos_y;
    bottom_right.pos_y = text_pen.pos_y + text_glyph_size.pos_y;
    top_left.depth = bottom_right.depth = text_pen.depth;
    limit++;
    cursor = packet + 4;
    packet[0] = 0x8000;
    packet[1] = 0x84000000;
    packet[2] = 0xF5252106;
    packet[3] = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & GLYPH_COLOR) {
            count--;
            palette = Message_GetColor(text_color(text[i]));
            color[0] = palette[0];
            color[1] = palette[4];
            color[2] = palette[8];
            color[3] = palette[12];
            continue;
        }
        if (i != 0 && (text[i - 1] == 0x800 || (text[i - 1] & 0x8000))) {
            word_breaks++;
        }
        next_column = column + 1;
        if (next_column % limit == 0) {
            break;
        }
        if ((text[i] & 0x8000) && text[i + 1] == 0xFFFF) {
            break;
        }
        if (dots == 3) {
            break;
        }
        column = next_column;
        if ((text[i] & GLYPH_HALF_WIDTH) || text[i] == 0x8000) {
            if (word_breaks < 3) {
                if (text[i] & 0x8000) {
                    glyph = 0;
                } else {
                    code = text[i]; // match: read again as an int, into its own variable
                    glyph = code & GLYPH_CODE_MASK;
                }
            } else {
                glyph = 0xE;
                dots++;
            }
            glyph += 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
        } else {
            glyph = text[i];
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[glyph / 625];
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            bottom_right.pos_x += text_glyph_size.pos_x;
            scale[1] = scale[0] = 0.0390625f;
        }
        *cursor++ = 0x56;
        *cursor++ = 0;
        *cursor++ = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = glyph_column * scale[0];
        *((f32 *)cursor)++ = glyph_row * scale[1];
        *cursor++ = (s32)top_left.pos_x | ((s32)top_left.pos_y << 16);
        *cursor++ = top_left.depth;
        *((f32 *)cursor)++ = (glyph_column + 1) * scale[0];
        *((f32 *)cursor)++ = (glyph_row + 1) * scale[1];
        *cursor++ = (s32)bottom_right.pos_x | ((s32)bottom_right.pos_y << 16);
        *cursor++ = top_left.depth;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    packet[0] = count | GIF_TAG_EOP;
    text_pen.pos_y = bottom_right.pos_y;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    return count;
}

/* Like MenuText_DrawLine (one line of text at the pen position text_pen, at most `limit` glyphs), but centred:
   the text is first measured, and the pen starts half of the unused width (a cell is half of text_glyph_size's
   width) further right. Returns the number of glyphs drawn. */
s32 MenuText_DrawLineCentered(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count;
    s32 i;
    s32 column;
    s32 length;
    s32 offset;
    s32 next_column;
    s32 glyph;
    s32 glyph_row;
    s32 glyph_column;

    packet = MenuDraw_NextPacket();
    cursor = packet; // match: set here, used after the loop below
    top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
    top_left.pos_y = text_pen.pos_y;
    bottom_right.pos_y = text_pen.pos_y + text_glyph_size.pos_y;
    top_left.depth = bottom_right.depth = text_pen.depth;
    for (length = 0; text[length] != 0xFFFF; length++) {
    }
    if (limit < length) {
        length = limit;
    }
    length = limit - length; // match: reused for the number of cells left over
    offset = ((s32)(text_glyph_size.pos_x * length) >> 1) / 2;
    top_left.pos_x += offset;
    bottom_right.pos_x += offset;
    color[0] = text_ink.red;
    color[1] = text_ink.green;
    color[2] = text_ink.blue;
    color[3] = text_ink.alpha;
    limit++;
    *cursor++ = 0x8000;
    *cursor++ = 0x84000000;
    *cursor++ = 0xF5252106;
    *cursor++ = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & GLYPH_COLOR) {
            count--;
            palette = Message_GetColor(text_color(text[i]));
            color[0] = palette[0];
            color[1] = palette[4];
            color[2] = palette[8];
            color[3] = palette[12];
            continue;
        }
        next_column = column + 1;
        if (next_column % limit == 0) {
            break;
        }
        if (text[i] & 0x8000) {
            break;
        }
        column = next_column;
        if (text[i] & GLYPH_HALF_WIDTH) {
            glyph = text[i]; // match: read again as an int, then two statements
            glyph = (glyph & GLYPH_CODE_MASK) + 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
        } else {
            glyph = text[i];
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[glyph / 625];
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            bottom_right.pos_x += text_glyph_size.pos_x;
            scale[1] = scale[0] = 0.0390625f;
        }
        *cursor++ = 0x56;
        *cursor++ = 0;
        *cursor++ = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = glyph_column * scale[0];
        *((f32 *)cursor)++ = glyph_row * scale[1];
        *cursor++ = (s32)top_left.pos_x | ((s32)top_left.pos_y << 16);
        *cursor++ = top_left.depth;
        *((f32 *)cursor)++ = (glyph_column + 1) * scale[0];
        *((f32 *)cursor)++ = (glyph_row + 1) * scale[1];
        *cursor++ = (s32)bottom_right.pos_x | ((s32)bottom_right.pos_y << 16);
        *cursor++ = top_left.depth;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    packet[0] = count | GIF_TAG_EOP;
    text_pen.pos_y = bottom_right.pos_y;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    return count;
}

/* Draws text at the pen position text_pen like MenuText_DrawLine, but over several lines of at most `limit`
   glyphs: a line is broken when it is full (codes 10..16, 0xE3..0xF9 and 0x80E may still hang on its end) or,
   at the start of a word, when the word no longer fits; a word ends at a space (0x800) or a full stop (0x80E).
   A broken line moves the pen down by text_glyph_size's height and draws the same code again, unless it is 0x48.
   Codes with bit 0x8000 are skipped. Returns the number of glyphs drawn. */
static s32 MenuText_DrawWordWrapped(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count;
    s32 i;
    s32 column = 0; // match: cleared here and again in the loop's initialiser
    s32 next_column;
    s32 word_length;
    s32 j;
    s32 glyph;
    s32 glyph_row;
    s32 glyph_column;

    packet = MenuDraw_NextPacket();
    color[0] = text_ink.red;
    color[1] = text_ink.green;
    color[2] = text_ink.blue;
    color[3] = text_ink.alpha;
    top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
    top_left.pos_y = text_pen.pos_y;
    bottom_right.pos_y = text_pen.pos_y + text_glyph_size.pos_y;
    top_left.depth = bottom_right.depth = text_pen.depth;
    limit++;
    cursor = packet + 4;
    packet[0] = 0x8000;
    packet[1] = 0x84000000;
    packet[2] = 0xF5252106;
    packet[3] = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & GLYPH_COLOR) {
            count--;
            palette = Message_GetColor(text_color(text[i]));
            color[0] = palette[0];
            color[1] = palette[4];
            color[2] = palette[8];
            color[3] = palette[12];
            continue;
        }
        if (i != 0 && text[i - 1] == 0x800) {
            // match: no test for the 0xFFFF that ends the text (MenuText_DrawWordWrappedLong has one)
            for (j = 0, word_length = 0; text[i + j] != 0x800 && text[i + j] != 0x80E; j++) {
                if (text[i + j] != 0x8000) {
                    word_length++;
                }
            }
        }
        next_column = column + 1;
        if ((next_column == limit
             && !((text[i] >= 10 && text[i] <= 16) || (text[i] >= 0xE3 && text[i] <= 0xF9) || text[i] == 0x80E))
            || next_column > limit || (i != 0 && text[i - 1] == 0x800 && !(column + word_length < limit))) {
            top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
            top_left.pos_y += text_glyph_size.pos_y;
            bottom_right.pos_y += text_glyph_size.pos_y;
            if (text[i] != 0x48) {
                i--;
            }
            count--;
            column = 0;
            continue;
        }
        if (text[i] & 0x8000) {
            count--;
            continue;
        }
        column = next_column;
        if (text[i] & GLYPH_HALF_WIDTH) {
            glyph = text[i]; // match: read again as an int, then two statements
            glyph = (glyph & GLYPH_CODE_MASK) + 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
        } else {
            glyph = text[i];
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *menu_font_page_tex0[glyph / 625];
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            bottom_right.pos_x += text_glyph_size.pos_x;
            scale[1] = scale[0] = 0.0390625f;
        }
        *cursor++ = 0x56;
        *cursor++ = 0;
        *cursor++ = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = glyph_column * scale[0];
        *((f32 *)cursor)++ = glyph_row * scale[1];
        *cursor++ = (s32)top_left.pos_x | ((s32)top_left.pos_y << 16);
        *cursor++ = top_left.depth;
        *((f32 *)cursor)++ = (glyph_column + 1) * scale[0];
        *((f32 *)cursor)++ = (glyph_row + 1) * scale[1];
        *cursor++ = (s32)bottom_right.pos_x | ((s32)bottom_right.pos_y << 16);
        *cursor++ = top_left.depth;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    packet[0] = count | GIF_TAG_EOP;
    text_pen.pos_y = bottom_right.pos_y;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | D_MADR_SPR;
    *D2_CHCR = D_CHCR_STR;
    return count;
}

static s32 word_wrapped_long_buffer = 0;    /* 0x446D7C the same for MenuText_DrawWordWrappedLong */

/* Draws a string of glyphs at the pen position text_pen, wrapping at `limit` glyphs a line and only between
   words, and sends the packet to the GS; returns the number of glyphs drawn. MenuText_DrawWordWrapped for a
   long text: the packet is built in one of the two large buffers long_text_packets, not in the scratchpad. */
s32 MenuText_DrawWordWrappedLong(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count;
    s32 i;
    s32 column = 0; // match: cleared again in the loop's init, which makes it a copy of count's register
    s32 next_column;
    s32 word_length;
    s32 j;
    s32 line_end;
    s32 glyph;
    s32 glyph_row;
    s32 glyph_column;
    s32 color_index;

    packet = long_text_packets[word_wrapped_long_buffer];
    word_wrapped_long_buffer = word_wrapped_long_buffer == 0;
    color[0] = text_ink.red;
    color[1] = text_ink.green;
    color[2] = text_ink.blue;
    color[3] = text_ink.alpha;
    top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
    top_left.pos_y = text_pen.pos_y;
    bottom_right.pos_y = text_pen.pos_y + text_glyph_size.pos_y;
    top_left.depth = bottom_right.depth = text_pen.depth;
    line_end = limit + 1;
    cursor = packet + 4;
    packet[0] = 0x8000;
    packet[1] = 0x84000000;
    packet[2] = 0xF5252106;
    packet[3] = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & GLYPH_COLOR) {
            /* a colour change, not a glyph */
            count--;
            color_index = text[i];
            palette = Message_GetColor((color_index & 0xFFF) >> 8);
            color[0] = palette[0];
            color[1] = palette[4];
            color[2] = palette[8];
            color[3] = palette[12];
            continue;
        }
        if (i != 0 && (text[i - 1] == 0x800 || text[i - 1] == 0x8000)) {
            /* the first glyph after a space: measure the word */
            for (j = 0, word_length = 0; text[i + j] != 0x800 && text[i + j] != 0xFFFF && text[i + j] != 0x8000; j++) {
                word_length++;
            }
        }
        next_column = column + 1;
        if ((next_column == line_end
             && !((text[i] >= 10 && text[i] <= 16) || (text[i] >= 0xE3 && text[i] <= 0xF9) || text[i] == 0x80E
                  || text[i] == 0x800 || text[i] == 0x8000))
            || next_column > line_end
            || (i != 0 && (text[i - 1] == 0x800 || text[i - 1] == 0x8000) && !(column + word_length < line_end))
            || (column == 0 && (text[i] == 0x800 || text[i] == 0x8000))) {
            if (column != 0 || (text[i] != 0x800 && text[i] != 0x8000)) {
                /* line feed; the glyph is drawn on the next pass */
                top_left.pos_x = bottom_right.pos_x = text_pen.pos_x;
                top_left.pos_y += text_glyph_size.pos_y;
                bottom_right.pos_y += text_glyph_size.pos_y;
                if (text[i] != 0x48) {
                    i--;
                }
            }
            count--;
            column = 0;
            continue;
        }
        column = next_column;
        if (text[i] & GLYPH_HALF_WIDTH) {
            /* a half-width glyph */
            glyph = text[i]; // match: three statements on the one variable, not one expression
            glyph &= GLYPH_CODE_MASK;
            glyph += 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
        } else if (text[i] == 0x8000) {
            /* a space: half-width glyph 0 */
            glyph = 0; // match: the zero keeps glyph % 50 from being folded
            glyph += 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += (s32)text_glyph_size.pos_x >> 1;
            *((u64 *)cursor)++ = *menu_font_page_tex0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
        } else {
            glyph = text[i];
            *((u64 *)cursor)++ = *menu_font_page_tex0[glyph / 625];
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += text_glyph_size.pos_x;
            scale[0] = 0.0390625f;
            scale[1] = 0.0390625f;
        }
        *cursor++ = 0x56;
        *cursor++ = 0;
        *cursor++ = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = glyph_column * scale[0];
        *((f32 *)cursor)++ = glyph_row * scale[1];
        *cursor++ = (s32)top_left.pos_x | ((s32)top_left.pos_y << 16);
        *cursor++ = top_left.depth;
        *((f32 *)cursor)++ = (glyph_column + 1) * scale[0];
        *((f32 *)cursor)++ = (glyph_row + 1) * scale[1];
        *cursor++ = (s32)bottom_right.pos_x | ((s32)bottom_right.pos_y << 16);
        *cursor++ = top_left.depth;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    packet[0] = count | GIF_TAG_EOP;
    text_pen.pos_y = bottom_right.pos_y;
    FlushCache(0);
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = (u32)packet & 0x7FFFFFFF;
    *D2_TADR = 0;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN;
    return count;
}

/* Turns the file offsets in a loaded part file (each part's strips, points and normals) into pointers. */
void PoolPartFile_Relocate(PoolPartFile *file) {
    u32 i;
    PoolPart *part;

    part = file->parts;
    for (i = 0; i < file->part_count; i++) {
        if (part->material != NULL) {
            part->material = (PoolPartMaterial *)((char *)file + (s32)part->material);
        }
        if (part->points != NULL) {
            part->points = (f32 (*)[4])((char *)file + (s32)part->points);
        }
        if (part->normals != NULL) {
            part->normals = (f32 (*)[4])((char *)file + (s32)part->normals);
        }
        part++;
    }
}

/* Turns the file offsets in a loaded node file (each node's keys, layers, child and sibling) into pointers.
   Returns 0. */
s32 PoolNodeFile_Relocate(RelocFile *file) {
    RelocEntry *entry;
    u32 i;
    s32 j;
    RelocEntry *cursor = file->entries;
    RelocLayer *layer;

    for (i = 0; i < file->entry_count; i++) {
        entry = cursor;
        if (entry->keys != NULL) {
            entry->keys = (char *)file + (s32)entry->keys;
        }
        if (entry->layers != NULL) {
            entry->layers = (RelocLayer *)((char *)file + (s32)entry->layers);
        }
        if (entry->child != NULL) {
            entry->child = (char *)file + (s32)entry->child;
        }
        if (entry->sibling != NULL) {
            entry->sibling = (char *)file + (s32)entry->sibling;
        }
        for (j = 0, layer = entry->layers; j < entry->layer_count; j++, layer++) {
            if (layer->layer_data != NULL) {
                layer->layer_data = (char *)file + (s32)layer->layer_data;
            }
            if (layer->layer_aux_data != NULL) {
                layer->layer_aux_data = (char *)file + (s32)layer->layer_aux_data;
            }
        }
        cursor = entry + 1;
    }
    return 0;
}

/* Draws a part in one flat colour (the four bytes of the third argument): turns its points into screen
   points, then sends each strip to the GS as it stands, its registers first, with the colour on every
   point, and, when the strip has a second texture (unk10), the same strip again with that texture and the
   points' normals as texture coordinates. Last a packet that sets ALPHA_1. */
void PoolPart_DrawTinted(PoolPart *part, s32 parent, RenderSlotArg color) {
    PoolDrawScratch *scratch = POOL_DRAW_SCRATCH;
    PoolDrawScratch *again;
    u32 *out;
    f32 unit[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    s32 source;
    s32 target;

    scratch->unity_color_scale[0] = 128.0f;
    scratch->unity_color_scale[1] = 128.0f;
    scratch->unity_color_scale[2] = 128.0f;
    scratch->unity_color_scale[3] = 128.0f;
    scratch->work = part_work_buffer;
    scratch->screen_points = part_work_buffer + 0x500;
    scratch->normals = part_work_buffer + 0xA00;
    *(Uint128 *)scratch->default_point_color = 0x00000080000000000000000000000000;
    scratch->point_count = part->point_count;
    scratch->strip_count = part->strip_count;
    scratch->normal_count = part->normal_count;
    scratch->point_index = 0;
    scratch->normal_index = 0;
    sceVu0RotMatrixX(scratch->world, unit, part->rotation[0]);
    sceVu0RotMatrixZ(scratch->world, scratch->world, part->rotation[2]);
    sceVu0RotMatrixY(scratch->world, scratch->world, part->rotation[1]);
    sceVu0TransMatrix(scratch->world, scratch->world, part->position);
    sceVu0MulMatrix(scratch->world, (void *)parent, scratch->world);
    sceVu0MulMatrix(scratch->screen, menu_view_screen_matrix, scratch->world);
    sceVu0MulMatrix(scratch->light, scratch->world, menu_model_light_directions);
    __asm__ volatile("lqc2 $vf12, 0x0(%0)\n"
                     "lqc2 $vf13, 0x10(%0)\n"
                     "lqc2 $vf14, 0x20(%0)\n"
                     "lqc2 $vf15, 0x30(%0)\n"
                     : : "r"(scratch->world));
    __asm__ volatile("lqc2 $vf1, 0x0(%0)\n"
                     "lqc2 $vf2, 0x10(%0)\n"
                     "lqc2 $vf3, 0x20(%0)\n"
                     "lqc2 $vf4, 0x30(%0)\n"
                     : : "r"(scratch->screen));
    scratch->point_index = 0;
    scratch->normal_index = 0;
    __asm__ volatile("lqc2 $vf5, 0x0(%0)\n"
                     "lqc2 $vf6, 0x10(%0)\n"
                     "lqc2 $vf7, 0x20(%0)\n"
                     : : "r"(scratch->light));
    __asm__ volatile("lqc2 $vf8, 0x0(%0)\n"
                     "lqc2 $vf9, 0x10(%0)\n"
                     "lqc2 $vf10, 0x20(%0)\n"
                     "lqc2 $vf11, 0x30(%0)\n"
                     : : "r"(menu_model_light_colors));
    __asm__ volatile("vaddw.xyzw $vf20, $vf0, $vf0w\n"
                     "vdiv Q, $vf0w, $vf20w\n"
                     "vmul.xyzw $vf21, $vf0, $vf0\n"
                     "vwaitq\n"
                     "vmulq.w $vf21, $vf0, Q\n");
    for (scratch->point_at = 0; scratch->point_at < scratch->point_count; scratch->point_at++) {
        source = scratch->point_index * 16 + (s32)part->points;
        target = scratch->point_index++ * 16 + (s32)scratch->screen_points;
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf1, $vf16x\n"
                         "vmadday.xyzw ACC, $vf2, $vf16y\n"
                         "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
                         "vmaddw.xyzw $vf17, $vf4, $vf16w\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vdiv Q, $vf0w, $vf17w\n"
                         "vwaitq\n"
                         "vmulq.xyzw $vf26, $vf17, Q\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vftoi4.xyzw $vf17, $vf26\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(target), "r"(source));
    }
    for (scratch->normal = 0; scratch->normal < scratch->normal_count; scratch->normal++) {
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf12, $vf16x\n"
                         "vmadday.xyzw ACC, $vf13, $vf16y\n"
                         "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
                         "vaddw.xy $vf17, $vf17, $vf0w\n"
                         "vmulw.xy $vf17, $vf17, $vf21w\n"
                         "vaddw.z $vf17, $vf0, $vf0w\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(scratch->normal_index * 16 + (s32)scratch->normals), "r"(scratch->normal_index * 16 + (s32)part->normals));
        scratch->normal_index++;
    }
    FlushCache(0);
    scratch->prim_tag[2] = 0x412;
    scratch->reg_tag[0] = 0x8008;
    scratch->reg_tag[1] = 0x10004000;
    scratch->reg_tag[2] = 0xE;
    scratch->prim_tag[3] = 0;
    scratch->strip_at = part->material;
    scratch->reg_tag[3] = 0;
    scratch->tex0 = scratch->strip_at->tex0; // match: every strip is sent with the first strip's TEX0
    for (scratch->strip = 0; scratch->strip < scratch->strip_count; scratch->strip++) {
        scratch->cursor = MenuDraw_NextPacket();
        scratch->packet = scratch->cursor;
        *scratch->cursor++ = scratch->reg_tag[0];
        *scratch->cursor++ = scratch->reg_tag[1];
        *scratch->cursor++ = scratch->reg_tag[2];
        *scratch->cursor++ = scratch->reg_tag[3];
        scratch->registers = (u64 *)scratch->cursor;
        scratch->strip_point = scratch->strip_at->points;
        scratch->strip_start = scratch->strip_at;
        ((PoolPartMaterialBits *)scratch->strip_start)->cld = 4; // match: written to the strip's own TEX0, which is never sent
        scratch->registers[0] = 0;
        scratch->registers[1] = 0;
        scratch->registers[4] = scratch->tex0;
        scratch->registers[5] = 7;
        scratch->registers[2] = 0x60;
        scratch->registers[3] = 0x15;
        scratch->registers[6] = 0;
        scratch->registers[7] = 9;
        scratch->cursor += 0x10;
        *((u64 *)scratch->cursor)++ = 4;
        *((u64 *)scratch->cursor)++ = 0x1C;
        *((u64 *)scratch->cursor)++ = scratch->tex0;
        *((u64 *)scratch->cursor)++ = 0x17;
        *scratch->cursor++ = scratch->strip_at->alpha_2;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x43;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x50000;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x48;
        *scratch->cursor++ = 0;
        scratch->prim_tag[0] = scratch->strip_at->point_count | GIF_TAG_EOP;
        scratch->prim_tag[1] = 0x31004000;
        scratch->prim_tag[1] |= (scratch->strip_at->prim & GS_PRIM_MASK) << 15;
        *scratch->cursor++ = scratch->prim_tag[0];
        *scratch->cursor++ = scratch->prim_tag[1];
        *scratch->cursor++ = scratch->prim_tag[2];
        *scratch->cursor++ = scratch->prim_tag[3];
        for (scratch->point = 0; scratch->point < scratch->strip_at->point_count; scratch->point++) {
            ((f32 *)scratch->cursor)[0] = scratch->strip_point->tex_s;
            ((f32 *)scratch->cursor)[1] = scratch->strip_point->tex_t;
            ((f32 *)scratch->cursor)[2] = 1.0f;
            POOL_DRAW_SCRATCH_CURSOR(scratch).words += 4; // match: through the union this store waits for the float stores, which decides the loop's registers
            scratch->cursor[0] = ((u8 *)&color)[0];
            again = POOL_DRAW_SCRATCH; // match: a second pointer the compiler cannot fold: its reload of the cursor is the daddu copy
            out = again->cursor;
            out[1] = ((u8 *)&color)[1];
            out[2] = ((u8 *)&color)[2];
            out[3] = ((u8 *)&color)[3];
            __asm__ volatile("lq $9, 0x0(%0)\n"
                             "sq $9, 0x0(%1)\n"
                             : : "r"(&scratch->screen_points[scratch->strip_point->point]), "r"(out + 4) : "$9");
            scratch->cursor = out + 8;
            scratch->strip_point++;
        }
        if (scratch->strip_at->second_tex0 != 0) {
            scratch->strip_point = scratch->strip_at->points;
            *scratch->cursor++ = scratch->reg_tag[0];
            *scratch->cursor++ = scratch->reg_tag[1];
            *scratch->cursor++ = scratch->reg_tag[2];
            *scratch->cursor++ = scratch->reg_tag[3];
            scratch->registers = (u64 *)scratch->cursor;
            scratch->strip_start = scratch->strip_at;
            ((PoolPartMaterialBits *)scratch->strip_start)->cld = 4; // match: as above
            scratch->registers[0] = 0;
            scratch->registers[1] = 0;
            scratch->registers[4] = scratch->strip_at->second_tex0 | GS_TEX0_TFX_DECAL;
            scratch->registers[5] = 7;
            scratch->registers[2] = 0x60;
            scratch->registers[3] = 0x15;
            scratch->registers[6] = 0;
            scratch->registers[7] = 9;
            scratch->cursor += 0x10;
            *((u64 *)scratch->cursor)++ = 4;
            *((u64 *)scratch->cursor)++ = 0x1C;
            *((u64 *)scratch->cursor)++ = scratch->strip_at->second_tex0;
            *((u64 *)scratch->cursor)++ = 0x17;
            *((u64 *)scratch->cursor)++ = 9;
            *scratch->cursor++ = 0x43;
            *scratch->cursor++ = 0;
            *scratch->cursor++ = 0x50000;
            *scratch->cursor++ = 0;
            *scratch->cursor++ = 0x48;
            *scratch->cursor++ = 0;
            scratch->prim_tag[0] = scratch->strip_at->point_count | GIF_TAG_EOP;
            scratch->prim_tag[1] = 0x31004000;
            scratch->prim_tag[1] |= (scratch->strip_at->prim & GS_PRIM_MASK) << 15;
            scratch->prim_tag[1] |= GIF_TAG_PRIM_ABE;
            scratch->prim_tag[1] |= GIF_TAG_PRIM_TME;
            *scratch->cursor++ = scratch->prim_tag[0];
            *scratch->cursor++ = scratch->prim_tag[1];
            *scratch->cursor++ = scratch->prim_tag[2];
            *scratch->cursor++ = scratch->prim_tag[3];
            for (scratch->point = 0; scratch->point < scratch->strip_at->point_count; scratch->point++) {
                __asm__ volatile("lq $9, 0x0(%0)\n"
                                 "sq $9, 0x0(%1)\n"
                                 : : "r"(&scratch->normals[scratch->strip_point->normal]), "r"(scratch->cursor) : "$9");
                __asm__ volatile("lq $9, 0x0(%0)\n"
                                 "lq $8, 0x0(%2)\n"
                                 "sq $9, 0x0(%1)\n"
                                 "sq $8, 0x0(%3)\n"
                                 : : "r"(&scratch->screen_points[scratch->strip_point->point]), "r"(scratch->cursor + 8), "r"(scratch->default_point_color), "r"(scratch->cursor + 4) : "$8", "$9");
                scratch->cursor += 12;
                scratch->strip_point++;
            }
        }
        scratch->strip_at = (PoolPartMaterial *)scratch->strip_point;
        scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
        if (scratch->packet_size < 3) { // match: never true, the packet is 9 quadwords before its points
            return;
        }
        send_packet(scratch->packet, scratch->packet_size);
    }
    scratch->cursor = MenuDraw_NextPacket();
    scratch->packet = scratch->cursor;
    *scratch->cursor++ = 0x8001;
    *scratch->cursor++ = 0x10004000;
    *scratch->cursor++ = 0xE;
    *scratch->cursor++ = 0;
    *((u64 *)scratch->cursor)++ = 0x44;
    *scratch->cursor++ = 0x42;
    *scratch->cursor++ = 0;
    scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
    send_packet(scratch->packet, scratch->packet_size);
}

/* Draws a part: turns its points into screen points, then sends each strip to the GS as it stands,
   its registers first, and last a packet that sets ALPHA_1. */
void PoolPart_Draw(PoolPart *part, s32 parent) {
    PoolDrawScratch *scratch = POOL_DRAW_SCRATCH;
    f32 unit[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    s32 source;
    s32 target;

    scratch->unity_color_scale[0] = 128.0f;
    scratch->unity_color_scale[1] = 128.0f;
    scratch->unity_color_scale[2] = 128.0f;
    scratch->unity_color_scale[3] = 128.0f;
    scratch->work = part_work_buffer;
    scratch->screen_points = part_work_buffer + 0x500;
    scratch->normals = part_work_buffer + 0xA00;
    *(Uint128 *)scratch->default_point_color = 0x00000080000000000000000000000000;
    scratch->point_count = part->point_count;
    scratch->strip_count = part->strip_count;
    scratch->normal_count = part->normal_count;
    scratch->point_index = 0;
    scratch->normal_index = 0;
    sceVu0RotMatrixX(scratch->world, unit, part->rotation[0]);
    sceVu0RotMatrixZ(scratch->world, scratch->world, part->rotation[2]);
    sceVu0RotMatrixY(scratch->world, scratch->world, part->rotation[1]);
    sceVu0TransMatrix(scratch->world, scratch->world, part->position);
    sceVu0MulMatrix(scratch->world, (void *)parent, scratch->world);
    sceVu0MulMatrix(scratch->screen, menu_view_screen_matrix, scratch->world);
    sceVu0MulMatrix(scratch->light, scratch->world, menu_model_light_directions);
    __asm__ volatile("lqc2 $vf12, 0x0(%0)\n"
                     "lqc2 $vf13, 0x10(%0)\n"
                     "lqc2 $vf14, 0x20(%0)\n"
                     "lqc2 $vf15, 0x30(%0)\n"
                     : : "r"(scratch->world));
    __asm__ volatile("lqc2 $vf1, 0x0(%0)\n"
                     "lqc2 $vf2, 0x10(%0)\n"
                     "lqc2 $vf3, 0x20(%0)\n"
                     "lqc2 $vf4, 0x30(%0)\n"
                     : : "r"(scratch->screen));
    scratch->point_index = 0; // match: both zeroed a second time
    scratch->normal_index = 0;
    __asm__ volatile("lqc2 $vf5, 0x0(%0)\n"
                     "lqc2 $vf6, 0x10(%0)\n"
                     "lqc2 $vf7, 0x20(%0)\n"
                     : : "r"(scratch->light));
    __asm__ volatile("lqc2 $vf8, 0x0(%0)\n"
                     "lqc2 $vf9, 0x10(%0)\n"
                     "lqc2 $vf10, 0x20(%0)\n"
                     "lqc2 $vf11, 0x30(%0)\n"
                     : : "r"(menu_model_light_colors));
    __asm__ volatile("vaddw.xyzw $vf20, $vf0, $vf0w\n"
                     "vdiv Q, $vf0w, $vf20w\n"
                     "vmul.xyzw $vf21, $vf0, $vf0\n"
                     "vwaitq\n"
                     "vmulq.w $vf21, $vf0, Q\n");
    for (scratch->point_at = 0; scratch->point_at < scratch->point_count; scratch->point_at++) {
        source = scratch->point_index * 16 + (s32)part->points;
        target = scratch->point_index++ * 16 + (s32)scratch->screen_points;
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf1, $vf16x\n"
                         "vmadday.xyzw ACC, $vf2, $vf16y\n"
                         "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
                         "vmaddw.xyzw $vf17, $vf4, $vf16w\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vdiv Q, $vf0w, $vf17w\n"
                         "vwaitq\n"
                         "vmulq.xyzw $vf26, $vf17, Q\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vftoi4.xyzw $vf17, $vf26\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(target), "r"(source));
    }
    /* match: the normals are transformed, and vf5 to vf11 loaded above, though nothing here uses them */
    for (scratch->normal = 0; scratch->normal < scratch->normal_count; scratch->normal++) {
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf12, $vf16x\n"
                         "vmadday.xyzw ACC, $vf13, $vf16y\n"
                         "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
                         "vaddw.xy $vf17, $vf17, $vf0w\n"
                         "vmulw.xy $vf17, $vf17, $vf21w\n"
                         "vaddw.z $vf17, $vf0, $vf0w\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(scratch->normal_index * 16 + (s32)scratch->normals), "r"(scratch->normal_index * 16 + (s32)part->normals));
        scratch->normal_index++;
    }
    FlushCache(0);
    scratch->prim_tag[2] = 0x412;
    scratch->reg_tag[0] = 0x8008;
    scratch->reg_tag[1] = 0x10004000;
    scratch->reg_tag[2] = 0xE;
    scratch->prim_tag[3] = 0;
    scratch->strip_at = part->material;
    scratch->reg_tag[3] = 0;
    scratch->tex0 = scratch->strip_at->tex0; // match: the first strip's TEX0 serves every strip
    for (scratch->strip = 0; scratch->strip < scratch->strip_count; scratch->strip++) {
        scratch->cursor = MenuDraw_NextPacket();
        scratch->packet = scratch->cursor;
        *scratch->cursor++ = scratch->reg_tag[0];
        *scratch->cursor++ = scratch->reg_tag[1];
        *scratch->cursor++ = scratch->reg_tag[2];
        *scratch->cursor++ = scratch->reg_tag[3];
        scratch->registers = (u64 *)scratch->cursor;
        scratch->strip_point = scratch->strip_at->points;
        scratch->strip_start = scratch->strip_at;
        ((PoolPartMaterialBits *)scratch->strip_start)->cld = 4; // match: set in the strip's own TEX0, which is not the one sent
        scratch->registers[0] = 0;
        scratch->registers[1] = 0;
        scratch->registers[4] = scratch->tex0;
        scratch->registers[5] = 7;
        scratch->registers[2] = 0x60;
        scratch->registers[3] = 0x15;
        scratch->registers[6] = 0;
        scratch->registers[7] = 9;
        scratch->cursor += 0x10;
        *((u64 *)scratch->cursor)++ = 4;
        *((u64 *)scratch->cursor)++ = 0x1C;
        *((u64 *)scratch->cursor)++ = scratch->tex0;
        *((u64 *)scratch->cursor)++ = 0x17;
        *scratch->cursor++ = scratch->strip_at->alpha_2;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x43;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x50000;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x48;
        *scratch->cursor++ = 0;
        scratch->prim_tag[0] = scratch->strip_at->point_count | GIF_TAG_EOP;
        scratch->prim_tag[1] = 0x31004000; // match: two statements, or the constant is not kept in a register across the loop
        scratch->prim_tag[1] |= (scratch->strip_at->prim & GS_PRIM_MASK) << 15;
        *scratch->cursor++ = scratch->prim_tag[0];
        *scratch->cursor++ = scratch->prim_tag[1];
        *scratch->cursor++ = scratch->prim_tag[2];
        *scratch->cursor++ = scratch->prim_tag[3];
        for (scratch->point = 0; scratch->point < scratch->strip_at->point_count; scratch->point++) {
            ((f32 *)scratch->cursor)[0] = scratch->strip_point->tex_s;
            ((f32 *)scratch->cursor)[1] = scratch->strip_point->tex_t;
            ((f32 *)scratch->cursor)[2] = 1.0f;
            __asm__ volatile("lqc2 $vf16, 0x0(%0)\n"
                             "vftoi0.xyzw $vf17, $vf16\n"
                             "sqc2 $vf17, 0x0(%1)\n"
                             : : "r"(scratch->strip_point->color), "r"(scratch->cursor + 4));
            __asm__ volatile("lq $9, 0x0(%0)\n"
                             "sq $9, 0x0(%1)\n"
                             : : "r"(&scratch->screen_points[scratch->strip_point->point]), "r"(scratch->cursor + 8) : "$9");
            scratch->cursor += 12;
            scratch->strip_point++;
        }
        scratch->strip_at = (PoolPartMaterial *)scratch->strip_point;
        scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
        if (scratch->packet_size < 3) { // match: never true, the packet is 9 quadwords before its points
            return;
        }
        send_packet(scratch->packet, scratch->packet_size);
    }
    scratch->cursor = MenuDraw_NextPacket();
    scratch->packet = scratch->cursor;
    *scratch->cursor++ = 0x8001;
    *scratch->cursor++ = 0x10004000;
    *scratch->cursor++ = 0xE;
    *scratch->cursor++ = 0;
    *((u64 *)scratch->cursor)++ = 0x44;
    *scratch->cursor++ = 0x42;
    *scratch->cursor++ = 0;
    scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
    send_packet(scratch->packet, scratch->packet_size);
}

static u8 tinted_model_page = 0;            /* 0x446D80 which place in GS memory MenuModel_DrawTinted's texture goes, always 0 */
static u8 model_page = 0;                   /* 0x446D81 the same for MenuModel_Draw */

/* Uploads a loaded model's texture and draws its parts with PoolPart_DrawTinted. */
void MenuModel_DrawTinted(ChunkUpload *upload, s32 draw_param, RenderSlotArg slot_arg) {
    u64 texture_pages[1] = { 0x2840 }; // match: one entry, though indexed like clut_pages (tinted_model_page is always 0)
    u64 clut_pages[2] = { 0x3D68, 0x3D6C };
    KeyItemModelFile *file;
    TextureHeader *texture;
    PoolPartFile *part_file;
    PoolPart *part;
    u32 i;

    if (upload != NULL && upload->done != 0) {
        file = upload->pool_node->buffer;
        texture = (TextureHeader *)((char *)file + file->texture_offset);
        part_file = (PoolPartFile *)((char *)file + file->part_offset);
        TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages[tinted_model_page];
        TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages[tinted_model_page];
        MenuTexture_Upload(texture, NULL);
        FlushCache(0);
        part = part_file->parts;
        for (i = 0; i < part_file->part_count; i++) {
            part->material->tex0 = texture->tex0;
            PoolPart_DrawTinted(part, draw_param, slot_arg);
            part++;
        }
    }
}

/* Uploads a loaded model's texture and draws its parts with PoolPart_Draw. */
void MenuModel_Draw(ChunkUpload *upload, s32 draw_param) {
    u64 texture_pages[1] = { 0x2840 }; // match: one entry, though indexed like clut_pages (model_page is always 0)
    u64 clut_pages[2] = { 0x3D68, 0x3D6C };
    KeyItemModelFile *file;
    TextureHeader *texture;
    PoolPartFile *part_file;
    PoolPart *part;
    u32 i;

    if (upload != NULL && upload->done != 0) {
        file = upload->pool_node->buffer;
        texture = (TextureHeader *)((char *)file + file->texture_offset);
        part_file = (PoolPartFile *)((char *)file + file->part_offset);
        TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages[model_page];
        TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages[model_page];
        MenuTexture_Upload(texture, NULL);
        FlushCache(0);
        part = part_file->parts;
        for (i = 0; i < part_file->part_count; i++) {
            part->material->tex0 = texture->tex0;
            PoolPart_Draw(part, draw_param);
            part++;
        }
    }
}

/* Draws a slot's part like PoolPart_DrawTinted does a part, but with the slot's own matrix, and, when the part
   has a record, with its first points and normals moved by the parent slot's matrix instead. */
void PoolSlot_Draw(RenderSlot *slot, RenderSlotArg unused) {
    PoolDrawScratch *scratch = POOL_DRAW_SCRATCH;
    // match: a unit matrix and a vector that are set up and never read
    f32 unit[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FVECTOR unused_vector = { 0.0f, 128.0f, 0.0f, 128.0f };
    PoolPart *part;
    PoolPartRecord *record;
    s32 source;
    s32 target;

    part = POOL(slot)->part;
    if (part == NULL) {
        return;
    }
    scratch->work = part_work_buffer;
    scratch->screen_points = part_work_buffer + 0x500;
    scratch->normals = part_work_buffer + 0xA00;
    scratch->unity_color_scale[0] = 128.0f;
    scratch->unity_color_scale[1] = 128.0f;
    scratch->unity_color_scale[2] = 128.0f;
    scratch->unity_color_scale[3] = 128.0f;
    *(Uint128 *)scratch->default_point_color = 0x00000080000000000000000000000000;
    scratch->point_count = part->point_count;
    scratch->strip_count = part->strip_count;
    scratch->normal_count = part->normal_count;
    scratch->point_index = 0;
    scratch->normal_index = 0;
    record = POOL(slot)->part_record;
    wait_gif();
    if (record != NULL) {
        scratch->parent_point_count = record->parent_point_count;
        scratch->parent_normal_count = record->parent_normal_count;
        sceVu0MulMatrix(scratch->screen, menu_view_screen_matrix, POOL(slot)->parent);
        sceVu0MulMatrix(scratch->light, POOL(slot)->parent, menu_model_light_directions);
        __asm__ volatile("lqc2 $vf12, 0x0(%0)\n"
                         "lqc2 $vf13, 0x10(%0)\n"
                         "lqc2 $vf14, 0x20(%0)\n"
                         "lqc2 $vf15, 0x30(%0)\n"
                         : : "r"(POOL(slot)->parent));
        __asm__ volatile("lqc2 $vf1, 0x0(%0)\n"
                         "lqc2 $vf2, 0x10(%0)\n"
                         "lqc2 $vf3, 0x20(%0)\n"
                         "lqc2 $vf4, 0x30(%0)\n"
                         : : "r"(scratch->screen));
        __asm__ volatile("lqc2 $vf5, 0x0(%0)\n"
                         "lqc2 $vf6, 0x10(%0)\n"
                         "lqc2 $vf7, 0x20(%0)\n"
                         : : "r"(scratch->light));
        __asm__ volatile("lqc2 $vf8, 0x0(%0)\n"
                         "lqc2 $vf9, 0x10(%0)\n"
                         "lqc2 $vf10, 0x20(%0)\n"
                         "lqc2 $vf11, 0x30(%0)\n"
                         : : "r"(menu_model_light_colors));
        __asm__ volatile("vaddw.xyzw $vf20, $vf0, $vf0w\n"
                         "vdiv Q, $vf0w, $vf20w\n"
                         "vmul.xyzw $vf21, $vf0, $vf0\n"
                         "vwaitq\n"
                         "vmulq.w $vf21, $vf0, Q\n");
        for (scratch->point_at = 0; scratch->point_at < scratch->parent_point_count; scratch->point_at++) {
            __asm__ volatile("lqc2 $vf16, 0x0(%0)\n"
                             "vmulax.xyzw ACC, $vf1, $vf16x\n"
                             "vmadday.xyzw ACC, $vf2, $vf16y\n"
                             "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
                             "vmaddw.xyzw $vf17, $vf4, $vf16w\n"
                             "vnop\n"
                             "vnop\n"
                             "vnop\n"
                             "vdiv Q, $vf0w, $vf17w\n"
                             "vwaitq\n"
                             "vmulq.xyzw $vf26, $vf17, Q\n"
                             "vnop\n"
                             "vnop\n"
                             "vnop\n"
                             "vftoi4.xyzw $vf17, $vf26\n"
                             "vnop\n"
                             "vnop\n"
                             "vnop\n"
                             "sqc2 $vf17, 0x0(%1)\n"
                             : : "r"(scratch->point_at * 16 + (s32)part->points), "r"(scratch->point_at * 16 + (s32)scratch->screen_points));
            scratch->point_index++;
        }
        scratch->point_index = scratch->parent_point_count;
        for (scratch->normal = 0; scratch->normal < scratch->parent_normal_count; scratch->normal++) {
            __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                             "vmulax.xyzw ACC, $vf12, $vf16x\n"
                             "vmadday.xyzw ACC, $vf13, $vf16y\n"
                             "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
                             "vaddw.xy $vf17, $vf17, $vf0w\n"
                             "vmulw.xy $vf17, $vf17, $vf21w\n"
                             "vaddw.z $vf17, $vf0, $vf0w\n"
                             "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(scratch->normal * 16 + (s32)scratch->normals), "r"(scratch->normal * 16 + (s32)part->normals));
            scratch->normal_index++;
        }
        scratch->normal_index = scratch->parent_normal_count;
    }
    sceVu0MulMatrix(scratch->screen, menu_view_screen_matrix, slot);
    sceVu0MulMatrix(scratch->light, slot, menu_model_light_directions);
    __asm__ volatile("lqc2 $vf12, 0x0(%0)\n"
                     "lqc2 $vf13, 0x10(%0)\n"
                     "lqc2 $vf14, 0x20(%0)\n"
                     "lqc2 $vf15, 0x30(%0)\n"
                     : : "r"(slot));
    __asm__ volatile("lqc2 $vf1, 0x0(%0)\n"
                     "lqc2 $vf2, 0x10(%0)\n"
                     "lqc2 $vf3, 0x20(%0)\n"
                     "lqc2 $vf4, 0x30(%0)\n"
                     : : "r"(scratch->screen));
    __asm__ volatile("lqc2 $vf5, 0x0(%0)\n"
                     "lqc2 $vf6, 0x10(%0)\n"
                     "lqc2 $vf7, 0x20(%0)\n"
                     : : "r"(scratch->light));
    __asm__ volatile("lqc2 $vf8, 0x0(%0)\n"
                     "lqc2 $vf9, 0x10(%0)\n"
                     "lqc2 $vf10, 0x20(%0)\n"
                     "lqc2 $vf11, 0x30(%0)\n"
                     : : "r"(menu_model_light_colors));
    __asm__ volatile("vaddw.xyzw $vf20, $vf0, $vf0w\n"
                     "vdiv Q, $vf0w, $vf20w\n"
                     "vmul.xyzw $vf21, $vf0, $vf0\n"
                     "vwaitq\n"
                     "vmulq.w $vf21, $vf0, Q\n");
    while (scratch->point_index < scratch->point_count) {
        source = scratch->point_index * 16 + (s32)part->points;
        target = scratch->point_index++ * 16 + (s32)scratch->screen_points;
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf1, $vf16x\n"
                         "vmadday.xyzw ACC, $vf2, $vf16y\n"
                         "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
                         "vmaddw.xyzw $vf17, $vf4, $vf16w\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vdiv Q, $vf0w, $vf17w\n"
                         "vwaitq\n"
                         "vmulq.xyzw $vf26, $vf17, Q\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vftoi4.xyzw $vf17, $vf26\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(target), "r"(source));
    }
    while (scratch->normal_index < scratch->normal_count) {
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf12, $vf16x\n"
                         "vmadday.xyzw ACC, $vf13, $vf16y\n"
                         "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
                         "vaddw.xy $vf17, $vf17, $vf0w\n"
                         "vmulw.xy $vf17, $vf17, $vf21w\n"
                         "vaddw.z $vf17, $vf0, $vf0w\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(scratch->normal_index * 16 + (s32)scratch->normals), "r"(scratch->normal_index * 16 + (s32)part->normals));
        scratch->normal_index++;
    }
    FlushCache(0);
    scratch->prim_tag[2] = 0x412;
    scratch->strip_at = part->material;
    scratch->reg_tag[0] = 0x8008;
    scratch->reg_tag[1] = 0x10004000;
    scratch->reg_tag[2] = 0xE;
    scratch->tex0 = POOL(slot)->tex0; // match: the slot's TEX0 serves every strip
    scratch->prim_tag[3] = 0;
    scratch->reg_tag[3] = 0;
    for (scratch->strip = 0; scratch->strip < scratch->strip_count; scratch->strip++) {
        scratch->cursor = MenuDraw_NextPacket();
        scratch->packet = scratch->cursor;
        *scratch->cursor++ = scratch->reg_tag[0];
        *scratch->cursor++ = scratch->reg_tag[1];
        *scratch->cursor++ = scratch->reg_tag[2];
        *scratch->cursor++ = scratch->reg_tag[3];
        scratch->registers = (u64 *)scratch->cursor;
        scratch->strip_point = scratch->strip_at->points;
        scratch->strip_start = scratch->strip_at;
        ((PoolPartMaterialBits *)scratch->strip_start)->cld = 4; // match: set in the strip's own TEX0, which is not the one sent
        scratch->registers[0] = 0;
        scratch->registers[1] = 0;
        scratch->registers[4] = scratch->tex0;
        scratch->registers[5] = 6;
        scratch->registers[2] = 0x60;
        scratch->registers[3] = 0x15;
        scratch->registers[6] = 0;
        scratch->registers[7] = 9;
        scratch->cursor += 0x10;
        *((u64 *)scratch->cursor)++ = 4;
        *((u64 *)scratch->cursor)++ = 0x1C;
        *((u64 *)scratch->cursor)++ = scratch->tex0;
        *((u64 *)scratch->cursor)++ = 0x16;
        *scratch->cursor++ = scratch->strip_at->alpha_2;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x42;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x50000;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x47;
        *scratch->cursor++ = 0;
        scratch->prim_tag[0] = scratch->strip_at->point_count | GIF_TAG_EOP;
        scratch->prim_tag[1] = 0x30004000;
        scratch->prim_tag[1] |= (scratch->strip_at->prim & GS_PRIM_MASK) << 15;
        *scratch->cursor++ = scratch->prim_tag[0];
        *scratch->cursor++ = scratch->prim_tag[1];
        *scratch->cursor++ = scratch->prim_tag[2];
        *scratch->cursor++ = scratch->prim_tag[3];
        for (scratch->point = 0; scratch->point < scratch->strip_at->point_count; scratch->point++) {
            ((f32 *)scratch->cursor)[0] = scratch->strip_point->tex_s;
            ((f32 *)scratch->cursor)[1] = scratch->strip_point->tex_t;
            ((f32 *)scratch->cursor)[2] = 1.0f;
            __asm__ volatile("lqc2 $vf16, 0x0(%0)\n"
                             "vftoi0.xyzw $vf17, $vf16\n"
                             "sqc2 $vf17, 0x0(%1)\n"
                             : : "r"(scratch->strip_point->color), "r"(scratch->cursor + 4));
            __asm__ volatile("lq $9, 0x0(%0)\n"
                             "sq $9, 0x0(%1)\n"
                             : : "r"(&scratch->screen_points[scratch->strip_point->point]), "r"(scratch->cursor + 8) : "$9");
            scratch->cursor += 12;
            scratch->strip_point++;
        }
        if (scratch->strip_at->second_tex0 != 0) {
            scratch->strip_point = scratch->strip_at->points;
            *scratch->cursor++ = scratch->reg_tag[0];
            *scratch->cursor++ = scratch->reg_tag[1];
            *scratch->cursor++ = scratch->reg_tag[2];
            *scratch->cursor++ = scratch->reg_tag[3];
            scratch->registers = (u64 *)scratch->cursor;
            scratch->strip_start = scratch->strip_at;
            ((PoolPartMaterialBits *)scratch->strip_start)->cld = 4;
            scratch->registers[1] = 0x3F;
            scratch->registers[0] = 0;
            scratch->registers[4] = scratch->strip_at->second_tex0 | GS_TEX0_TFX_DECAL;
            scratch->registers[5] = 6;
            scratch->registers[2] = 0x60;
            scratch->registers[3] = 0x15;
            scratch->registers[6] = 0;
            scratch->registers[7] = 9;
            scratch->cursor += 0x10;
            *((u64 *)scratch->cursor)++ = 4;
            *((u64 *)scratch->cursor)++ = 0x1C;
            *((u64 *)scratch->cursor)++ = scratch->strip_at->second_tex0 | GS_TEX0_TFX_DECAL;
            *((u64 *)scratch->cursor)++ = 0x16;
            *((u64 *)scratch->cursor)++ = 9;
            *scratch->cursor++ = 0x42;
            *scratch->cursor++ = 0;
            *scratch->cursor++ = 0x50000;
            *scratch->cursor++ = 0;
            *scratch->cursor++ = 0x47;
            *scratch->cursor++ = 0;
            scratch->prim_tag[0] = scratch->strip_at->point_count | GIF_TAG_EOP;
            scratch->prim_tag[1] = 0x30004000;
            scratch->prim_tag[1] |= (scratch->strip_at->prim & GS_PRIM_MASK) << 15;
            scratch->prim_tag[1] |= GIF_TAG_PRIM_ABE;
            scratch->prim_tag[1] |= GIF_TAG_PRIM_TME;
            *scratch->cursor++ = scratch->prim_tag[0];
            *scratch->cursor++ = scratch->prim_tag[1];
            *scratch->cursor++ = scratch->prim_tag[2];
            *scratch->cursor++ = scratch->prim_tag[3];
            for (scratch->point = 0; scratch->point < scratch->strip_at->point_count; scratch->point++) {
                __asm__ volatile("lq $9, 0x0(%0)\n"
                                 "sq $9, 0x0(%1)\n"
                                 : : "r"(&scratch->normals[scratch->strip_point->normal]), "r"(scratch->cursor) : "$9");
                __asm__ volatile("lq $9, 0x0(%0)\n"
                                 "lq $8, 0x0(%2)\n"
                                 "sq $9, 0x0(%1)\n"
                                 "sq $8, 0x0(%3)\n"
                                 : : "r"(&scratch->screen_points[scratch->strip_point->point]), "r"(scratch->cursor + 8), "r"(scratch->default_point_color), "r"(scratch->cursor + 4) : "$8", "$9");
                scratch->cursor += 12;
                scratch->strip_point++;
            }
        }
        scratch->strip_at = (PoolPartMaterial *)scratch->strip_point;
        scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
        if (scratch->packet_size < 3) { // match: never true, the packet is 9 quadwords before its points
            return;
        }
        send_packet(scratch->packet, scratch->packet_size);
    }
    scratch->cursor = MenuDraw_NextPacket();
    scratch->packet = scratch->cursor;
    *scratch->cursor++ = 0x8001;
    *scratch->cursor++ = 0x10004000;
    *scratch->cursor++ = 0xE;
    *scratch->cursor++ = 0;
    *((u64 *)scratch->cursor)++ = 0x44;
    *scratch->cursor++ = 0x42;
    *scratch->cursor++ = 0;
    scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
    send_packet(scratch->packet, scratch->packet_size);
}

/* Runs PoolSlot_Draw on the first 21 slots of the slot array. */
void PoolSlot_DrawAll(RenderSlot *slots, RenderSlotArg slot_arg) {
    s32 slot; // match: the slots are walked as integers (a signed compare ends the loop)
    s32 end;

    slot = (s32)slots;
    end = slot + 21 * sizeof(RenderSlot);
    do {
        PoolSlot_Draw((RenderSlot *)slot, slot_arg);
        slot += sizeof(RenderSlot);
    } while (slot < end);
}

/* Runs PoolSlot_Draw on the first 20 slots of the slot array. */
void PoolSlot_DrawFirst20(RenderSlot *slots, RenderSlotArg slot_arg) {
    int i;

    for (i = 0; i < 20; i++) {
        PoolSlot_Draw(&slots[i], slot_arg);
    }
}

/* Runs PoolSlot_Draw on slots 0-14 and 16-20 of the slot array. */
void PoolSlot_DrawAllBut15(RenderSlot *slots, RenderSlotArg slot_arg) {
    int i;

    for (i = 0; i < 15; i++) {
        PoolSlot_Draw(&slots[i], slot_arg);
    }
    for (i = 16; i < 21; i++) {
        PoolSlot_Draw(&slots[i], slot_arg);
    }
}

/* Runs PoolSlot_Draw on slots 0-14 and 16-19 of the slot array. */
void PoolSlot_DrawFirst20But15(RenderSlot *slots, RenderSlotArg slot_arg) {
    int i;

    for (i = 0; i < 15; i++) {
        PoolSlot_Draw(&slots[i], slot_arg);
    }
    for (i = 16; i < 20; i++) {
        PoolSlot_Draw(&slots[i], slot_arg);
    }
}

/* Gives the first 21 slots of the pool the file's parts, in order. */
static void PoolSlot_SetAllParts(PoolPartFile *file) {
    u32 i;

    for (i = 0; i < 21; i++) {
        pool_slots[i].part = &file->parts[i];
    }
}

/* Gives the slots of one group the file's parts in order; group 5 loses its part instead. */
void PoolSlot_SetGroupParts(PoolPartFile *file, s32 group) {
    u8 group0[7] = { 0, 1, 2, 3, 4, 5, 6 };
    u8 group1[4] = { 7, 8, 11, 16 };
    u8 group2[2] = { 9, 10 };
    u8 group3[6] = { 17, 18, 19, 12, 13, 14 };
    u8 slots15[1] = { 15 };
    u8 slots20[1] = { 20 };
    u8 *groups[6] = { group0, group1, group2, group3, slots15, slots20 };
    u32 group_sizes[6] = { 7, 4, 2, 6, 1, 1 };
    PoolPart *part;
    u32 i;

    if (group != 5) {
        part = file->parts;
        for (i = 0; i < group_sizes[group]; i++) {
            pool_slots[groups[group][i]].part = part;
            part++;
        }
    } else {
        for (i = 0; i < group_sizes[group]; i++) {
            pool_slots[groups[group][i]].part = NULL;
        }
    }
}

/* Chains the 32 pool slots into the free list pool_slot_free_list and points pool_slot_buffer_table[0] at pool_slot_spare_buffer. */
static void PoolSlot_InitFreeList(void) {
    PoolSlot **link;
    PoolSlot *next_slot;
    s32 i;

    pool_slot_buffer_table[0] = pool_slot_spare_buffer;
    pool_slot_free_list = pool_slots;
    link = &pool_slots[0].next;
    next_slot = pool_slots + 1;
    for (i = 0x1F; i != 0; i--) {
        *link = next_slot;
        link += sizeof(PoolSlot) / sizeof(PoolSlot *); // match: steps the next pointer one slot on
        next_slot++;
    }
    pool_slots[31].next = NULL;
}

/* Uploads a group's texture to its place in GS memory and gives the group's slots the file's parts. */
void PoolSlot_LoadGroup(PoolPartSource *source, u32 group) {
    u8 group0[7] = { 0, 1, 2, 3, 4, 5, 6 };
    u8 group1[4] = { 7, 8, 11, 16 };
    u8 group2[2] = { 9, 10 };
    u8 group3[6] = { 17, 18, 19, 12, 13, 14 };
    u8 group4[2] = { 15, 20 };
    u8 slots20[1] = { 20 };
    u8 slots21[1] = { 21 };
    u8 slots22[1] = { 22 };
    u8 slots23[1] = { 23 };
    u8 *groups[9] = {
        group0, group1, group2, group3, group4, slots20, slots21, slots22, slots23
    };
    u32 group_sizes[10] = { 7, 4, 2, 6, 1, 1, 1, 1, 1, 1 };
    PoolPartFile *file = source->file;
    TextureHeader *texture = source->texture;
    u32 texture_pages[11] = { 0x1A40, 0x1AC0, 0x1B40, 0x1BC0, 0x1C40, 0x1CC0, 0x1D40, 0x1DC0, 0x1E40, 0x1EC0, 0x1F40 };
    u32 clut_pages[11] = { 0x3D40, 0x3D44, 0x3D48, 0x3D4C, 0x3D50, 0x3D54, 0x3D58, 0x3D5C, 0x3D60, 0x3D64, 0x3D68 };
    PoolPart *part;
    PoolPartRecord *record;
    u8 *slots;
    u32 i;
    u32 remaining;

    TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages[group];
    TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages[group];
    MenuTexture_Upload(texture, NULL);
    if (group < 6) {
        if (group == 4) {
            group_sizes[4] = file->part_count;
        }
        part = file->parts;
        for (i = 0; i < group_sizes[group]; i++) {
            slots = groups[group];
            remaining = group_sizes[group] - i;
            pool_slots[slots[i]].part = part;
            // match: tex0 and unk18 go across as one 16-byte struct
            POOL_SLOT_REGISTERS(&pool_slots[slots[i]]) = *(PoolSlotRegisters *)&texture->tex0;
            record = (PoolPartRecord *)((char *)file + (s32)file->records_end) - remaining;
            if (record->parent_point_count > 0) {
                pool_slots[slots[i]].part_record = record;
                pool_slots[slots[i]].parent = &pool_slots[slots[record->parent_slot_index]];
            } else {
                pool_slots[slots[i]].part_record = NULL;
                pool_slots[slots[i]].parent = NULL;
            }
            part++;
        }
    } else {
        part = file->parts;
        pool_slots[groups[group][0]].part = part;
        POOL_SLOT_REGISTERS(&pool_slots[groups[group][0]]) = *(PoolSlotRegisters *)&texture->tex0; // match: as above
        pool_slots[groups[group][0]].part_record = NULL;
        pool_slots[groups[group][0]].parent = NULL;
    }
}

/* Takes the part away from every slot of the pool. */
s32 PoolSlot_ClearParts(void) {
    u32 i;

    for (i = 0; i < 32; i++) {
        pool_slots[i].part = NULL;
    }
    return TRUE;
}

/* Blends two points and their tangents with the cubic Hermite weights for phase. */
static void PoolNode_BlendKeys(void *out, void *start, void *end, void *start_tangent, void *end_tangent, float phase) {
    float weights[4];
    float t_sq = phase * phase;
    float t_cube = t_sq * phase;

    weights[0] = t_cube + t_cube - 3.0f * t_sq + 1.0f;
    weights[1] = t_cube * -2.0f + 3.0f * t_sq;
    weights[2] = t_cube - 2.0f * t_sq + phase;
    weights[3] = t_cube - t_sq;
    Vector_WeightedSum4(out, start, end, start_tangent, end_tangent, weights);
}

/* Poses a node of a skeleton at a frame of its animation, into its slot's matrix, then its children and siblings. */
void PoolNode_Pose(RelocEntry *node, PoolSlot *slots, f32 *parent_matrix, s32 frame) {
    f32 local[4][4];
    f32 unused[4][4]; /* unused, but retail reserves the 0x40 bytes */
    f32 unit[4][4] = { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } };
    f32 rotation[4];
    f32 translation[4];
    f32 scale[4];
    f32 key_scale[4];
    f32 next_scale[4];
    PoolSlot *slot;
    PoolNodeKey *key;
    PoolNodeKey *next;
    f32 phase;

    slot = &slots[node->slot]; // match: node is read here, before the test for NULL below
    if (node != NULL) {
        if (frame == 0) {
            slot->key_index = 0;
        }
        // match: with no keys, key, next and phase are used below without being set
        if (node->keys != NULL) {
            key = &((PoolNodeKey *)node->keys)[slot->key_index];
            for (;;) {
                if (frame < key->frame + key->length) {
                    phase = (f32)(frame - key->frame) / (f32)key->length;
                    next = key + 1;
                    break;
                }
                next = key;
                if (node->key_count < (u32)(slot->key_index + 1)) {
                    phase = 0.0f;
                    break;
                }
                slot->key_index++;
                key++;
            }
        }
        PoolNode_BlendKeys(rotation, key->rotation, next->rotation, key->rotation_out, next->rotation_in, phase);
        sceVu0RotMatrixX(local, unit, WRAP_ANGLE(rotation[0]));
        sceVu0RotMatrixZ(local, local, WRAP_ANGLE(rotation[2]));
        sceVu0RotMatrixY(local, local, WRAP_ANGLE(rotation[1]));
        PoolNode_BlendKeys(translation, key->translation, next->translation, key->translation_out, next->translation_in, phase);
        sceVu0TransMatrix(local[0], local[0], translation);
        sceVu0ScaleVectorXYZ(key_scale, key->scale, 1.0f - phase);
        sceVu0ScaleVectorXYZ(next_scale, next->scale, phase);
        sceVu0AddVector(scale, key_scale, next_scale);
        sceVu0ScaleVectorXYZ(local[0], local[0], scale[0]);
        sceVu0ScaleVectorXYZ(local[1], local[1], scale[1]);
        sceVu0ScaleVectorXYZ(local[2], local[2], scale[2]);
    } else {
        sceVu0RotMatrixX(local, unit, WRAP_ANGLE(slot->part->rotation[0]));
        sceVu0RotMatrixY(local, local, WRAP_ANGLE(slot->part->rotation[1]));
        sceVu0RotMatrixZ(local, local, WRAP_ANGLE(slot->part->rotation[2]));
        sceVu0TransMatrix(local[0], local[0], slot->part->position);
        sceVu0ScaleVectorXYZ(local[0], local[0], slot->part->scale[0]);
        sceVu0ScaleVectorXYZ(local[1], local[1], slot->part->scale[1]);
        sceVu0ScaleVectorXYZ(local[2], local[2], slot->part->scale[2]);
    }
    sceVu0MulMatrix(slot, parent_matrix, local);
    if (node->child != NULL) {
        PoolNode_Pose(node->child, slots, (f32 *)slot, frame);
    }
    if (node->sibling != NULL) {
        PoolNode_Pose(node->sibling, slots, parent_matrix, frame);
    }
}

/* Copies a part file with every strip point expanded to its position and normal (0x40 bytes a point);
   the strips go right behind the copy's parts. */
void PoolPartFile_Expand(PoolPartFile *source, PoolPartFile *dest) {
    PoolPart *source_part;
    PoolPart *dest_part;
    PoolPartFlatStrip *dest_strip;
    PoolPartMaterial *source_strip;
    PoolPartFlatPoint *dest_point;
    PoolPartPoint *source_point;
    f32 (*normals)[4];
    f32 (*points)[4];
    s32 part_count;
    s32 strip_count;
    s32 point_count;

    *(PoolPartFileHeader *)dest = *(PoolPartFileHeader *)source;
    part_count = source->part_count;
    dest_part = dest->parts;
    source_part = source->parts;
    dest_strip = (PoolPartFlatStrip *)(dest_part + part_count);
    while (--part_count != -1) {
        *dest_part = *source_part;
        source_strip = source_part->material;
        strip_count = dest_part->strip_count;
        points = source_part->points;
        normals = source_part->normals;
        dest_part->material = (PoolPartMaterial *)dest_strip;
        while (--strip_count != -1) {
            // match: the strip's 0x20 bytes go over a word at a time through a union, between the other statements
            ((PoolPartStripWords *)dest_strip)->words[0] = ((PoolPartStripWords *)source_strip)->words[0];
            dest_point = dest_strip->points;
            source_point = source_strip->points;
            ((PoolPartStripWords *)dest_strip)->words[1] = ((PoolPartStripWords *)source_strip)->words[1];
            point_count = dest_strip->point_count;
            ((PoolPartStripWords *)dest_strip)->words[2] = ((PoolPartStripWords *)source_strip)->words[2];
            ((PoolPartStripWords *)dest_strip)->words[3] = ((PoolPartStripWords *)source_strip)->words[3];
            while (--point_count != -1) {
                sceVu0CopyVector(dest_point->position, points[source_point->point]);
                sceVu0CopyVector(dest_point->normal, normals[source_point->normal]);
                dest_point->tex_s = source_point->tex_s;
                dest_point->tex_t = source_point->tex_t;
                dest_point->tex_q = 1.0f;
                dest_point->tex_w = 0.0f;
                dest_point->color[0] = source_point->color[0];
                dest_point->color[1] = source_point->color[1];
                dest_point->color[2] = source_point->color[2];
                dest_point->color[3] = source_point->color[3];
                source_point++;
                dest_point++;
            }
            source_strip = (PoolPartMaterial *)source_point;
            dest_strip = (PoolPartFlatStrip *)dest_point;
        }
        source_part++;
        dest_part++;
    }
}

/* Draws a sprite: runs sprite handler `index` on it and draws what the handler gives back by its kind
   (0: a textured rectangle, 1: a word, 2: a number). */
void MenuSprite_DrawPlain(int owner, int index, int param) {
    SpriteHandler handler = sprite_handlers[index];
    KindObject *object = NULL;

    if (handler != NULL) {
        object = handler(owner, param);
    }
    if (object != NULL) {
        switch (object->kind) {
        case 0:
            MenuDraw_Sprite(object);
            break;
        case 1:
            MenuDraw_Word(object);
            break;
        case 2:
            MenuDraw_Number(object);
            break;
        }
    }
}

/* Reads a whole file through the game's file functions into buffer; returns buffer, or 0 when the file
   does not open. */
int MenuFile_Load(char *name, void *buffer) {
    int file_handle;

    file_handle = File_Open(name, 0, 1);
    if (file_handle < 0) {
        return 0;
    }
    File_Read(file_handle, buffer, -1);
    File_Close(file_handle);
    FlushCache(0);
    return (int)buffer;
}

/* Reads a whole file into buffer; returns buffer, or NULL when the file does not open. */
static void *MenuFile_LoadFromHost(const char *name, void *buffer) {
    s32 file_handle;
    s32 end_offset;

    file_handle = sceOpen(name, 1);
    if (file_handle < 0) {
        return NULL;
    }
    end_offset = sceLseek(file_handle, 0, 2);
    sceRead(file_handle, buffer, end_offset - sceLseek(file_handle, 0, 0));
    sceClose(file_handle);
    FlushCache(0);
    return buffer;
}

/* Reads the first 0x19000 bytes of a file with sceRead; returns 1, or 0 when the file does not open. */
static s32 MenuFile_LoadHostBlock(const char *name, void *buffer) {
    s32 file_handle;

    file_handle = sceOpen(name, 0x8001);
    if (file_handle < 0) {
        return 0;
    }
    sceRead(file_handle, buffer, 0x19000);
    sceClose(file_handle);
    FlushCache(0);
    return 1;
}

/* Notes the blocks of the loaded menu file: how many in menu_file_block_count, and where each one is in menu_file_blocks. */
void MenuFile_IndexBlocks(char *file) {
    TextureTableEntry *entries = (TextureTableEntry *)(file + sizeof(TextureTableHeader));
    int i;
    int count = ((TextureTableHeader *)file)->entry_count;

    menu_file_block_count = count;
    for (i = 0; i < count; i++) {
        s32 offset = entries[i].offset;
        LoadedBlockRef *block = &menu_file_blocks[i];

        block->data = (char *)(offset + (s32)file);
    }
}

/* Sends a texture's image to GS memory, in two halves when it is large, and then its CLUT when the pixel format has one. */
void MenuTexture_Upload(TextureHeader *header, LoadedBlockRef *block) {
    sceGsLoadImage clut_load;
    sceGsLoadImage image_load;
    u64 buffer_width;
    u32 width;
    u32 height;
    u32 pixel_size;
    int pixel_format;
    int clut_width;
    int clut_height;

    buffer_width = 1 << HEADER_TEX0(header)->tw;
    HEADER_TEX0(header)->tbw = buffer_width >> 6;
    width = 1 << HEADER_TEX0(header)->tw;
    height = 1 << HEADER_TEX0(header)->th;
    pixel_format = HEADER_TEX0(header)->psm;
    if (pixel_format == 0x13 || pixel_format == 0x1B) {
        pixel_size = 2;
    } else if (pixel_format == 0x14 || pixel_format == 0x2C || pixel_format == 0x24) {
        pixel_size = 1;
    } else {
        pixel_size = 8;
    }
    if (width * height * pixel_size > 0x7FFFF) {
        sceGsSetDefLoadImage(&image_load, HEADER_TEX0(header)->tbp0, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, height >> 1);
        FlushCache(0);
        sceGsExecLoadImage(&image_load, (char *)header + sizeof(TextureHeader));
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
        }
        sceGsSetDefLoadImage(&image_load, HEADER_TEX0(header)->tbp0, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, height >> 1, width, height >> 1);
        FlushCache(0);
        sceGsExecLoadImage(&image_load, (char *)header + ((width * height * pixel_size >> 2) + sizeof(TextureHeader)));
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
        }
    } else {
        sceGsSetDefLoadImage(&image_load, HEADER_TEX0(header)->tbp0, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, height);
        FlushCache(0);
        sceGsExecLoadImage(&image_load, (char *)header + sizeof(TextureHeader));
        while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
        }
    }
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
    pixel_format = HEADER_TEX0(header)->psm;
    if ((pixel_format >> 4) == 0 || (pixel_format >> 4) == 3) {
        return;
    }
    clut_width = (pixel_format & GS_PSM_4BIT_PALETTE) ? 8 : 16;
    clut_height = (pixel_format & GS_PSM_4BIT_PALETTE) ? 2 : 16;
    sceGsSetDefLoadImage(&clut_load, HEADER_TEX0(header)->cbp, 1, HEADER_TEX0(header)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&clut_load, (char *)header + (header->image_size + sizeof(TextureHeader)));
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
}

/* Sends a texture's image to GS memory a strip of rows at a time, and then its CLUT when the pixel format has one. */
static void MenuTexture_UploadInStrips(TextureHeader *header) {
    sceGsLoadImage load_image;
    u16 width = 1 << HEADER_TEX0(header)->tw;
    u16 height = 1 << HEADER_TEX0(header)->th;
    u16 base_pointer = HEADER_TEX0(header)->tbp0;
    char *image;
    f32 strip_rows;
    f32 rows;
    int pixel_format;
    int clut_width;
    int clut_height;

    // match: menu_font_page_tex0 is a table of pointers; its address is never NULL and the store overwrites two of them
    if (menu_font_page_tex0 != NULL) {
        *(u64 *)menu_font_page_tex0 = header->tex0;
    }
    image = (char *)header + sizeof(TextureHeader);
    strip_rows = 131068.0f / (((f32)(u32)header->data_offset - 16.0f) / width);
    strip_rows = (int)(strip_rows * 0.015625f) << 6;
    rows = (height < strip_rows) ? height : strip_rows;
    strip_rows = rows;
    while (height != 0) {
        if (rows < height) {
            sceGsSetDefLoadImage(&load_image, base_pointer, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, (int)strip_rows);
            FlushCache(0);
            while (sceGsExecLoadImage(&load_image, image) != 0) {
            }
            height -= (int)strip_rows;
            base_pointer += HEADER_TEX0(header)->tbw * (int)strip_rows;
            image += (HEADER_TEX0(header)->tbw * (int)strip_rows) << 8;
        } else {
            sceGsSetDefLoadImage(&load_image, base_pointer, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, height);
            FlushCache(0);
            sceGsExecLoadImage(&load_image, image);
            height = 0;
        }
    }
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
    pixel_format = HEADER_TEX0(header)->psm;
    if ((pixel_format >> 4) == 0 || (pixel_format >> 4) == 3) {
        return;
    }
    clut_width = (pixel_format & GS_PSM_4BIT_PALETTE) ? 8 : 16;
    clut_height = (pixel_format & GS_PSM_4BIT_PALETTE) ? 2 : 16;
    sceGsSetDefLoadImage(&load_image, HEADER_TEX0(header)->cbp, 1, HEADER_TEX0(header)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
}

/* Sends a texture's image to GS memory a strip of rows at a time, then its CLUT and a second CLUT from the file; hands back
   the TEX0 word and the word after the first CLUT, which is the second CLUT's TEX0. */
void MenuTexture_UploadTwoCluts(TextureHeader *header, u64 *tex0_out, u64 *trailer_out) {
    sceGsLoadImage load_image;
    sceGsLoadImage second_load;
    u16 width = 1 << HEADER_TEX0(header)->tw;
    u16 height = 1 << HEADER_TEX0(header)->th;
    u16 base_pointer = HEADER_TEX0(header)->tbp0;
    char *image;
    f32 strip_rows;
    f32 rows;
    int pixel_format;
    u16 clut_width;
    u16 clut_height;
    f32 pixel_bytes;
    u64 *trailer;

    if (tex0_out != NULL) {
        *tex0_out = header->tex0;
    }
    // match: no code; without it gcc loads TEX0 into other registers ($7/$5 in retail), same instructions otherwise
    __asm__ volatile("" : : : "memory");
    image = (char *)header + sizeof(TextureHeader);
    switch (HEADER_TEX0(header)->psm) {
    case 0x00:
    case 0x30:
        pixel_bytes = 4.0f;
        break;
    case 0x01:
    case 0x31:
        pixel_bytes = 3.0f;
        break;
    case 0x02:
    case 0x0A:
    case 0x32:
    case 0x3A:
        pixel_bytes = 2.0f;
        break;
    case 0x13:
    case 0x1B:
        pixel_bytes = 1.0f;
        break;
    default:
        pixel_bytes = 0.5f;
        break;
    }
    pixel_bytes *= width * height;
    strip_rows = 131068.0f / ((pixel_bytes - 16.0f) / width);
    strip_rows = (int)(strip_rows * 0.015625f) << 6;
    rows = (height < strip_rows) ? height : strip_rows;
    strip_rows = rows;
    while (height != 0) {
        if (rows < height) {
            sceGsSetDefLoadImage(&load_image, base_pointer, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, (int)strip_rows);
            FlushCache(0);
            while (sceGsExecLoadImage(&load_image, image) != 0) {
            }
            height -= (int)strip_rows;
            base_pointer += HEADER_TEX0(header)->tbw * (int)strip_rows;
            image += (HEADER_TEX0(header)->tbw * (int)strip_rows) << 8;
        } else {
            sceGsSetDefLoadImage(&load_image, base_pointer, HEADER_TEX0(header)->tbw, HEADER_TEX0(header)->psm, 0, 0, width, height);
            FlushCache(0);
            sceGsExecLoadImage(&load_image, image);
            height = 0;
        }
    }
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
    pixel_format = HEADER_TEX0(header)->psm;
    if ((pixel_format >> 4) == 0 || (pixel_format >> 4) == 3) {
        return;
    }
    clut_width = 16;
    if (pixel_format & GS_PSM_4BIT_PALETTE) {
        clut_width = 8;
    }
    clut_height = 16;
    if (pixel_format & GS_PSM_4BIT_PALETTE) {
        clut_height = 2;
    }
    sceGsSetDefLoadImage(&load_image, HEADER_TEX0(header)->cbp, 1, HEADER_TEX0(header)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)header + (header->image_size + sizeof(TextureHeader)));
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
    trailer = (u64 *)((char *)header + clut_width * clut_height * 4 + (header->image_size + sizeof(TextureHeader)));
    if (trailer_out != NULL) {
        *trailer_out = *trailer;
    }
    sceGsSetDefLoadImage(&second_load, ((BootTex0 *)trailer)->cbp, 1, ((BootTex0 *)trailer)->cpsm, 0, 0, clut_width, clut_height);
    FlushCache(0);
    sceGsExecLoadImage(&second_load, (char *)header + header->second_clut_offset);
    while ((*D2_CHCR & D_CHCR_STR) || (*GIF_STAT & GIF_STAT_APATH)) {
    }
}

/* Copies out a texture's TEX0 word and, for a palettized PSM, the word after its CLUT. */
void MenuTexture_GetRegisters(TextureHeader *tex, u64 *tex0_out, u64 *trailer_out) {
    char buf[0xC0]; /* unused, but retail reserves the 0xC0 bytes */
    u64 *trailer;
    int psm;
    int clut_w;
    int clut_h;

    if (tex0_out != NULL) {
        *tex0_out = tex->tex0;
    }
    psm = (int)GS_TEX0_PSM(tex->tex0);
    if ((psm >> 4) == 0 || (psm >> 4) == 3) {
        return;
    }
    clut_w = (psm & GS_PSM_4BIT_PALETTE) ? 8 : 16;
    clut_h = (psm & GS_PSM_4BIT_PALETTE) ? 2 : 16;
    trailer = (u64 *)((char *)tex + (clut_w * clut_h) * 4 + (tex->image_size + 0x20));
    if (trailer_out != NULL) {
        *trailer_out = *trailer;
    }
}

/* Sprite handler: the sprite as it is. */
static SpritePacket *MenuSprite_Keep(SpritePacket *packet) {
    return packet;
}

/* Sprite handler: nothing. */
static SpritePacket *MenuSprite_Hide(void) {
    return NULL;
}

/* Sprite handler: a copy of the sprite when its slot is the one given, else nothing. */
static SpritePacket *MenuSprite_OnlySlot(SpritePacket *packet, s32 slot) {
    static SpritePacket copy; /* 0x5DD400 */
    if (packet->slot == slot) {
        copy = *packet;
        return &copy;
    }
    return NULL;
}

/* A copy of the sprite moved 15 pixels left when its slot is the one given, else the sprite as it is.
   Not in the handler table and never called. */
static SpritePacket *MenuSprite_NudgeSlot(SpritePacket *packet, int slot) {
    static SpritePacket copy; /* 0x5DD460 */
    if (packet->slot == slot) {
        copy = *packet;
        copy.xyz0.X -= 0xF0;
        copy.xyz1.X -= 0xF0;
        return &copy;
    }
    return packet;
}

/* Sprite handler: a copy of the sprite moved 15 pixels left when its slot is the one given, else nothing. */
static SpritePacket *MenuSprite_OnlySlotNudged(SpritePacket *packet, int slot) {
    static SpritePacket copy; /* 0x5DD4C0 */
    if (packet->slot == slot) {
        copy = *packet;
        copy.xyz0.X -= 0xF0;
        copy.xyz1.X -= 0xF0;
        return &copy;
    }
    return NULL;
}

/* Sprite handler: a copy of the sprite moved 15 pixels left, or nothing when its slot is the one given. */
static SpritePacket *MenuSprite_ExceptSlotNudged(SpritePacket *packet, int slot) {
    static SpritePacket copy; /* 0x5DD520 */
    if (packet->slot != slot) {
        copy = *packet;
        copy.xyz0.X -= 0xF0;
        copy.xyz1.X -= 0xF0;
        return &copy;
    }
    return NULL;
}

/* Sprite handler: a copy of the sprite moved left by amount / unk41 of the screen's width. */
static SpritePacket *MenuSprite_SlideLeft(SpritePacket *packet, int amount) {
    static SpritePacket copy; /* 0x5DD580 */
    float ratio = (float)amount / (float)packet->slide_range;
    int offset;

    copy = *packet;
    offset = (int)(ratio * 320.0f) * 16;
    copy.xyz0.X -= offset;
    copy.xyz1.X -= offset;
    return &copy;
}

/* Sprite handler: a copy of the sprite moved right by amount / unk41 of the screen's width. */
static SpritePacket *MenuSprite_SlideRight(SpritePacket *src_packet, int amount) {
    static SpritePacket copy; /* 0x5DD5E0 */
    float ratio = (float)amount / (float)src_packet->slide_range;
    int offset;

    copy = *src_packet;
    offset = (int)(ratio * 320.0f) * 0x10;
    copy.xyz0.X += offset;
    copy.xyz1.X += offset;
    return &copy;
}

/* Sprite handler: a copy of the sprite moved up by amount / unk41 of the screen's height. */
static SpritePacket *MenuSprite_SlideUp(SpritePacket *src_packet, int amount) {
    static SpritePacket copy; /* 0x5DD640 */
    float ratio = (float)amount / (float)src_packet->slide_range;
    int offset;

    copy = *src_packet;
    offset = (int)(ratio * 224.0f) * 0x10;
    copy.xyz0.Y -= offset;
    copy.xyz1.Y -= offset;
    // match: the two clamps below can never run (Y is an unsigned field), yet they leave retail's 0x10 frame
    if (copy.xyz0.Y < 0) {
        copy.xyz0.Y = 0;
    }
    if (copy.xyz1.Y < 0) {
        copy.xyz1.Y = 0;
    }
    return &copy;
}

/* Sprite handler: a copy of the sprite moved down by amount / unk41 of the screen's height, no further than 0xFFF0. */
static SpritePacket *MenuSprite_SlideDown(SpritePacket *packet, int amount) {
    static SpritePacket copy; /* 0x5DD6A0 */
    float ratio = (float)amount / (float)packet->slide_range;
    int offset;

    copy = *packet;
    offset = (int)(ratio * 224.0f) << 4;
    copy.xyz0.Y += offset;
    copy.xyz1.Y += offset;
    if (copy.xyz0.Y > 0xFFF0) {
        copy.xyz0.Y = 0xFFF0;
    }
    if (copy.xyz1.Y > 0xFFF0) {
        copy.xyz1.Y = 0xFFF0;
    }
    return &copy;
}

/* Sprite handler: a copy of the sprite with the alpha passed in. */
static SpritePacket *MenuSprite_Fade(SpritePacket *src_packet, int alpha) {
    static SpritePacket copy; /* 0x5DD700 */
    copy = *src_packet;
    copy.alpha = alpha;
    return &copy;
}

/* Sprite handler: the same as MenuSprite_SlideRight, with a copy of its own. */
static SpritePacket *MenuSprite_SlideRightAlt(SpritePacket *src_packet, int amount) {
    static SpritePacket copy; /* 0x5DD760 */
    float ratio = (float)amount / (float)src_packet->slide_range;
    int offset;

    copy = *src_packet;
    offset = (int)(ratio * 320.0f) * 0x10;
    copy.xyz0.X += offset;
    copy.xyz1.X += offset;
    return &copy;
}

/* Sprite handler 12: empty. */
static void MenuSprite_Stub12(void) {
}

/* Sprite handler 15: empty. */
static void MenuSprite_Stub15(void) {
}

/* Sprite handler 14: empty. */
static void MenuSprite_Stub14(void) {
}

/* Sprite handler: a copy of the sprite, blue when the number it stands for (44 to 52) is above what the
   weapon menu_number_72 gives at its level, red when it is below. */
static SpritePacket *MenuSprite_TintWeaponStat(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DD7C0 */
    int current;
    int base;

    copy = *packet;
    current = MenuDraw_GetNumber(packet->number_id);
    switch (packet->number_id) {
    case 0x2C:
        base = (weapon_level_records[menu_number_72].gained_values[0] + weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[0]) * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x2D:
        base = (weapon_level_records[menu_number_72].gained_values[1] + weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[1]) * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x2E:
        base = (weapon_level_records[menu_number_72].gained_values[2] + weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[2]) * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x2F:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[3] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x30:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[4] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x31:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[5] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x32:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[6] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x33:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[7] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x34:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[8] * weapon_level_records[menu_number_72].level / 100;
        break;
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        copy.red = 0x40;
        copy.green = 0x40;
        copy.blue = 0xFF;
    } else if (current - base < 0) {
        copy.red = 0xFF;
        copy.green = copy.blue = 0x20;
    }
    return &copy;
}

/* Sprite handler: a copy of the sprite, blue when the number it stands for (44 to 52) is above what the
   worn item `index` gives at its level, red when it is below. */
static SpritePacket *MenuSprite_TintWornStat(SpritePacket *packet, int index) {
    static SpritePacket copy; /* 0x5DD820 */
    int current;
    int base;

    copy = *packet;
    current = MenuDraw_GetNumber(packet->number_id);
    switch (packet->number_id) {
    case 0x2C:
        base = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
        break;
    case 0x2D:
        base = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
        break;
    case 0x2E:
        base = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
        break;
    case 0x2F:
        base = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
        break;
    case 0x30:
        base = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
        break;
    case 0x31:
        base = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
        break;
    case 0x32:
        base = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
        break;
    case 0x33:
        base = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
        break;
    case 0x34:
        base = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
        break;
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        copy.red = 0x40;
        copy.green = 0x40;
        copy.blue = 0xFF;
    } else if (current - base < 0) {
        copy.red = 0xFF;
        copy.green = copy.blue = 0x20;
    }
    return &copy;
}

/* Sprite handler: a copy of the sprite, blue when the number it stands for (44 to 51) is above what the
   player's magic gives, red when it is below. */
static SpritePacket *MenuSprite_TintMagicStat(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DD880 */
    int current;
    int base;

    copy = *packet;
    current = MenuDraw_GetNumber(packet->number_id);
    switch (packet->number_id) {
    case 0x2C:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[0];
        break;
    case 0x2D:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[1];
        break;
    case 0x2E:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[2];
        break;
    case 0x2F:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[3];
        break;
    case 0x30:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[4];
        break;
    case 0x31:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[5];
        break;
    case 0x32:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[6];
        break;
    case 0x33:
        base = fx_table[player_state.equipped_magic].hit_checks[0].strengths.element_strengths[7];
        break;
    case 0x34:
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        copy.red = 0x40;
        copy.green = 0x40;
        copy.blue = 0xFF;
    } else if (current - base < 0) {
        copy.red = 0xFF;
        copy.green = copy.blue = 0x20;
    }
    return &copy;
}

/* Sprite handler 19: empty. */
static void MenuSprite_Stub19(void) {
}

/* Sprite handler: a copy of the sprite, opaque (alpha 0x80) when its slot's bit is set in the mask, else clear. */
static SpritePacket *MenuSprite_ShowIfBitSet(SpritePacket *packet, s32 slot_mask) {
    static SpritePacket copy; /* 0x5DD8E0 */
    copy = *packet;
    if ((slot_mask >> packet->slot) & 1) {
        copy.alpha = 0x80;
    } else {
        copy.alpha = 0;
    }
    return &copy;
}

/* Sprite handler: the sprite when its slot is the chosen one of its group of options (each group follows
   one byte of the options block game_options), nothing when it is not. */
static SpritePacket *MenuSprite_ShowIfOptionChosen(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DD940 */
    copy = *packet;
    switch (packet->slot) {
    case 0:
    case 1:
        if (packet->slot == game_options[0]) {
            return packet; // match: the packet itself, not the copy just made
        }
        break;
    case 2:
    case 3:
        if (packet->slot - 2 == head_bob_option[0]) {
            return packet;
        }
        break;
    case 4:
    case 5:
        if (packet->slot - 4 == rumble_option[0]) {
            return packet;
        }
        break;
    case 6:
    case 7:
        if (packet->slot - 6 == hud_option[0]) {
            return packet;
        }
        break;
    case 8:
    case 9:
    case 10:
        if (packet->slot - 8 == menu_backdrop_option[0]) {
            return packet;
        }
        break;
    }
    return NULL;
}

/* Sprite handler: a copy of the sprite while the player's timed flag for its slot is set, else nothing. */
static SpritePacket *MenuSprite_ShowIfFlagSet(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DD9A0 */
    u16 *flags[6] = {
        (u16 *)&player_state.timed_flags[2], (u16 *)&player_state.timed_flags[1], (u16 *)&player_state.timed_flags[3],
        (u16 *)&player_state.timed_flags[0], (u16 *)&player_state.timed_flags[4], (u16 *)&player_state.timed_flags[5],
    };

    copy = *packet;
    if (*flags[packet->slot] == 0) {
        return NULL;
    }
    return &copy;
}

/* Sprite handler: a copy of the sprite as wide as the level in the option byte for its slot (options 1 to 3,
   the first two being volumes; one pixel a step, 0x7F for another slot), showing as much of its texture. */
static SpritePacket *MenuSprite_SizeToLevel(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DDA00 */
    u8 level;

    copy = *packet;
    switch (packet->slot) {
    case 0:
        level = music_volume[0];
        break;
    case 1:
        level = sfx_volume[0];
        break;
    case 2:
        level = game_option_third_bar[0];
        break;
    default:
        level = 0x7F;
        break;
    }
    copy.xyz1.X = copy.xyz0.X + level * 16;
    copy.second_s = (float)(level * 2) / 255.0f;
    return &copy;
}

static s64 color_data_blue = 0x80000000;    /* 0x446DB0 what color_data points at for the colour 0x80FF00 (a blue) */
static s64 color_data_orange = 0x80000000;  /* 0x446DB8 and for 0xFF800000 (an orange) */

/* Sprite handler: a copy of the sprite in the colour number 45 (menu_number_45) holds, nothing when that is 0;
   a blue and an orange also choose what color_data points at, which nothing reads. */
static SpritePacket *MenuSprite_ColorFromNumber(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DDA60 */
    u32 colour = menu_number_45;
    u8 unused[16]; // match: retail's frame has 16 unused bytes below the colour bytes
    u8 rgb[3];
    s32 green;
    s32 blue;

    if (colour == 0x80FF00) {
        color_data = &color_data_blue;
    } else if (colour == 0xFF800000) {
        color_data = &color_data_orange;
    } else if (colour == 0) {
        return NULL;
    }
    copy = *packet;
    UnpackColour(rgb, &colour); // match: taking the colour's address keeps the three shifts after the copy
    copy.red = rgb[0];
    green = rgb[1]; // match: green and blue widened to int (andi 0xFF), red stored as a byte
    blue = rgb[2];
    copy.green = green;
    copy.blue = blue;
    return &copy;
}

/* Sprite handler: a copy of the sprite; the one whose slot is given is yellow, its alpha swinging between
   80 and 128. */
static SpritePacket *MenuSprite_PulseYellow(SpritePacket *packet, int slot) {
    static SpritePacket copy; /* 0x5DDAC0 */
    copy = *packet;

    if (packet->slot == slot) {
        if (pulse_sprite_falling == 0) {
            pulse_sprite_alpha += 0.8f;
            if (pulse_sprite_alpha > 128.0f) {
                pulse_sprite_alpha = 128.0f;
                pulse_sprite_falling = 1;
            }
        } else {
            pulse_sprite_alpha -= 0.8f;
            if (pulse_sprite_alpha < 80.0f) {
                pulse_sprite_alpha = 80.0f;
                pulse_sprite_falling = 0;
            }
        }
        copy.red = 0xFF;
        copy.alpha = (char)pulse_sprite_alpha;
        copy.blue = 0x78;
        copy.green = 0xFF;
    }
    return &copy;
}

/* Sprite handler: the same as MenuSprite_PulseYellow in pale blue. */
static SpritePacket *MenuSprite_PulseBlue(SpritePacket *packet, int slot) {
    static SpritePacket copy; /* 0x5DDB20 */
    copy = *packet;

    if (packet->slot == slot) {
        if (pulse_sprite_falling == 0) {
            pulse_sprite_alpha += 0.8f;
            if (pulse_sprite_alpha > 128.0f) {
                pulse_sprite_alpha = 128.0f;
                pulse_sprite_falling = 1;
            }
        } else {
            pulse_sprite_alpha -= 0.8f;
            if (pulse_sprite_alpha < 80.0f) {
                pulse_sprite_alpha = 80.0f;
                pulse_sprite_falling = 0;
            }
        }
        copy.red = 0x7A;
        copy.green = 0x99;
        copy.blue = 0xC2;
        copy.alpha = (char)pulse_sprite_alpha;
    }
    return &copy;
}

/* Sprite handler: sets ALPHA_1 to (source - destination) * source alpha + destination and gives the sprite
   back as it is. */
static SpritePacket *MenuSprite_SetBlendNormal(SpritePacket *packet) {
    GsAlphaReg reg; // match: never seeded, so the bits outside the five fields are whatever the register held

    reg.f.a_source = 0;
    reg.f.b_source = 2;
    reg.f.c_source = 0;
    reg.f.d_source = 1;
    reg.f.fix = 0;
    MenuDraw_SetGsRegister(GS_REG_ALPHA_1, *(s64 *)&reg); // match: read through a pointer; reg.raw keeps the value in another register
    return packet;
}

/* Sprite handler: a copy of the sprite, grey (0x40) when its slot is the one given. */
static SpritePacket *MenuSprite_DimSlot(SpritePacket *packet, int slot) {
    static SpritePacket copy; /* 0x5DDB80 */
    copy = *packet;
    if (packet->slot == slot) {
        copy.red = 0x40;
        copy.green = 0x40;
        copy.blue = 0x40;
    }
    return &copy;
}

/* Sprite handler: a copy of the sprite, 24 pixels wide from pixel `column`. */
static SpritePacket *MenuSprite_SetColumn(SpritePacket *packet, int column) {
    static SpritePacket copy; /* 0x5DDBE0 */
    copy = *packet;
    copy.xyz0.X = column << 4;
    copy.xyz1.X = (column << 4) + 0x180;
    return &copy;
}

/* Sprite handler: a copy of the sprite moved down `row` rows of 11 pixels. */
static SpritePacket *MenuSprite_MoveDownRows(SpritePacket *source, int row) {
    static SpritePacket copy; /* 0x5DDC40 */
    int step;

    copy = *source;
    step = (row * 11) << 4;
    copy.xyz0.Y += step;
    copy.xyz1.Y += step;
    return &copy;
}

/* Sprite handler: MenuSprite_TintWeaponStat for the numbers 45 to 53. */
static SpritePacket *MenuSprite_TintWeaponStatAlt(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DDCA0 */
    int current;
    int base;

    copy = *packet;
    current = MenuDraw_GetNumber(packet->number_id);
    switch (packet->number_id) {
    case 0x2D:
        base = (weapon_level_records[menu_number_72].gained_values[0] + weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[0]) * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x2E:
        base = (weapon_level_records[menu_number_72].gained_values[1] + weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[1]) * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x2F:
        base = (weapon_level_records[menu_number_72].gained_values[2] + weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[2]) * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x30:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[3] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x31:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[4] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x32:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[5] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x33:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[6] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x34:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[7] * weapon_level_records[menu_number_72].level / 100;
        break;
    case 0x35:
        base = weapon_defs[menu_number_72].attack_by_level[weapon_level_records[menu_number_72].level_index].values[8] * weapon_level_records[menu_number_72].level / 100;
        break;
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        copy.red = 0x40;
        copy.green = 0x40;
        copy.blue = 0xFF;
    } else if (current - base < 0) {
        copy.red = 0xFF;
        copy.green = copy.blue = 0x20;
    }
    return &copy;
}

/* Sprite handler: MenuSprite_TintWornStat for the numbers 45 to 53. */
static SpritePacket *MenuSprite_TintWornStatAlt(SpritePacket *packet, int index) {
    static SpritePacket copy; /* 0x5DDD00 */
    int current;
    int base;

    copy = *packet;
    current = MenuDraw_GetNumber(packet->number_id);
    switch (packet->number_id) {
    case 0x2D:
        base = (worn_item_level_records[index].gained_values[0] + worn_defs[index].base_values[0]) * worn_item_level_records[index].level / 100;
        break;
    case 0x2E:
        base = (worn_item_level_records[index].gained_values[1] + worn_defs[index].base_values[1]) * worn_item_level_records[index].level / 100;
        break;
    case 0x2F:
        base = (worn_item_level_records[index].gained_values[2] + worn_defs[index].base_values[2]) * worn_item_level_records[index].level / 100;
        break;
    case 0x30:
        base = worn_defs[index].base_values[3] * worn_item_level_records[index].level / 100;
        break;
    case 0x31:
        base = worn_defs[index].base_values[4] * worn_item_level_records[index].level / 100;
        break;
    case 0x32:
        base = worn_defs[index].base_values[5] * worn_item_level_records[index].level / 100;
        break;
    case 0x33:
        base = worn_defs[index].base_values[6] * worn_item_level_records[index].level / 100;
        break;
    case 0x34:
        base = worn_defs[index].base_values[7] * worn_item_level_records[index].level / 100;
        break;
    case 0x35:
        base = worn_defs[index].base_values[8] * worn_item_level_records[index].level / 100;
        break;
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        copy.red = 0x40;
        copy.green = 0x40;
        copy.blue = 0xFF;
    } else if (current - base < 0) {
        copy.red = 0xFF;
        copy.green = copy.blue = 0x20;
    }
    return &copy;
}

/* Sprite handler: a copy of the sprite, red while the player's unkA8 is below unkA4 (numbers 67 and 66). */
static SpritePacket *MenuSprite_TintShortfall(SpritePacket *packet) {
    static SpritePacket copy; /* 0x5DDD60 */
    copy = *packet;
    if (player_state.weight > player_state.max_weight) {
        copy.red = 0xFF;
        copy.green = copy.blue = 0x20;
    }
    return &copy;
}

/* Builds the menu's buttons for this frame in menu_buttons: what was pressed, the held shoulder buttons, and the
   directions as repeating steps, from the directional buttons and, in analog mode, from the left stick. */
s16 MenuPad_Update(void) {
    PadReport *report = Pad_GetReport(0, 0);
    int horizontal;
    int vertical;
    int mode;
    u16 stick;

    menu_buttons = report->pressed & ~PAD_DIRECTIONS;
    menu_buttons |= report->held & (PAD_L2 | PAD_R2 | PAD_L1 | PAD_R1);
    horizontal = MenuPad_StepHorizontal(1);
    vertical = MenuPad_StepVertical(1);
    if (horizontal > 0) {
        menu_buttons |= PAD_RIGHT;
    }
    if (horizontal < 0) {
        menu_buttons |= PAD_LEFT;
    }
    if (vertical > 0) {
        menu_buttons |= PAD_UP;
    }
    if (vertical < 0) {
        menu_buttons |= PAD_DOWN;
    }
    mode = Pad_GetMode(0, 0);
    if (mode == 1 && ((PadReportAxes *)report)->left_stick != 0) {
        stick = 0;
        if (report->left_stick_x < 0x40) {
            stick |= PAD_LEFT;
        }
        if (report->left_stick_x > 0xC0) {
            stick |= PAD_RIGHT;
        }
        if (report->left_stick_y < 0x40) {
            stick |= PAD_UP;
        }
        if (report->left_stick_y > 0xC0) {
            stick |= PAD_DOWN;
        }
        if (!(report->held & PAD_DIRECTIONS)) {
            horizontal = MenuPad_StepHorizontalFrom(1, stick);
            vertical = MenuPad_StepVerticalFrom(1, stick);
            if (horizontal > 0) {
                menu_buttons |= PAD_RIGHT;
            }
            if (horizontal < 0) {
                menu_buttons |= PAD_LEFT;
            }
            if (vertical > 0) {
                menu_buttons |= PAD_UP;
            }
            if (vertical < 0) {
                menu_buttons |= PAD_DOWN;
            }
        }
    }
    if (mode == 0xFF) {
        menu_buttons = 0;
    }
    return menu_buttons;
}

/* the directional buttons' right and left */
static s8 horizontal_repeating = 0;         /* 0x446DC0 TRUE while the held direction is repeating */
static s8 horizontal_idle_frames = 0;       /* 0x446DC1 frames since the last step, stops at 11 */
static s32 horizontal_held_frames = 0;      /* 0x446DC4 frames right (positive) or left (negative) has been held */

/* Right and left on the directional buttons as steps of a menu: 1 or -1 when the buttons are let go
   more than ten frames after the last step, and held for more than ten frames, one step every time the
   count runs from repeat_start past ten again. Nothing while the controller's mode is not known. */
static int MenuPad_StepHorizontal(int repeat_start) {
    int step = 0;

    if (Pad_GetMode(0, 0) == 0xFF) {
        horizontal_idle_frames = 0;
        horizontal_held_frames = 0;
        horizontal_repeating = 0;
        return 0;
    }
    if (horizontal_idle_frames > 10) {
        horizontal_idle_frames = 10;
    }
    horizontal_idle_frames++;
    if (!(Pad_GetReport(0, 0)->held & (PAD_LEFT | PAD_RIGHT))) {
        if (horizontal_held_frames != 0 && horizontal_repeating == 0 && horizontal_idle_frames > 10) {
            int backward = horizontal_held_frames < 1; // match: the test as a value, or the compare turns round
            int back_step = -1;         // match: -1 loaded before step is set

            step = 1;
            if (backward) {
                step = back_step;
            }
            horizontal_held_frames = 0;
            horizontal_idle_frames = 0;
        } else {
            horizontal_repeating = 0;
            horizontal_held_frames = 0;
        }
    }
    if (Pad_GetReport(0, 0)->held & PAD_RIGHT) {
        horizontal_held_frames++;
        if (horizontal_held_frames > 10) {
            step = 1;
            horizontal_held_frames = repeat_start;
            horizontal_repeating = 1;
            horizontal_idle_frames = 0;
        }
    } else if (Pad_GetReport(0, 0)->held & PAD_LEFT) {
        horizontal_held_frames--;
        if (horizontal_held_frames < -10) {
            horizontal_held_frames = -repeat_start;
            step = -1;
            horizontal_repeating = 1;
            horizontal_idle_frames = 0;
        }
    }
    return step;
}

/* the directional buttons' up and down */
static s32 vertical_idle_frames = 0;        /* 0x446DC8 */
static s8 vertical_repeating = 0;           /* 0x446DCC */
static s32 vertical_held_frames = 0;        /* 0x446DD0 up is positive */

/* Up and down on the directional buttons as steps of a menu: 1 or -1 when the buttons are let go
   more than ten frames after the last step, and held for more than ten frames, one step every time the
   count runs from repeat_start past ten again. Nothing while the controller's mode is not known. */
static int MenuPad_StepVertical(int repeat_start) {
    int step = 0;

    if (Pad_GetMode(0, 0) == 0xFF) {
        vertical_idle_frames = 0;
        vertical_held_frames = 0;
        vertical_repeating = 0;
        return 0;
    }
    if (vertical_idle_frames > 10) {
        vertical_idle_frames = 10;
    }
    vertical_idle_frames++;
    if (!(Pad_GetReport(0, 0)->held & (PAD_UP | PAD_DOWN))) {
        if (vertical_held_frames != 0 && vertical_repeating == 0 && vertical_idle_frames > 10) {
            int backward = vertical_held_frames < 1; // match: the test as a value, or the compare turns round
            int back_step = -1;         // match: -1 loaded before step is set

            step = 1;
            if (backward) {
                step = back_step;
            }
            vertical_held_frames = 0;
            vertical_idle_frames = 0;
        } else {
            vertical_repeating = 0;
            vertical_held_frames = 0;
        }
    }
    if (Pad_GetReport(0, 0)->held & PAD_UP) {
        vertical_held_frames++;
        if (vertical_held_frames > 10) {
            step = 1;
            vertical_held_frames = repeat_start;
            vertical_repeating = 1;
            vertical_idle_frames = 0;
        }
    } else if (Pad_GetReport(0, 0)->held & PAD_DOWN) {
        vertical_held_frames--;
        if (vertical_held_frames < -10) {
            vertical_held_frames = -repeat_start;
            step = -1;
            vertical_repeating = 1;
            vertical_idle_frames = 0;
        }
    }
    return step;
}

/* the left stick's right and left */
static s8 stick_horizontal_repeating = 0;   /* 0x446DD4 */
static s8 stick_horizontal_idle_frames = 0; /* 0x446DD5 */
static s32 stick_horizontal_held_frames = 0; /* 0x446DD8 */

/* Right and left in `buttons` as steps of a menu, the way MenuPad_StepHorizontal does it for the directional buttons:
   1 or -1 when both are let go more than ten frames after the last step, and held for more than ten frames,
   one step every time the count runs from repeat_start past ten again. */
static int MenuPad_StepHorizontalFrom(int repeat_start, u16 buttons) {
    int step = 0;

    if (Pad_GetMode(0, 0) == 0xFF) {
        stick_horizontal_idle_frames = 0;
        stick_horizontal_held_frames = 0;
        stick_horizontal_repeating = 0;
        return 0;
    }
    if (stick_horizontal_idle_frames > 10) {
        stick_horizontal_idle_frames = 10;
    }
    stick_horizontal_idle_frames++;
    if (!(buttons & (PAD_LEFT | PAD_RIGHT))) {
        if (stick_horizontal_held_frames != 0 && stick_horizontal_repeating == 0 && stick_horizontal_idle_frames > 10) {
            int backward = stick_horizontal_held_frames < 1; // match: the test as a value, or the compare turns round
            int back_step = -1;         // match: -1 loaded before step is set

            step = 1;
            if (backward) {
                step = back_step;
            }
            stick_horizontal_held_frames = 0;
            stick_horizontal_idle_frames = 0;
        } else {
            stick_horizontal_repeating = 0;
            stick_horizontal_held_frames = 0;
        }
    }
    if (buttons & PAD_RIGHT) {
        stick_horizontal_held_frames++;
        if (stick_horizontal_held_frames > 10) {
            step = 1;
            stick_horizontal_held_frames = repeat_start;
            stick_horizontal_repeating = 1;
            stick_horizontal_idle_frames = 0;
        }
    } else if (buttons & PAD_LEFT) {
        stick_horizontal_held_frames--;
        if (stick_horizontal_held_frames < -10) {
            stick_horizontal_held_frames = -repeat_start;
            step = -1;
            stick_horizontal_repeating = 1;
            stick_horizontal_idle_frames = 0;
        }
    }
    return step;
}

/* the left stick's up and down */
static s8 stick_vertical_repeating = 0;     /* 0x446DDC */
static s32 stick_vertical_idle_frames = 0;  /* 0x446DE0 */
static s32 stick_vertical_held_frames = 0;  /* 0x446DE4 */

/* 0x446DE8 the buttons this frame, as the menus read them (MenuPad_Update) */
u16 menu_buttons = 0;

/* 0x446DEC how many blocks the loaded menu file has (MenuFile_IndexBlocks) */
int menu_file_block_count = 0;

/* 0x446DF0 the head of the pool slots' free list */
void *pool_slot_free_list = NULL;

/* 0x446DF4 to 0x446E80 the numbers a menu sprite can stand for (MenuDraw_GetNumber) */
s32 menu_number_44 = 0;                 /* 0x446DF4 numbers 44 to 54: set by the screen that shows them */
s32 menu_number_45 = 0;                 /* 0x446DF8 */
s32 menu_number_46 = 0;                 /* 0x446DFC */
s32 menu_number_47 = 0;                 /* 0x446E00 */
s32 menu_number_48 = 0;                 /* 0x446E04 */
s32 menu_number_49 = 0;                 /* 0x446E08 */
s32 menu_number_50 = 0;                 /* 0x446E0C */
s32 menu_number_51 = 0;                 /* 0x446E10 */
s32 menu_number_52 = 0;                 /* 0x446E14 */
s32 menu_number_53 = 0;                 /* 0x446E18 */
s32 menu_number_54 = 0;                 /* 0x446E1C */
s32 menu_number_70 = 0;                 /* 0x446E20 numbers 70 to 94, likewise */
s32 menu_number_71 = 0;                 /* 0x446E24 */
s32 menu_number_72 = 0;                 /* 0x446E28 number 72; the weapon MenuSprite_TintWeaponStat compares with */
s32 menu_number_73 = 0;                 /* 0x446E2C */
s32 menu_number_74 = 0;                 /* 0x446E30 */
s32 menu_number_75 = 0;                 /* 0x446E34 */
s32 menu_number_76 = 0;                 /* 0x446E38 */
s32 menu_number_77 = 0;                 /* 0x446E3C */
s32 menu_number_78 = 0;                 /* 0x446E40 */
s32 menu_number_79 = 0;                 /* 0x446E44 */
s32 menu_number_80 = 0;                 /* 0x446E48 */
s32 menu_number_81 = 0;                 /* 0x446E4C */
s32 menu_number_82 = 0;                 /* 0x446E50 */
s32 menu_number_83 = 0;                 /* 0x446E54 */
s32 menu_number_84 = 0;                 /* 0x446E58 */
s32 menu_number_85 = 0;                 /* 0x446E5C */
s32 menu_number_86 = 0;                 /* 0x446E60 */
s32 menu_number_87 = 0;                 /* 0x446E64 */
s32 menu_number_88 = 0;                 /* 0x446E68 */
s32 menu_number_89 = 0;                 /* 0x446E6C */
s32 menu_number_90 = 0;                 /* 0x446E70 */
s32 menu_number_91 = 0;                 /* 0x446E74 */
s32 menu_number_92 = 0;                 /* 0x446E78 */
s32 menu_number_93 = 0;                 /* 0x446E7C */
s32 menu_number_94 = 0;                 /* 0x446E80 */

/* Up and down in `buttons` as steps of a menu, the way MenuPad_StepVertical does it for the directional buttons:
   1 or -1 when both are let go more than ten frames after the last step, and held for more than ten frames,
   one step every time the count runs from repeat_start past ten again. */
static int MenuPad_StepVerticalFrom(int repeat_start, u16 buttons) {
    int step = 0;

    if (Pad_GetMode(0, 0) == 0xFF) {
        stick_vertical_idle_frames = 0;
        stick_vertical_held_frames = 0;
        stick_vertical_repeating = 0;
        return 0;
    }
    if (stick_vertical_idle_frames > 10) {
        stick_vertical_idle_frames = 10;
    }
    stick_vertical_idle_frames++;
    if (!(buttons & (PAD_UP | PAD_DOWN))) {
        if (stick_vertical_held_frames != 0 && stick_vertical_repeating == 0 && stick_vertical_idle_frames > 10) {
            int backward = stick_vertical_held_frames < 1; // match: the test as a value, or the compare turns round
            int back_step = -1;         // match: -1 loaded before step is set

            step = 1;
            if (backward) {
                step = back_step;
            }
            stick_vertical_held_frames = 0;
            stick_vertical_idle_frames = 0;
        } else {
            stick_vertical_repeating = 0;
            stick_vertical_held_frames = 0;
        }
    }
    if (buttons & PAD_UP) {
        stick_vertical_held_frames++;
        if (stick_vertical_held_frames > 10) {
            step = 1;
            stick_vertical_held_frames = repeat_start;
            stick_vertical_repeating = 1;
            stick_vertical_idle_frames = 0;
        }
    } else if (buttons & PAD_DOWN) {
        stick_vertical_held_frames--;
        if (stick_vertical_held_frames < -10) {
            stick_vertical_held_frames = -repeat_start;
            step = -1;
            stick_vertical_repeating = 1;
            stick_vertical_idle_frames = 0;
        }
    }
    return step;
}

/* Writes one GS register: sends the value and the register's address as a packet over DMA channel 2 and
   waits for it; returns what sceGsSyncPath says. */
s32 MenuDraw_SetGsRegister(int reg_address, s64 reg_value) {
    static int packet[32]; /* 0x5DDDC0 0x80 bytes, of which 0x20 are used */
    int *cursor;

    cursor = packet;
    *cursor++ = 0x8001;
    *cursor++ = 0x10004000;
    *cursor++ = 0xE;
    *cursor++ = 0;
    *(s64 *)cursor = reg_value;
    cursor += 2;
    *cursor++ = reg_address;
    *cursor = 0;
    FlushCache(0);
    cursor++;
    while (*D2_CHCR & D_CHCR_STR) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = (u32)packet & 0x7FFFFFFF;
    *D2_TADR = 0;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN;
    /* match: retail tail-calls sceGsSyncPath and returns its result; a written return defeats the tail call */
    sceGsSyncPath(0, 0);
}

/* A copy of the sprite with the alpha passed in. */
static SpritePacket *MenuSprite_SetAlpha(SpritePacket *packet, int alpha) {
    static SpritePacket copy; /* 0x5DDE40 */
    copy = *packet;
    copy.alpha = alpha;
    return &copy;
}

/* Draws a sprite like MenuSprite_DrawPlain, with handler entry_id % 1000; from 1000 on the sprite is drawn
   with `variant` as its alpha. */
void MenuSprite_Draw(int owner, int entry_id, int param, int variant) {
    static SpritePacket unused_copy; /* 0x5DDEA0 0x60 bytes that nothing uses; which function had them is a guess */
    SpriteHandler handler = sprite_handlers[entry_id % 1000];
    KindObject *object = NULL;

    if (handler != NULL) {
        object = handler(owner, param);
    }
    if (object != NULL) {
        if (entry_id >= 1000) {
            object = (KindObject *)MenuSprite_SetAlpha((SpritePacket *)object, variant);
        }
        switch (object->kind) {
        case 0:
            MenuDraw_Sprite(object);
            break;
        case 1:
            MenuDraw_Word(object);
            break;
        case 2:
            MenuDraw_Number(object);
            break;
        }
    }
}
