/* The menu screens: the game's menu (Use Item, Cast a Spell, Equip, Status Viewer, Recollection, Visible Item,
   System Config; the names are the game's own, from a table nothing reads) with the memory card, options, button
   and brightness screens, which Menu_Run also runs on their own. The file starts with the rings of items the lists
   walk and with the loader of the models the screens show: load nodes chained at D_00446C90, each loading one
   file in stages. Every screen is a handler in D_00446B20 that is called once a frame. */
#include "kf_menu_screens.h"
#include "kf_menu.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_effect.h"
#include "kf_link.h"
#include "kf_load.h"
#include "kf_file.h"
#include "kf_game.h"
#include "common.h"
#include "kf_texture.h"
#include "kf_packet.h"
#include "kf_pad.h"
#include <eeregs.h>
#include "kf_gs.h"
#include "kf_message.h"
#include "kf_state.h"
#include "kf_screen.h"
#include "kf_save_card.h"
#include "kf_item.h"

extern LevelRecord weapon_level_records[70];
extern LevelRecord worn_item_level_records[160];
extern u8 spell_levels[6][5];
extern u8 item_counts[380];
extern FxTableEntry fx_table[];
extern PlayerState player_state;
extern LevelDef item_name_defs[380];
extern ItemCommonDef item_common_defs[];
extern MenuItem **D_00420D90[11];
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
extern MenuItem D_00424050[];
extern MenuItem D_00424600[];
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C38;
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern s32 D_00446C48;
extern s32 D_004472E0;
extern MenuItemList D_005CE6A0;
extern MenuItemList D_005CE6B0;
extern MenuItemList D_005CE6C0;
extern MenuItemList D_005CE6D0;
extern MenuItemList D_005CE6E0;
extern MenuItemList D_005CE6F0;
extern MenuItemList D_005CE700;
extern MenuItemList D_005CE710;
extern MenuItemList D_005CE720;
extern MenuItemList D_005CE730;
extern MenuItemList D_005CE740;
extern MenuItemList D_005CE760;
extern MenuItemList D_005CE770;
extern MenuItemList D_005CE780;
extern MenuItemList D_005CE790;
extern MenuItemList D_005CE7A0;
extern MenuItemList D_005CE7B0;
extern MenuItemList D_005CE7C0;
extern MenuItemList D_005CE7D0;
extern MenuItemList D_005CE7E0;
extern MenuItemList D_005CE7F0;
extern MenuItemList D_005CE800;
extern MenuItemList D_005CE810;
extern MenuItem *D_005CE830[4];
extern MenuItemList *D_005CE840[];
extern MenuItem *D_005CE860[];
extern MenuItem *D_005CE850[];
extern MenuItem *D_005CE870[];
extern MenuItem *D_005CE880[];
extern MenuItem *D_005CE890[];
extern MenuItem *D_005CE8A0[];
// match: D_005CE860[0] reached from D_005CE840 (the same address). gcse numbers its hoisted address registers by a
// hash of the symbol's name, and with the name D_005CE860 two of them ($19, $20) come out the other way round.
#define D_005CE860_FIRST (((MenuItem **)D_005CE840)[8])
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
            (records)[(record_base) + i].unk18 == 0) {                                                   \
            LINK_ITEM(&(nodes)[i])                                                                       \
        }                                                                                                \
    }                                                                                                    \
    first->next = last;                                                                                  \
    last->prev = first;                                                                                  \
    (list).item = (start_list).item = &(nodes)[(worn) - (record_base)];
/* Whether the spell is the one its slot of spell_levels holds (three spells to a slot, numbered from 1). */
#define SPELL_IS_SET(spell) (spell_levels[((spell) - 1) / 15][(((spell) - 1) / 3) % 5] == ((spell) - 1) % 3 + 1)
extern LinkNode *D_00446C90;
extern LinkNode D_005CE8B0;
extern LinkNode D_00426470;
extern LinkNode D_004264C0;
extern LinkNode D_00426510;
extern LinkNode D_00426560;
extern LinkNode D_004265B0;
extern int D_00446AD0;
int func_00102238(char *, int, int);
int func_00102258(char *name, int nowait, int pack);
int func_001025C0(int, void *, int);
int func_001024B0(int file);
int func_00102790(int file, int mode);
int FlushCache(int);
void PoolPartFile_Relocate(char *);
extern int D_004472F0;
extern PoolPartSource *D_00420DF0[13];
void PoolSlot_LoadGroup(PoolPartSource *, u32);
extern ChunkPoolNode *D_004472B0;
extern ChunkPoolNode D_005CE900[];
int sprintf(char *, char *, ...);
extern char D_00426498[];
extern char D_004264E8[];
extern char D_00426538[];
extern char D_00426588[];
extern char D_004265D8[];
extern char D_005CE9F8[];
extern char D_005CEA98[];
extern char D_005CEAE8[];
extern char D_005CEB38[];
extern char D_005CEB88[];
extern char D_005CEBD8[];
extern char D_005CEC28[];
extern char D_005CEC78[];
extern char D_005CECC8[];
extern LinkNode D_005CED40;
extern LinkNode D_005CE9D0;
extern LinkNode D_005CEA70;
extern LinkNode D_005CEAC0;
extern LinkNode D_005CEB10;
extern LinkNode D_005CEB60;
extern LinkNode D_005CEBB0;
extern LinkNode D_005CEC00;
extern LinkNode D_005CEC50;
extern LinkNode D_005CECA0;
extern LinkNode D_005CEF30[12];
extern s32 D_00446F58;
extern ChunkUpload D_005CEEE0;
int MenuArena_Init(int); // match: this file only matches with an int result
void *MenuArena_Alloc(s32, s32);
void *MenuHeap_Alloc(int);
int MenuFile_Load(char *, void *);
extern u32 D_00446A30;
extern u32 D_00446A34;
extern u32 D_00446A38;
extern u32 D_00446A3C;
extern u32 D_00446A40;
extern u32 D_00446A44;
extern u32 D_00446A48;
extern u32 D_00446A4C;
extern u32 D_00446A50;
extern u32 D_00446A54;
extern u32 D_00446A58;
extern u32 D_00446A5C;
extern u32 D_00446A60;
extern u32 D_00446A64;
extern u32 D_00446A68;
extern u32 D_00446A6C;
extern u32 D_00446A70;
extern u32 D_00446A74;
extern u32 D_00446A78;
extern u32 D_00446A7C;
extern u32 D_00446A80;
extern u32 D_00446A84;
extern u32 D_00446A88;
extern u32 D_00446A8C;
extern u32 D_00446A94;
extern u32 D_00446A98;
extern u32 D_00446A9C;
extern u32 D_00446AA0;
extern u32 D_00446AA4;
extern u32 D_00446AA8;
extern u32 D_00446AAC;
extern u32 D_00446AB0;
extern u32 D_00446AB4;
extern u32 D_00446AB8;
extern u32 D_00446ABC;
extern u32 D_00446AC0;
extern SpritePacket *D_00446BF4;
extern SpritePacket *D_00446BF8;
extern SpritePacket *D_00446BFC;
extern SpritePacket *D_00446C00;
extern SpritePacket *D_00446C0C;
extern SpritePacket *D_00446C10;
extern SpritePacket *D_00446C14;
extern SpritePacket *D_00446C4C;
extern SpritePacket *D_00446C50;
extern SpritePacket *D_00446C54;
extern SpritePacket *D_00446C58;
extern SpritePacket *D_00446C5C;
extern SpritePacket *D_00446C60;
extern SpritePacket *D_00446C64;
extern SpritePacket *D_00446C68;
extern SpritePacket *D_00446C6C;
extern SpritePacket *D_0044722C;
extern SpritePacket *D_00447230;
extern SpritePacket *D_00447234;
extern SpritePacket *D_00447238;
extern SpritePacket *D_0044723C;
extern SpritePacket *D_00447240;
extern SpritePacket *D_00447244;
extern SpritePacket *D_00447248;
extern SpritePacket *D_0044724C;
extern SpritePacket *D_00447250;
extern SpritePacket *D_00447254;
extern SpritePacket *D_00447258;
extern SpritePacket *D_0044725C;
extern SpritePacket *D_00447260;
extern SpritePacket *D_00447264;
extern SpritePacket *D_00447268;
extern SpritePacket *D_0044726C;
extern SpritePacket *D_00447270;
extern SpritePacket *D_00447274;
extern SpritePacket *D_00447278;
extern PlayerItemDef weapon_defs[];
extern LevelRecordDef worn_defs[];
extern GsDoubleBufferDc D_00426140;
extern LoadedTexture D_0042A8D0;
extern MenuModelMotion *D_005CEE60[];
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
int func_001CEBE0(void);
s16 MenuPad_Update(void);
extern GsDrawEnv D_00432D80;
extern s32 D_00446AC4;
extern s8 D_00446C94;
extern u16 D_00446DE8;
extern int D_00446F78;
extern int D_00446F7C;
extern s32 D_00447200;
extern s32 D_00447204;
extern int D_004472D0;
extern int D_004472D4;
extern MenuScreenState D_005CF2F0;
extern ColorBytes D_00446C78;
extern f32 D_0042B7D0[16];
void MenuModel_DrawTinted(ChunkUpload *upload, s32 draw_param, RenderSlotArg slot_arg);
// match: called with the node, a matrix and the colour, not as the definition has it (the colour is loaded unaligned)
#define DRAW_NODE_MODEL(node, matrix, colour) ((void (*)(LinkNode *, f32 *, ColorBytes))MenuModel_DrawTinted)(node, matrix, colour)
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
extern u64 D_004261A0[47];             /* the frame register of the first buffer ([0]) and of the second ([46]) */
extern u8 game_options[0x10];  /* the options block */
extern char D_00426220[];
extern char D_00426310[];           /* inside D_00426140: the second buffer's draw environments */
extern char D_00426390[];
extern s32 D_00446C98;
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

extern ColorBytes D_00446B18[]; // match: the initialiser of the local below ({ 0x10, 0x10, 0x10 }); an array so it is not reached through $gp
int Menu_SetBlendMode(u8);
void MenuSprite_DrawPlain(int owner, int index, int param);
extern s32 D_00446AC8;
extern char D_005CE0E0[];
extern u8 D_0034968A[];
extern LoadedBlockRef D_0042A2A0[];
extern void *D_00446B20;
extern float D_00446C04;
extern char D_00446C08;
extern s8 D_00446C09;
extern s8 D_00446C70;
extern ColorBytes D_00446C80;
extern ColorBytes D_00446C88;
extern s32 D_00446DEC;
extern s32 D_00446F38;
extern u8 D_004472CF;
void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_00109E88(void);
void func_0010B238(TextureTableHeader *);
void MenuTexture_Upload(TextureHeader *, LoadedBlockRef *);
extern s8 D_004472CD;
void Sound_LoadMgBank(int);
void *memset(void *, int, u32);
extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern char D_004472CE;
extern s8 D_004472DA;
extern s8 D_004472C4;               /* 0: fading in, 1: open, 2: fading out to the chosen screen, 3: fading out to leave */
extern s8 D_004472D8;
s32 Sound_PlayMenu(s32);
int Menu_OwnsAnyItem(void);
int Menu_KnowsCastableSpell(void);
void MenuSprite_Draw(int, int, int, int);
extern u8 D_00446B39;
extern GameStatus game_status;
extern f32 D_00421000[4];
extern f32 D_00446B3C;              /* the shown item model's turn about y */
extern s32 D_00446B40;
extern s32 D_00446DF4;
extern s8 D_004472C5;               /* 0: fading in, 1: open, 2 to 4: waiting for the models, 5 to 7: fading out */
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
extern f32 D_00421010[4];
extern s32 D_00446DF8;
extern s8 D_004472C8;               /* 0: fading in, 1: open, 2: waiting for the models, 3: fading out */
int MenuText_SetGlyphSize(GsPackedWord);
s32 MenuText_DrawLine(u16 *text, s32 limit);
extern u8 player_weapon[];
extern MenuItem **D_00420D60[11];
extern LinkNode *D_00420DC8[];
extern f32 D_00421020[4];
extern f32 D_00421040[4];
extern void (*D_0042A290[])(PoolSlot *, ColorBytes);
extern PoolSlot D_0042A8F0[32];
extern s32 D_00446ACC;
extern u8 D_00446AD4; // match: a byte here; kf_trade.c reads and writes it as a word
extern u8 D_00446AD5;
extern s8 D_00446B50;
extern f32 D_00446B54;
extern f32 D_00446B58;
extern u32 D_00446B5C;
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
extern s8 D_004472C6;
extern s8 D_004472C7;
// match: D_004472C7 is the second byte of what the symbol list has as one two-byte object. It has to be a variable of
// its own: a store to `D_004472C6[1]` is never put in a delay slot, and retail has it there.
__asm__("D_004472C7 = D_004472C6 + 1");
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
#define EQUIP_LIST (((MenuItem **)D_005CE840)[0])
#define CURRENT_ITEM (((MenuItemList *)EQUIP_LIST)->item)
// match: called as a void function, so the result register is free at once
#define SET_BLEND(mode) ((void (*)(u8))Menu_SetBlendMode)(mode)
#define DRAW_ITEM_MODEL(matrix) DRAW_NODE_MODEL(&D_00426470, matrix, D_00446C88)
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
        MenuSprite_Draw((int)&parts[i], parts[i].unk3E + 1000, variant, (int)D_00425910[i]); \
        SET_BLEND(0);                                       \
    }
extern s8 D_004472C9;               /* 0: fading in, 1: open, 2: fading out to Menu_UpdateKeyItems, 3: fading out to leave */
extern s32 D_00421050[12];
extern s32 D_00446BB8;              /* the row of three the list starts at, 0 to 2 */
extern s32 D_00446BBC;
#define LIST_PAGES 50
#define LIST_ROWS 7
extern u8 recollection_flags[LIST_PAGES][8];
extern u8 recollection_pages[50];
extern f32 D_00421080[4];
extern s32 D_00446BC0;              /* the page shown, 0 to 49 */
extern s32 D_00446BC4;              /* the page shown the frame before */
extern s8 D_004472CA;               /* 0: fading in, 1: open, 2: fading out */
extern s8 D_004472D9;               /* the counter at the top of the list */
int sceCdBreak(void);
s32 MenuText_DrawWrappedLong(u16 *, s32);
s32 MenuText_DrawLineShortened(u16 *, s32);
/* The offset of text `number` in the text table, 0 when it has none. */
static inline s32 GetListTextOffset(s32 table, s32 number) {
    return ((s32 *)table)[number];
}
extern f32 D_00421090[4];
/* The offset of text `number` in the text table, 0 when it has none. */
static inline s32 GetPageTextOffset(s32 table, s32 number) {
    return ((s32 *)table)[number];
}
extern f32 D_004210A0[4];
extern f32 D_00446BD0;              /* the turning model's angle */
extern s32 D_00446E2C;
extern MenuItem *D_00447210;
extern MenuItem *D_00447214;
extern MenuItem *D_00447218;
extern MenuItem *D_0044721C;
extern MenuItem *D_00447220;
extern s8 D_004472CB;               /* 0: fading in, 1: open, 2: waiting for the loads, 3: fading out */
extern s8 D_004472CC;               /* the detail panel's: 0: fading in, 1: open, 2: fading out */
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
        Menu_SetBlendMode((packets)[i].unk42);                                                              \
        MenuSprite_Draw((int)&(packets)[i], (packets)[i].unk3E + 1000, (param), (int)D_00425910[i]);      \
        ((void (*)(u8))Menu_SetBlendMode)(0);                                                               \
    }
extern s32 D_004472E8;
extern s32 D_00446BD8;
extern u16 D_005CF318[13];
extern u16 D_0044692C;
extern u16 D_0044692E;
extern u16 D_00446930;
extern u16 D_00446932;
extern u16 D_00446934;
extern u16 D_00446936;
extern u16 D_00446938;
extern u16 D_0044693A;
extern u16 D_0044693C;
extern u16 D_0044693E;
extern u16 D_00446940;
extern s16 D_00446942;
extern s16 D_00446944;
// match: both are called as void functions, so the result register is free at once
#define PLAY_MENU_SOUND(sound) ((void (*)(s32))Sound_PlayMenu)(sound)
#define SET_PART_MODE(mode) ((void (*)(u8))Menu_SetBlendMode)(mode)
s32 SaveCard_Open(void);
s32 SaveCard_StartCheck(s8);
#define PAGE_ROWS 7
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 ScrollItemListUp(void) {
    D_00426120[0] = D_00426120[0]->prev;
    D_00426130[0] = D_00426130[0]->prev;
    D_00426110[0] = D_00426120[0];
    return D_00426120[0] != D_00446C44;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 ScrollItemListDown(void) {
    D_00426120[0] = D_00426120[0]->next;
    D_00426130[0] = D_00426130[0]->next;
    D_00426110[0] = D_00426120[0];
    return D_00426120[0] != D_00446C40;
}
/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 ScrollAllItemsUp(void) {
    D_005CE860[0] = D_005CE860[0]->prev;
    D_005CE870[0] = D_005CE870[0]->prev;
    D_005CE850[0] = D_005CE860[0];
    return D_005CE860[0] != D_00446C28;
}
/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 ScrollAllItemsDown(void) {
    D_005CE860[0] = D_005CE860[0]->next;
    D_005CE870[0] = D_005CE870[0]->next;
    D_005CE850[0] = D_005CE860[0];
    return D_005CE860[0] != D_00446C24;
}
extern s8 D_00446BDC;
extern u8 D_00447224;
extern void *D_00446B24;
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
extern s8 D_00446BDD;
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
extern s32 D_00446BE0;              /* option 0 as it was before it was first toggled */
extern s8 D_00446BE4;               /* 0: option 0 untouched, 1: toggled and not applied yet, 2 to 4: applied */
extern s8 D_00446BE5;               /* counts the two steps of leaving when the screen is a start-up step */
void Sound_SetBgmVolume(s32);
void Sound_SyncBgmLevel(void);
void func_001CBED8(void);
void func_001CC018(void);
extern s8 D_004210B0[12];
extern s8 D_004210C0[12];
extern s8 D_00446BE6;
extern s32 D_00446BE8;
extern s32 D_00446BEC;
extern s8 D_00446BF0;
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
#define SET_CHOICE(n) D_004210C0[i] = D_004210B0[i] = (n)
#define SET_SHOWN(n) D_004210C0[i] = (n)
s32 SaveCard_StartRescan(void);
s32 SaveCard_IsCheckDone(void);
s8 SaveCard_IsFormatted(s32);
int SaveCard_GetCardProblem(int);
s32 SaveCard_GetSavesState(void);
extern f32 D_00447228;

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

    D_00426120[0] = NULL;
    D_00426130[0] = NULL;
    D_00446C40 = NULL;
    D_00446C44 = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 150; i++) {
        if (item_counts[i] != 0 && (item_name_defs[i].state & 1)) {
            LINK_COUNTED_ITEM(&D_004210D0[i], D_00426130)
        }
    }
    D_00446C48 = count;
    if (first != NULL) {
        last->prev = first;
        D_00426110[0] = D_00426120[0] = D_005CE760.item = first;
        first->next = last;
        if (D_00426130[0] == NULL) {
            D_00426130[0] = last->next;
        } else {
            D_00426130[0] = D_00426130[0]->next;
        }
        D_00446C40 = first;
        D_00446C44 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C44 = D_00446C44->next;
        }
    } else {
        D_00426110[0] = NULL;
    }

    D_005CE860_FIRST = NULL;
    D_005CE870[0] = NULL;
    D_00446C24 = NULL;
    D_00446C28 = NULL;
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 380; i++) {
        if (item_counts[i] != 0 && !(item_name_defs[i].state & 2)) {
            if ((i >= 0 && i < 150) || (i >= 150 && i < 220 && weapon_level_records[i - 150].unk18 == 0) ||
                (i >= 220 && i < 250 && worn_item_level_records[i - 220].unk18 == 0) ||
                (i >= 250 && i < 275 && worn_item_level_records[i - 220].unk18 == 0) ||
                (i >= 275 && i < 300 && worn_item_level_records[i - 220].unk18 == 0) ||
                (i >= 300 && i < 325 && worn_item_level_records[i - 220].unk18 == 0) ||
                (i >= 325 && i < 350 && worn_item_level_records[i - 220].unk18 == 0) ||
                (i >= 350 && i < 360 && worn_item_level_records[i - 220].unk18 == 0) ||
                (i >= 360 && i < 370 && worn_item_level_records[i - 220].unk18 == 0) ||
                (i >= 370 && worn_item_level_records[i - 220].unk18 == 0)) {
                count++;
                if (count == 7) {
                    D_005CE870[0] = &D_00421A30[i];
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
    D_00446C38 = count;
    if (first != NULL) {
        last->prev = first;
        D_005CE850[0] = D_005CE860_FIRST = first;
        first->next = last;
        if (D_005CE870[0] == NULL) {
            D_005CE870[0] = last->next;
        } else {
            D_005CE870[0] = D_005CE870[0]->next;
        }
        D_00446C24 = first;
        D_00446C28 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C28 = D_00446C28->next;
        }
    } else {
        D_005CE850[0] = NULL;
    }

    last = NULL;
    first = NULL;
    for (i = 0; i < 150; i++) {
        if (item_counts[i] != 0 && (item_name_defs[i].state & 1) && !((u8)item_common_defs[i].flags & 1) && !(i >= 145 && i < 148)) {
            LINK_ITEM(&D_00424600[i])
        }
    }
    for (i = 150, spell = 0; i < 241; i++, spell++) {
        if (SPELL_IS_SET(spell) && fx_table[spell].unk102 == 1) {
            LINK_ITEM(&D_00424600[i])
        }
    }
    // match: closed without the `first != NULL` test of the counted rings (so are the rings below); an empty one
    // would be written through NULL
    first->next = last;
    last->prev = first;
    switch (player_state.shortcut_type) {
    case 0:
        D_005CE810.item = D_005CE6A0.item = D_00424600;
        // match: no break, case 0 goes on into case 1, which sets both again
    case 1:
        if (item_counts[player_state.shortcut] != 0) {
            D_005CE810.item = D_005CE6A0.item = &D_00424600[player_state.shortcut];
        } else {
            player_state.shortcut = 0;
            D_005CE810.item = D_005CE6A0.item = D_00424600;
        }
        break;
    case 2:
        D_005CE810.item = D_005CE6A0.item = &D_00424600[player_state.shortcut + 150];
        break;
    }

    last = NULL;
    first = NULL;
    for (i = 0; i < 70; i++) {
        if (item_counts[150 + i] != 0 && (item_name_defs[150 + i].state & 1) && weapon_level_records[i].unk18 == 0) {
            LINK_ITEM(&D_004231F0[i])
        }
    }
    first->next = last;
    last->prev = first;
    D_005CE770.item = D_005CE6B0.item = &D_004231F0[player_state.weapon];

    LINK_GROUP(D_00423650, worn_item_level_records, 0, 220, 30, D_005CE780, D_005CE6C0, player_state.equipment[EQUIP_SHIELD])
    LINK_GROUP(D_00423830, worn_item_level_records, 30, 250, 25, D_005CE790, D_005CE6D0, player_state.equipment[EQUIP_HELM])
    LINK_GROUP(D_004239C0, worn_item_level_records, 55, 275, 25, D_005CE7A0, D_005CE6E0, player_state.equipment[EQUIP_ARMOR])
    LINK_GROUP(D_00423B50, worn_item_level_records, 80, 300, 25, D_005CE7B0, D_005CE6F0, player_state.equipment[EQUIP_GAUNTLET])
    LINK_GROUP(D_00423CE0, worn_item_level_records, 105, 325, 25, D_005CE7C0, D_005CE700, player_state.equipment[EQUIP_BOOT])
    LINK_GROUP(D_00423E70, worn_item_level_records, 130, 350, 10, D_005CE7D0, D_005CE710, player_state.equipment[EQUIP_RING])
    LINK_GROUP(D_00423F10, worn_item_level_records, 140, 360, 10, D_005CE7E0, D_005CE720, player_state.equipment[EQUIP_BRACELET])
    LINK_GROUP(D_00423FB0, worn_item_level_records, 150, 370, 10, D_005CE7F0, D_005CE730, player_state.equipment[EQUIP_AMULET])

    last = NULL;
    first = NULL;
    for (i = 0; i < 91; i++) {
        if (i == 0 || (SPELL_IS_SET(i) && fx_table[i].unk102 == 0)) {
            LINK_ITEM(&D_00424050[i])
        }
    }
    first->next = last;
    last->prev = first;
    D_005CE890[0] = NULL;
    D_005CE800.item = D_005CE740.item = &D_00424050[player_state.equipped_magic];
    D_005CE8A0[0] = NULL;

    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 91; i++) {
        if (SPELL_IS_SET(i) && fx_table[i].unk102 == 1) {
            LINK_COUNTED_ITEM(&D_00424050[i], D_005CE8A0)
        }
    }
    if (first != NULL) {
        first->next = last;
        D_005CE830[0] = first;
        D_005CE890[0] = D_005CE880[0] = first;
        last->prev = first;
        if (D_005CE8A0[0] == NULL) {
            D_005CE8A0[0] = last->next;
        } else {
            D_005CE8A0[0] = D_005CE8A0[0]->next;
        }
    } else {
        D_005CE880[0] = NULL;
    }
    D_005CE840[0] = (MenuItemList *)D_00420D90[D_004472E0];
}

/* Runs the callback of the load node at the head of the chain, or goes on to the next node when it has none or is done. */
void Menu_StepModelLoads(int unused) {
    LinkNode *node = D_00446C90;

    if (node != NULL) {
        if (node->unk20 == 0 && node->callback != NULL) {
            ((void (*)(LinkNode *))node->callback)(node); // match: the callback gets its node
        } else {
            D_00446C90 = node->next;
        }
    }
}

/* Moves the model load in progress into the spare node D_005CE8B0, so that its own node can be reused: the
   node is reset with a fresh buffer and the chain is cut after the spare. */
static void Menu_MoveLoadToSpare(void) {
    LinkNode *node = D_00446C90;

    if (node != NULL && node->callback == (void (*)(void))Menu_StepModelLoad && node != &D_005CE8B0) {
        D_005CE8B0 = *node;
        Menu_TakeLoadBuffer(D_00446C90);
        D_00446C90->unk08 = -1;
        D_00446C90->unk20 = 0;
        D_00446C90->unk1C = 0;
        D_00446C90 = &D_005CE8B0;
        if (D_005CE8B0.next != NULL && D_005CE8B0.next->callback == (void (*)(void))Menu_StepModelLoad) {
            D_005CE8B0.next = NULL;
        }
        Menu_FreeLoadBuffer((MenuFreeRequest *)&D_005CE8B0);
        node = NULL; // match: a statement after the call keeps it from becoming a sibling jump
    }
}

/* Copies the model load in progress into the spare node D_005CE8B0, which then loads for the node it was
   copied from, and cuts the chain after it. */
static void Menu_CopyLoadToSpare(void) {
    LinkNode *node = D_00446C90;

    if (node != NULL) {
        if (node->callback == (void (*)(void))Menu_StepModelLoad) {
            if (node != &D_005CE8B0) {
                D_005CE8B0 = *node;
                node->unk20 = 0;
                node->unk1C = 0;
                D_005CE8B0.unk10 = node;
                D_00446C90 = &D_005CE8B0;
                if (D_005CE8B0.next != NULL && D_005CE8B0.next->callback == (void (*)(void))Menu_StepModelLoad) {
                    D_005CE8B0.next = NULL;
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

    if (D_00446C90 == &D_004264C0 || D_00446C90 == &D_00426470) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_00426510);
    D_00426510 = D_00426470;
    D_00426510.next = NULL;
    if (D_00426510.unk20 == 0) {
        pending = &D_00426510;
    }
    D_00426470 = D_004264C0;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_004264C0);
    if (pending == NULL) {
        pending = &D_004264C0;
    }
    D_004264C0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_004264C0.unk08 = -1;
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_004264C0.next = NULL;
    current = D_00446C90;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_00426510;
    D_00426510.next = &D_004264C0;
    D_004264C0.next = &D_00426560;
    D_00426560.next = &D_004265B0;
    D_004265B0.next = NULL;
    Menu_SetEquipModelNames();
}

/* The equipment screen went back to the list before: the model shown moves to node 1, node 2's becomes the
   one shown, nodes 2, 3 and 4 are made fresh, and the five are chained and named again. */
static void Menu_ShiftEquipModelsPrevList(int arg, void *unused_data) {
    LinkNode *pending = NULL;
    LinkNode *current;

    if (D_00446C90 == &D_00426510 || D_00446C90 == &D_00426470) {
        ((void (*)(int))Menu_CopyLoadToSpare)(arg); // match: defined (void); passing the handler's argument through fixes the saved registers
    } else {
        ((void (*)(int))Menu_MoveLoadToSpare)(arg); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_004264C0);
    D_004264C0 = D_00426470;
    D_004264C0.next = NULL;
    if (D_004264C0.unk20 == 0) {
        pending = &D_004264C0;
    }
    D_00426470 = D_00426510;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_00426510);
    if (pending == NULL) {
        pending = &D_00426510;
    }
    D_00426510.unk20 = 0;
    D_00426510.unk1C = 0;
    D_00426510.unk08 = -1;
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426510.next = NULL;
    current = D_00446C90;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = &D_00426560;
    D_00426560.next = &D_004265B0;
    D_004265B0.next = NULL;
    Menu_SetEquipModelNames();
}

/* The equipment screen went back one item in its list: the model shown moves to node 3, node 4's becomes the
   one shown, node 4 is made fresh, and the five are chained and named again. */
static void Menu_ShiftEquipModelsPrevItem(int unused, void *unused_data) {
    LinkNode *pending = NULL;
    LinkNode *current;

    if (D_00446C90 != &D_00426560) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_00426560);
    D_00426560 = D_00426470;
    D_00426560.next = NULL;
    if (D_00426560.unk20 == 0) {
        pending = &D_00426560;
    }
    D_00426470 = D_004265B0;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_004265B0);
    if (pending == NULL) {
        pending = &D_004265B0;
    }
    D_004264C0.next = NULL;
    D_00426510.next = NULL;
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.next = NULL;
    D_004265B0.unk08 = -1;
    current = D_00446C90;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_00426560;
    D_00426560.next = &D_004265B0;
    D_004265B0.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
    Menu_SetEquipModelNames();
}

/* The equipment screen went on one item in its list: the model shown moves to node 4, node 3's becomes the one
   shown, node 3 is made fresh, and the five are chained and named again. */
static void Menu_ShiftEquipModelsNextItem(int unused, void *unused_data) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (D_00446C90 != &D_004265B0) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_004265B0);
    D_004265B0 = D_00426470;
    node = &D_004265B0;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &D_00426560; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    D_00426470 = D_00426560;
    done = D_00426470.unk20;
    D_00426470.next = NULL;
    if (done == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_00426560);
    if (pending == NULL) {
        pending = &D_00426560;
    }
    D_004264C0.next = NULL;
    D_00426510.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.next = NULL;
    D_00426560.unk08 = -1;
    current = D_00446C90;
    if (current != NULL) {
        if (current->next != NULL) {
            current->next->next = pending;
        } else {
            current->next = pending;
        }
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004265B0;
    D_004265B0.next = &D_00426560;
    D_00426560.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
    Menu_SetEquipModelNames();
}

/* Stage 0 of a menu model's load: opens the node's file (a stand-in model when there is none) and starts reading it. */
static void Menu_OpenModelFile(ChunkUpload *upload) {
    s32 file_handle;

    file_handle = func_00102258(upload->name, 1, 1);
    upload->unk08 = file_handle;
    if (file_handle < 0) {
        upload->unk08 = func_00102238("menu/select2/i0000.mod", 1, 1);
    }
    func_001025C0(upload->unk08, upload->pool_node->buffer, 0x25800);
    D_00446AD0 = upload->unk08;
    upload->stage++;
}

/* Stage 1 of a menu model's load: when the read has finished closes the file and goes on; on error starts over. */
static void Menu_WaitModelRead(ChunkUpload *upload) {
    s32 status;

    status = func_00102790(upload->unk08, 1);
    if (status == 0) {
        func_001024B0(upload->unk08);
        D_00446AD0 = -1;
        upload->stage++;
    } else if (status < 0) {
        func_001024B0(upload->unk08);
        upload->done = upload->stage = 0;
        D_00446AD0 = -1;
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

    file_handle = func_00102258(node->unk10->name, 1, 1);
    node->unk08 = file_handle;
    if (file_handle < 0) {
        LinkNode *next;

        node->unk08 = -1;
        node->unk1C = 0;
        next = node->unk10->next;
        D_004472F0 = 0;
        if (next != NULL) {
            node->unk10 = next;
            return;
        }
        node->unk20 = 1;
        return;
    }
    func_001025C0(file_handle, ((ChunkPoolNode *)node->pool_node)->buffer, 0x25800);
    D_00446AD0 = node->unk08;
    node->unk1C += 1;
}

/* Stage 1 of an equipment model's load: as Stage 1 of a menu model's, and an error also frees the screen's input. */
static void Menu_WaitWornModelRead(ChunkUpload *upload) {
    s32 status;

    status = func_00102790(upload->unk08, 1);
    if (status == 0) {
        func_001024B0(upload->unk08);
        D_00446AD0 = -1;
        upload->stage++;
    } else if (status < 0) {
        func_001024B0(upload->unk08);
        upload->done = upload->stage = 0;
        D_00446AD0 = -1;
        D_004472F0 = 0;
    }
}

/* PoolSlot_LoadGroup's second argument for each slot of D_00420DF0 */
static const LinkSlotValues slot_part_values = { { 4, 0xFF, 5, 2, 1, 3, 0, 6, 7, 8, 0xFF, 0xFF } };

/* Last stage of a node's load: points its slot at the two parts of the file, gives the buffer to
   the node it was loaded for and goes on to the next waiting node. */
static void Menu_FinishWornModelLoad(LinkNode *node) {
    LinkSlotValues values = slot_part_values;
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
    D_00420DF0[target->slot]->texture = (char *)file + file->second_offset;
    D_00420DF0[target->slot]->file = (PoolPartFile *)((char *)file + file->first_offset);
    PoolPartFile_Relocate((char *)D_00420DF0[target->slot]->file);
    pool_node = node->unk10->pool_node;
    node->unk10->pool_node = node->pool_node;
    node->pool_node = pool_node;
    PoolSlot_LoadGroup(D_00420DF0[node->unk10->slot], values.value[node->unk10->slot]);
    loaded = node->unk10;
    node->unk1C = 0;
    node->unk08 = -1;
    loaded->unk20 = 1;
    D_004472F0 = 0;
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

/* The callback of the node that loads the worn models (D_005CED40): runs the stage it is at, until all are loaded. */
static void Menu_StepWornModelLoad(ChunkUpload *upload) {
    ChunkUploadStages handlers = stream_load_stages;

    if (upload->done == 0) {
        handlers.stage[upload->stage](upload);
    }
}

/* Stage 0 of a chunk upload: open the named file and start reading up to 0x1A2000 bytes. */
static void Menu_OpenPackageFile(ChunkUpload *upload) {
    s32 file_handle;

    file_handle = func_00102258(upload->name, 1, 1);
    upload->unk08 = file_handle;
    if (file_handle < 0) {
        upload->stage = 0;
        upload->done = 0;
        upload = NULL; // match: retail goes on to read through this null pointer
    }
    func_001025C0(upload->unk08, upload->pool_node->buffer, 0x1A2000);
    D_00446AD0 = upload->unk08;
    upload->stage++;
}

/* Stage 1 of a chunk upload: when the read has finished close the file and go on; on error start over. */
static void Menu_WaitPackageRead(ChunkUpload *upload) {
    s32 status;

    status = func_00102790(upload->unk08, 1);
    if (status == 0) {
        func_001024B0(upload->unk08);
        D_00446AD0 = -1;
        upload->stage++;
    } else if (status < 0) {
        func_001024B0(upload->unk08);
        upload->done = upload->stage = 0;
        D_00446AD0 = -1;
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

/* The callback of the package load node D_005CEEE0: runs the stage it is at, until it is done. */
static void Menu_StepPackageLoad(ChunkUpload *upload) {
    ChunkUploadStages stages = package_load_stages;

    if (upload->done == 0) {
        stages.stage[upload->stage](upload);
    }
}

/* Gives a load node a buffer from the free list, if one is left. Returns nothing. */
int Menu_TakeLoadBuffer(LinkNode *node) { // match: declared int in kf_link.h, returns nothing
    ChunkPoolNode *free_node = D_004472B0;

    if (free_node != NULL) {
        node->pool_node = free_node;
        D_004472B0 = free_node->next;
    }
}

/* Puts a load node's buffer back on the free list. */
static void Menu_FreeLoadBuffer(MenuFreeRequest *request) {
    ChunkPoolNode *node = (ChunkPoolNode *)request->node;
    ChunkPoolNode *old_head = D_004472B0;

    D_004472B0 = node;
    node->next = old_head;
}

/* Puts every load buffer back on the free list (a screen is left); a load still in progress is kept, moved to
   the spare node, with the node it was for marked as having no slot. */
void Menu_ResetLoadBuffers(void) {
    LinkNode *loading = D_00446C90;
    u32 i;

    D_004472B0 = D_005CE900;
    for (i = 0; i < 15; i++) {
        D_005CE900[i].next = &D_005CE900[i + 1];
    }
    D_005CE900[15].next = NULL;
    if (loading != NULL) {
        D_005CE8B0 = *loading;
        D_00446C90 = &D_005CE8B0;
        D_005CE8B0.next = NULL;
        if (D_005CE8B0.unk10 != NULL) {
            D_005CE8B0.unk10->slot = -1;
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

/* A copy of Menu_WriteItemModelName, the entry of group 10 in item_name_writers; no item has that group. */
static void Menu_WriteGroup10ModelName(char *buffer, MenuItem *item) {
    sprintf(buffer, "menu/select2/i%04d.mod", item_name_defs[item->unk0C].file_number);
}

/* per MenuItem.group, the function that writes the item's model file name */
static const MenuNameHandlers item_name_writers = { {
    Menu_WriteItemModelName, Menu_WriteSpellModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName,
    Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteItemModelName, Menu_WriteGroup10ModelName, Menu_WriteItemModelName,
} };

/* Writes the model file names of the five items on show: the current one, those of the next and
   previous categories, and the next and previous items of this category. */
static void Menu_SetEquipModelNames(void) {
    MenuNameHandlers handlers = item_name_writers;
    MenuItem *item;
    void (*handler)(char *buffer, MenuItem *item);

    item = D_005CE840[0]->item;
    handler = handlers.handler[item->group];
    handler(D_00426498, item);
    item = (MenuItem *)D_005CE840[0]; // match: the category passes through the item's register
    item = ((MenuCategory *)item)->next->item;
    handler = handlers.handler[item->group];
    handler(D_004264E8, item);
    item = (MenuItem *)D_005CE840[0]; // match: as above
    item = ((MenuCategory *)item)->prev->item;
    handler = handlers.handler[item->group];
    handler(D_00426538, item);
    item = (MenuItem *)D_005CE840[0]; // match: as above
    item = ((MenuCategory *)item)->item->next;
    handler = handlers.handler[item->group];
    handler(D_00426588, item);
    item = (MenuItem *)D_005CE840[0]; // match: as above
    item = ((MenuCategory *)item)->item->prev;
    handler = handlers.handler[item->group];
    handler(D_004265D8, item);
}

/* Writes the model file names of the player's weapon and the eight things worn into their upload nodes. */
static void Menu_SetWornModelNames(void) {
    sprintf(D_005CE9F8, "menu/equip/i%04d.mod", item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].file_number);
    sprintf(D_005CEA98, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST].file_number);
    sprintf(D_005CEAE8, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST].file_number);
    sprintf(D_005CEB38, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST].file_number);
    sprintf(D_005CEB88, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST].file_number);
    sprintf(D_005CEBD8, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST].file_number);
    sprintf(D_005CEC28, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST].file_number);
    sprintf(D_005CEC78, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST].file_number);
    sprintf(D_005CECC8, "menu/equip/i%04d.mod", item_name_defs[player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST].file_number);
    D_005CED40.unk20 = 0;
}

/* Sets up the loads of the player's equipment models (nine nodes behind D_005CED40) and of the five
   menu item models, and chains them all for the loader. */
// match: the order of the statements decides which node address gets which register (and D_005CED40 gets none)
static void Menu_ChainEquipModels(void) {
    Menu_TakeLoadBuffer(&D_005CED40);
    Menu_TakeLoadBuffer(&D_005CE9D0);
    Menu_TakeLoadBuffer(&D_005CEA70);
    Menu_TakeLoadBuffer(&D_005CEAC0);
    Menu_TakeLoadBuffer(&D_005CEB10);
    Menu_TakeLoadBuffer(&D_005CEB60);
    Menu_TakeLoadBuffer(&D_005CEBB0);
    Menu_TakeLoadBuffer(&D_005CEC00);
    Menu_TakeLoadBuffer(&D_005CEC50);
    Menu_TakeLoadBuffer(&D_005CECA0);
    Menu_SetWornModelNames();
    D_005CEA70.slot = 2;
    D_005CEAC0.slot = 4;
    D_005CEB10.slot = 3;
    D_005CEB60.slot = 5;
    D_005CEBB0.slot = 6;
    D_005CEC00.slot = 7;
    D_005CEC50.slot = 8;
    D_005CECA0.slot = 9;
    D_005CED40.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CE9D0.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CEA70.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CEAC0.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CEB10.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CEB60.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CEBB0.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CEC00.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CEC50.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CECA0.callback = (void (*)(void))Menu_StepWornModelLoad;
    D_005CE9D0.slot = 0;
    D_005CE9D0.unk1C = 0;
    D_005CEA70.unk1C = 0;
    D_005CEAC0.unk1C = 0;
    D_005CEB10.unk1C = 0;
    D_005CEB60.unk1C = 0;
    D_005CEBB0.unk1C = 0;
    D_005CEC00.unk1C = 0;
    D_005CEC50.unk1C = 0;
    D_005CECA0.unk1C = 0;
    D_005CED40.unk10 = &D_005CEA70;
    D_005CEA70.next = &D_005CE9D0;
    D_005CE9D0.next = &D_005CEAC0;
    D_005CED40.unk1C = 0;
    D_005CED40.unk20 = 0;
    D_005CEAC0.next = &D_005CEB10;
    D_005CEB10.next = &D_005CEB60;
    D_005CEB60.next = &D_005CEBB0;
    D_005CEBB0.next = &D_005CEC00;
    D_005CEC00.next = &D_005CEC50;
    D_005CEC50.next = &D_005CECA0;
    D_005CECA0.next = NULL;
    Menu_SetEquipModelNames();
    Menu_TakeLoadBuffer(&D_00426470);
    Menu_TakeLoadBuffer(&D_004264C0);
    Menu_TakeLoadBuffer(&D_00426510);
    Menu_TakeLoadBuffer(&D_00426560);
    Menu_TakeLoadBuffer(&D_004265B0);
    D_00426470.next = &D_004264C0;
    D_00426470.callback = (void (*)(void))Menu_StepModelLoad;
    D_004264C0.next = &D_00426510;
    D_004264C0.callback = (void (*)(void))Menu_StepModelLoad;
    D_00426510.next = &D_00426560;
    D_00426510.callback = (void (*)(void))Menu_StepModelLoad;
    D_00426560.next = &D_004265B0;
    D_00426560.callback = (void (*)(void))Menu_StepModelLoad;
    D_004265B0.next = NULL;
    D_004265B0.callback = (void (*)(void))Menu_StepModelLoad;
    D_00426470.unk20 = 0;
    D_004264C0.unk20 = 0;
    D_00426510.unk20 = 0;
    D_00426560.unk20 = 0;
    D_004265B0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_00426510.unk1C = 0;
    D_00426560.unk1C = 0;
    D_00426470.unk1C = D_004265B0.unk1C = 0;
    D_00446C90 = &D_005CED40;
    D_005CED40.next = &D_00426470;
}

/* Writes the model file names of the five items on show in a long list: the current one, its two
   neighbours, and the items seven steps on either way (or the list's ends when nearer). */
static void Menu_SetAllItemsModelNames(void) {
    MenuItem *ahead;
    MenuItem *behind;
    s32 i;

    ahead = behind = D_005CE860[0];
    if (D_00446C38 >= 8) {
        for (i = 0; i < 7; i++) {
            ahead = ahead->next;
            behind = behind->prev;
            if (ahead == D_00446C24) {
                i++;
                while (i < 7 && behind != D_00446C28) {
                    i++;
                    behind = behind->prev;
                }
                break;
            } else if (behind == D_00446C28) {
                i++;
                while (i < 7 && ahead != D_00446C24) {
                    i++;
                    ahead = ahead->next;
                }
                break;
            }
        }
    } else {
        ahead = behind = D_00446C24;
    }
    sprintf(D_00426498, "menu/select2/i%04d.mod", item_name_defs[D_005CE850[0]->unk0C].file_number);
    sprintf(D_00426538, "menu/select2/i%04d.mod", item_name_defs[D_005CE850[0]->prev->unk0C].file_number);
    sprintf(D_004264E8, "menu/select2/i%04d.mod", item_name_defs[D_005CE850[0]->next->unk0C].file_number);
    sprintf(D_00426588, "menu/select2/i%04d.mod", item_name_defs[ahead->unk0C].file_number);
    sprintf(D_004265D8, "menu/select2/i%04d.mod", item_name_defs[behind->unk0C].file_number);
}

/* Opens main menu line 5: names the five models of the list of everything owned and chains their loads. */
static void Menu_ChainAllItemsModels(void) {
    if (D_005CE850[0] != 0) {
        Menu_SetAllItemsModelNames();
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

/* Writes the model file names of the three spells on show: the one the cursor is on and its two neighbours. */
static void Menu_SetSpellModelNames(void) {
    sprintf(D_00426498, "menu/select/m%04d.mod", D_005CE880[0]->unk0C);
    sprintf(D_00426538, "menu/select/m%04d.mod", D_005CE880[0]->prev->unk0C);
    sprintf(D_004264E8, "menu/select/m%04d.mod", D_005CE880[0]->next->unk0C);
}

/* Opens main menu line 1: names the three spell models and chains the load nodes (nodes 3 and 4 apart, unused). */
static void Menu_ChainSpellModels(void) {
    Menu_SetSpellModelNames();
    Menu_TakeLoadBuffer(&D_00426470);
    Menu_TakeLoadBuffer(&D_004264C0);
    Menu_TakeLoadBuffer(&D_00426510);
    Menu_TakeLoadBuffer(&D_00426560);
    Menu_TakeLoadBuffer(&D_004265B0);
    D_00426470.next = &D_004264C0;
    D_00426470.callback = (void (*)(void))Menu_StepModelLoad;
    D_004264C0.next = &D_00426510;
    D_004264C0.callback = (void (*)(void))Menu_StepModelLoad;
    D_00426510.next = NULL;
    D_00426510.callback = (void (*)(void))Menu_StepModelLoad;
    D_00426560.next = &D_004265B0;
    D_00426560.callback = (void (*)(void))Menu_StepModelLoad;
    D_004265B0.next = NULL;
    D_004265B0.callback = (void (*)(void))Menu_StepModelLoad;
    D_00426470.unk20 = 0;
    D_004264C0.unk20 = 0;
    D_00426510.unk20 = 0;
    D_00426560.unk20 = 0;
    D_004265B0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_00426510.unk1C = 0;
    D_00426560.unk1C = 0;
    D_00426470.unk1C = D_004265B0.unk1C = 0;
    D_00446C90 = &D_00426470;
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
        Menu_TakeLoadBuffer(&D_005CEF30[i]);
        D_005CEF30[i].callback = (void (*)(void))Menu_StepModelLoad;
        D_005CEF30[i].unk1C = 0;
        D_005CEF30[i].unk20 = 0;
        if (item_counts[numbers.number[i]] != 0) {
            sprintf(D_005CEF30[i].name, "menu/select/i%04d.mod", item_name_defs[numbers.number[i]].file_number);
        } else {
            sprintf(D_005CEF30[i].name, "menu/equip/i7000.mod");
        }
    }
    if (D_00446C90 != NULL) {
        D_00446C90->next = &D_005CEF30[0];
    } else {
        D_00446C90 = &D_005CEF30[0];
    }
    for (i = 10; i >= 0; i--) {
        D_005CEF30[i].next = &D_005CEF30[i + 1];
    }
    D_005CEF30[11].next = NULL;
}

/* The spell cursor went on one: the model shown moves to node 2, node 1's becomes the one shown, node 1 is
   made fresh, and the three are chained again. */
static void Menu_ShiftSpellModelsNext(int unused) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (D_00446C90 != &D_00426510) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_00426510);
    D_00426510 = D_00426470;
    node = &D_00426510;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &D_004264C0; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    D_00426470 = D_004264C0;
    done = D_00426470.unk20;
    D_00426470.next = NULL;
    if (done == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_004264C0);
    if (pending == NULL) {
        pending = &D_004264C0;
    }
    D_004264C0.unk08 = -1;
    D_004264C0.unk1C = 0;
    D_004264C0.next = NULL;
    D_004264C0.unk20 = 0;
    current = D_00446C90;
    if (current != NULL) {
        current->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
}

/* The spell cursor went back one: the model shown moves to node 1, node 2's becomes the one shown, node 2 is
   made fresh, and the three are chained again. */
static void Menu_ShiftSpellModelsPrev(int unused) {
    LinkNode *pending;
    LinkNode *current;
    LinkNode *node;
    s32 done;

    if (D_00446C90 != &D_004264C0) {
        Menu_CopyLoadToSpare();
    } else {
        Menu_MoveLoadToSpare();
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_004264C0);
    D_004264C0 = D_00426470;
    node = &D_004264C0;
    done = node->unk20;
    node->next = NULL;
    pending = node;
    node = &D_00426510; // match: unused, but keeps the address in its own register for the two lines above
    if (done != 0) {
        pending = NULL;
    }
    D_00426470 = D_00426510;
    done = D_00426470.unk20;
    D_00426470.next = NULL;
    if (done == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_00426510);
    if (pending == NULL) {
        pending = &D_00426510;
    }
    D_00426510.unk08 = -1;
    D_00426510.unk1C = 0;
    D_00426510.next = NULL;
    D_00426510.unk20 = 0;
    current = D_00446C90;
    if (current != NULL) {
        current->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_00426510;
    D_00426510.next = &D_004264C0;
    D_004264C0.next = NULL;
}

/* Allocates the sixteen 0x25800-byte load buffers, builds their free list and sets up the package load node. */
void Menu_InitLoadBuffers(void) {
    u32 i;

    MenuArena_Init(D_00446F58);
    D_004472B0 = D_005CE900;
    for (i = 0; i < 15; i++) {
        D_005CE900[i].next = &D_005CE900[i + 1];
        D_005CE900[i].buffer = (void *)MenuArena_Alloc(0x25800, 0x10);
    }
    D_005CE900[15].next = NULL;
    D_005CE900[15].buffer = (void *)MenuArena_Alloc(0x25800, 0x10);
    D_005CEEE0.pool_node = D_005CE900;
    D_005CEEE0.callback = Menu_StepPackageLoad;
    D_005CEEE0.stage = 0;
    D_005CEEE0.done = 0;
    sprintf(D_005CEEE0.name, "menu/select/npc.mod");
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
    Menu_LoadPartFile("menu/main_para.dat", &D_00446A30, &D_0044722C);
    Menu_LoadPartFile("menu/item_para.dat", &D_00446A34, &D_00447230);
    Menu_LoadPartFile("menu/magic_para.dat", &D_00446A38, &D_00447234);
    Menu_LoadPartFile("menu/equip_para.dat", &D_00446A3C, &D_00447238);
    Menu_LoadPartFile("menu/equip_a_para.dat", &D_00446A40, &D_0044723C);
    Menu_LoadPartFile("menu/equip_g_para.dat", &D_00446A44, &D_00447240);
    Menu_LoadPartFile("menu/equip_m_para.dat", &D_00446A48, &D_00447244);
    Menu_LoadPartFile("menu/equip_s_para.dat", &D_00446A80, &D_0044726C);
    Menu_LoadPartFile("menu/equip_g2_para.dat", &D_00446A7C, &D_00447268);
    Menu_LoadPartFile("menu/status_para.dat", &D_00446A50, &D_0044724C);
    Menu_LoadPartFile("menu/keylist_para.dat", &D_00446A6C, &D_00447260);
    Menu_LoadPartFile("menu/image_para.dat", &D_00446A54, &D_00447250);
    Menu_LoadPartFile("menu/recol_para.dat", &D_00446A58, &D_00447254);
    Menu_LoadPartFile("menu/recol_a_para.dat", &D_00446A84, &D_00447270);
    Menu_LoadPartFile("menu/recol_g_para.dat", &D_00446A88, &D_00447274);
    Menu_LoadPartFile("menu/recol_g2_para.dat", &D_00446A8C, &D_00447278);
    Menu_LoadPartFile("menu/item_sel_para.dat", &D_00446A4C, &D_00447248);
    Menu_LoadPartFile("menu/system_para.dat", &D_00446A5C, &D_00447258);
    Menu_LoadPartFile("menu/option_para.dat", &D_00446A68, &D_0044725C);
    Menu_LoadPartFile("menu/keyconf_para.dat", &D_00446A70, &D_00447264);
    Menu_LoadPartFile("menu/preshop_para.dat", &D_00446A74, &D_00446BFC);
    Menu_LoadPartFile("menu/shop_para.dat", &D_00446A78, &D_00446C00);
    Menu_LoadPartFile("menu/shop_a_para.dat", &D_00446A98, &D_00446C10);
    Menu_LoadPartFile("menu/shop_g_para.dat", &D_00446A9C, &D_00446C14);
    Menu_LoadPartFile("menu/shop_g2_para.dat", &D_00446A94, &D_00446C0C);
    Menu_LoadPartFile("menu/pretune_para.dat", &D_00446AA0, &D_00446C4C);
    Menu_LoadPartFile("menu/tune_para.dat", &D_00446AA4, &D_00446C50);
    Menu_LoadPartFile("menu/tune_s_para.dat", &D_00446AA8, &D_00446C54);
    Menu_LoadPartFile("menu/slot_para.dat", &D_00446A60, &D_00446BF4);
    Menu_LoadPartFile("menu/data_para.dat", &D_00446A64, &D_00446BF8);
    Menu_LoadPartFile("menu/mc_para.dat", &D_00446AAC, &D_00446C58);
    Menu_LoadPartFile("menu/bright_para.dat", &D_00446AB0, &D_00446C5C);
    Menu_LoadPartFile("menu/shop_mess_para.dat", &D_00446AB4, &D_00446C60);
    Menu_LoadPartFile("menu/tune_mess_para.dat", &D_00446AB8, &D_00446C64);
    Menu_LoadPartFile("menu/mc_mess_para.dat", &D_00446ABC, &D_00446C68);
    Menu_LoadPartFile("menu/equip_m2_para.dat", &D_00446AC0, &D_00446C6C);
}

/* Sets the menus up: the frame buffers, the menu and font textures, the twenty equipment pictures, then every
   list of menu items (one MenuItem per definition, numbered) and the ring of the eleven lists. */
void Menu_Init(void) {
    char name[0x40];
    char *menu_file;
    s32 i;
    s32 number;

    sceGsResetGraph(0, 1, 2, 1);
    sceGsSetDefDBuffDc(&D_00426140, 0, 0x280, 0xE0, 3, 0x30, 1);
    D_00426140.clear1_rgbaq = D_00426140.clear0_rgbaq = 0x180000000L;
    MenuHeap_Reset();
    Menu_LoadAllParts();
    menu_file = MenuHeap_Alloc(0xE0000);
    MenuFile_Load("menu/menu.tmx", menu_file);
    MenuFile_IndexBlocks(menu_file);
    MenuHeap_Mark();
    D_0042A8D0.header = MenuHeap_Alloc(0x23000);
    MenuFile_Load("menu/font.tmr", D_0042A8D0.header);
    MenuTexture_GetRegisters(D_0042A8D0.header, &D_0042A8D0.tex0, &D_0042A8D0.trailer);
    MenuHeap_FreeToMark();
    for (i = 0; i < 20; i++) {
        D_005CEE60[i] = MenuHeap_Alloc(0x2800);
        sprintf(name, "menu/equip/a%03d.mi2", i);
        MenuFile_Load(name, D_005CEE60[i]);
        PoolNodeFile_Relocate((RelocFile *)D_005CEE60[i]);
    }
    for (i = 0; i < 150; i++) {
        D_00424600[i].unk0C = i;
        D_00424600[i].unk00 = (int)&item_common_defs[i];
        D_00424600[i].group = 11;
    }
    for (i = 150; i < 241; i++) {
        D_00424600[i].unk00 = (int)&fx_table[i - 150];
        D_00424600[i].unk0C = i - 150;
        D_00424600[i].group = 1;
    }
    number = 0;
    for (i = 0; i < 150; i++) {
        D_00421A30[number].unk00 = (int)&item_common_defs[i];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 12;
        number++;
    }
    for (i = 0; i < 70; i++) {
        D_00421A30[number].unk00 = (int)&weapon_defs[i];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 0;
        number++;
    }
    for (i = 0; i < 30; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 2;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i + 30];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 3;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i + 55];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 4;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i + 80];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 5;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i + 105];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 6;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i + 130];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 7;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i + 140];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 8;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00421A30[number].unk00 = (int)&worn_defs[i + 130];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 9;
        number++;
    }
    number = 0;
    for (i = 0; i < 150; i++) {
        D_004210D0[i].unk00 = (int)&item_common_defs[i];
        D_004210D0[i].unk0C = number;
        D_004210D0[i].group = 11;
        number++;
    }
    for (i = 0; i < 70; i++) {
        D_004231F0[i].unk00 = (int)&weapon_defs[i];
        D_004231F0[i].unk0C = number;
        D_004231F0[i].group = 0;
        number++;
    }
    for (i = 0; i < 30; i++) {
        D_00423650[i].unk00 = (int)&worn_defs[i];
        D_00423650[i].unk0C = number;
        D_00423650[i].group = 2;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00423830[i].unk00 = (int)&worn_defs[i + 30];
        D_00423830[i].unk0C = number;
        D_00423830[i].group = 3;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_004239C0[i].unk00 = (int)&worn_defs[i + 55];
        D_004239C0[i].unk0C = number;
        D_004239C0[i].group = 4;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00423B50[i].unk00 = (int)&worn_defs[i + 80];
        D_00423B50[i].unk0C = number;
        D_00423B50[i].group = 5;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00423CE0[i].unk00 = (int)&worn_defs[i + 105];
        D_00423CE0[i].unk0C = number;
        D_00423CE0[i].group = 6;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00423E70[i].unk00 = (int)&worn_defs[i + 130];
        D_00423E70[i].unk0C = number;
        D_00423E70[i].group = 7;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00423F10[i].unk00 = (int)&worn_defs[i + 140];
        D_00423F10[i].unk0C = number;
        D_00423F10[i].group = 8;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00423FB0[i].unk00 = (int)&worn_defs[i + 130];
        D_00423FB0[i].unk0C = number;
        D_00423FB0[i].group = 9;
        number++;
    }
    for (i = 0; i < 91; i++) {
        D_00424050[i].unk0C = i;
        D_00424050[i].unk00 = (int)&fx_table[i];
        D_00424050[i].group = 1;
    }
    D_005CE760.group = 11;
    D_005CE770.item = D_005CE6B0.item;
    D_005CE770.group = 0;
    D_005CE780.item = D_005CE6C0.item;
    D_005CE780.group = 2;
    D_005CE790.item = D_005CE6D0.item;
    D_005CE790.group = 3;
    D_005CE7A0.item = D_005CE6E0.item;
    D_005CE7A0.group = 4;
    D_005CE7B0.item = D_005CE6F0.item;
    D_005CE7B0.group = 5;
    D_005CE7C0.item = D_005CE700.item;
    D_005CE7C0.group = 6;
    D_005CE7D0.item = D_005CE710.item;
    D_005CE7D0.group = 7;
    D_005CE7E0.item = D_005CE720.item;
    D_005CE7E0.group = 8;
    D_005CE7F0.item = D_005CE730.item;
    D_005CE7F0.group = 9;
    D_005CE800.item = D_005CE740.item;
    D_005CE800.group = 1;
    D_005CE810.item = D_005CE6A0.item;
    D_005CE810.group = 10;
    D_005CE810.prev = &D_005CE770;
    D_005CE770.prev = &D_005CE800;
    D_005CE800.prev = &D_005CE780;
    D_005CE780.prev = &D_005CE790;
    D_005CE790.prev = &D_005CE7A0;
    D_005CE7A0.prev = &D_005CE7B0;
    D_005CE7B0.prev = &D_005CE7C0;
    D_005CE7C0.prev = &D_005CE7D0;
    D_005CE7D0.prev = &D_005CE7E0;
    D_005CE7E0.prev = &D_005CE7F0;
    D_005CE7F0.prev = &D_005CE810;
    D_005CE770.next = &D_005CE810;
    D_005CE800.next = &D_005CE770;
    D_005CE780.next = &D_005CE800;
    D_005CE790.next = &D_005CE780;
    D_005CE7A0.next = &D_005CE790;
    D_005CE7B0.next = &D_005CE7A0;
    D_005CE7C0.next = &D_005CE7B0;
    D_005CE7D0.next = &D_005CE7C0;
    D_005CE7E0.next = &D_005CE7D0;
    D_005CE7F0.next = &D_005CE7E0;
    D_005CE810.next = &D_005CE7F0;
    MenuHeap_Mark();
}

extern char D_00446AE0[8];
extern char D_00446AE8[8];
extern char D_00446AF0[8];
extern char D_00446AF8[8];
extern char D_00446B00[8];
extern char D_00446B08[8];
extern char D_00446B10[8];
/* the seven weekday names (Sunday first, in Japanese); copied to a local and never read */
static const MenuWeekdayNames weekday_names_table = { {
    D_00446AE0, D_00446AE8, D_00446AF0, D_00446AF8, D_00446B00, D_00446B08, D_00446B10,
} };

/* Runs the menu from opening to closing: sets up, calls the handler D_00446AC4 names every frame until it gives 0,
   then shows the last frames. `mode` (D_00446C94) is which menu; with mode 1 the result is 1 when
   D_005CF2F0.unk16 was set. */
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
    D_00446C94 = mode;
    D_004472D0 = D_004472D4 = Pad_GetMode(0, 0);
    while (func_001CEBE0(), menu_run_steps[D_00446AC4]() != 0) {
        sceGsSyncPath(0, 0);
        D_00447200 = Menu_EndFrame(1);
        Input_Update();
        status = Pad_GetMode(0, 0);
        D_004472D0 = status;
        if (status != D_004472D4) {
            D_00447204 = 1;
        }
        D_004472D4 = status;
        MenuPad_Update();
        if (D_00447204 > 0) {
            D_00447204++;
            if (D_00447204 > 20) {
                D_00447204 = 0;
            } else {
                D_00446DE8 = 0;
            }
        }
    }
    if (D_00446C94 == 0 && D_005CF2F0.unk16 == 0) {
        packet = MenuDraw_NextPacket();
        packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
        packet[2] = 0xE;
        packet[3] = 0;
        *(u64 *)&packet[4] = 0x66802B240;
        packet[6] = 6;
        packet[7] = 0;
        *(u64 *)&packet[8] = D_00432D80.frame;
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
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 11;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
        D_00446F7C = D_00446F78 = 1;
    }
    D_00446AC4 = 0;
    Sound_UnmuteTagged();
    Sound_UnmutePositional();
    Sound_UnmuteTracked();
    status = D_00446C94;
    if (status == 0) {
        sceGsSyncPath(0, 0);
        while (sceGsSyncV(0) != 0);
    }
    switch (D_00446C94) {
    case 2:
    case 3:
    case 4:
    case 5:
        D_005CF2F0.unk16 = 0;
        sceGsSyncPath(0, 0);
        Menu_EndFrame(0);
        sceGsSyncV(0);
        sceGsSyncPath(0, 0);
        Menu_EndFrame(0);
        sceGsSyncV(0);
        return 0;
    case 0:
        if (D_005CF2F0.unk16 != 0) {
            D_005CF2F0.unk16 = 0;
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
        if (D_005CF2F0.unk16 != 0) {
            D_005CF2F0.unk16 = 0;
            return 1;
        }
        return 0;
    }
}

extern char D_00446B28[8];
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
    "Use Item", "Cast a Spell", D_00446B28, "Status Viewer", "Recollection", "Visible Item", "System Config", NULL,
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
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
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
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
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
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
}

/* Draws the menu's item model with the matrix given: sends a three-quadword packet of GS settings,
   draws the model of node D_00426470 and puts the view-to-screen matrix back. */
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
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 3;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    DRAW_NODE_MODEL(&D_00426470, matrix, D_00446C78);
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
}

/* First step of Menu_Run's loop. In the game's menu (mode 0): copies the shown frame buffer into the drawn one in
   two half-brightness passes, as the backdrop. Returns 1. */
static s32 Menu_CopyFrameToTexture(void) {
    f32 unused[28]; // match: 0x70 bytes of locals nothing uses
    u32 texture_base; /* where the shown buffer is as a texture: 0x1180 when it is the second one */
    u32 *packet;

    if (D_00446C94 == 0) {
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
    }
    sceGsSyncPath(0, 0);
    D_00446AC4++;
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
    func_001CEBE0();
    sceGsSyncPath(0, 0);
    D_00446C98 = Timer_GetTicks();
    even_field = sceGsSyncV(0) == 0;
    Timer_Restart();
    if (D_00446DE8 & 0x100) {
        D_00447208++;
    }
    D_00447208 %= 3;
    sceGsSetHalfOffset(D_00446F78 != 0 ? D_00426310 : (char *)D_004261A0, 0x800, 0x800, even_field);
    sceGsSetHalfOffset2(D_00446F78 != 0 ? D_00426390 : D_00426220, 0x800, 0x800, even_field);
    FlushCache(0);
    sceGsSyncPath(0, 0);
    sceGsSwapDBuffDc(&D_00426140, D_00446F78);
    if (run_handler != 0) {
        frame_end_drawers[(s8)game_options[8]]();
    }
    D_00446F78 = D_00446F78 == 0;
    D_00446F7C = D_00446F7C == 0;
    return even_field;
}

/* The backdrop of option 8's first setting, in the game's menu only: the grabbed frame as a sprite, then a
   rectangle of the colour { 0x10, 0x10, 0x10 } over it. */
static void Menu_DrawBackdropPlain(void) {
    f32 unused[24];
    ColorBytes color = D_00446B18[0];
    u32 *packet;

    if (D_00446C94 == 0) {
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
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 5;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
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
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
    }
}

/* The backdrop of option 8's third setting: every kind 2 part of menu/item_sel_para.dat. That these are
   backdrops is a guess from where they are drawn. */
static void Menu_DrawBackdropItemSelParts(void) {
    u32 i;

    for (i = 0; i < D_00446A4C; i++) {
        if (D_00447248[i].unk3E == 2) {
            Menu_SetBlendMode(D_00447248[i].unk42);
            MenuSprite_DrawPlain((int)&D_00447248[i], 2, 0);
            Menu_SetBlendMode(0);
        }
    }
}

/* The backdrop of option 8's second setting: every kind 2 part of menu/preshop_para.dat. */
static void Menu_DrawBackdropPreshopParts(void) {
    u32 i;

    for (i = 0; i < D_00446A74; i++) {
        if (D_00446BFC[i].unk3E == 2) {
            Menu_SetBlendMode(D_00446BFC[i].unk42);
            MenuSprite_DrawPlain((int)&D_00446BFC[i], 2, 0);
            Menu_SetBlendMode(0);
        }
    }
}

/* Second step of Menu_Run's loop: steps the model loads, then runs the menu step D_00446AC8 names.
   Returns what the step returns, 1 when there is none. */
static int Menu_RunStep(void) {
    int (*step)(char *);

    Menu_StepModelLoads(0);
    step = (int (*)(char *))menu_steps[D_00446AC8]; // match: the steps take nothing, yet D_005CE0E0 is passed
    if (step != NULL) {
        return step(D_005CE0E0);
    }
    return 1;
}

/* Start-up step of a menu screen: loads its textures and blocks, then picks the handler for the screen D_00446C94 names. */
static s32 Menu_Start(void) {
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
    MenuArena_Init(D_00446F58);
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
    if (D_00446C94 == 0) {
        D_004472E0 = 0;
        Menu_BuildItemRings();
        Menu_InitLoadBuffers();
    }
    switch (D_00446C94) {
    case 0:
        D_00446B20 = (void *)Menu_OpenMain;
        break;
    case 1:
        D_00446B20 = (void *)Menu_OpenCardSlot;
        break;
    case 2:
        D_00446B20 = (void *)Menu_UpdateOptions;
        break;
    case 3:
        Menu_ReadButtonSettings();
        D_00446B20 = (void *)Menu_UpdateButtonConfig;
        break;
    case 4:
        D_00446B20 = (void *)Menu_OpenCardCheck;
        break;
    case 5:
        D_004472CF = D_0034968A[0];
        D_00446B20 = (void *)Menu_UpdateBrightness;
        D_00446C09 = 0;
        D_00446C70 = 0;
        break;
    }
    D_00446C04 = 128.0f;
    D_00446AC8++;
    D_00446C08 = 0;
    D_00446C78.blue = 0x80;
    D_00446C78.green = 0x80;
    D_00446C78.red = 0x80;
    D_00446C78.alpha = 0;
    D_00446C80.blue = 0x80;
    D_00446C80.green = 0x80;
    D_00446C80.red = 0x80;
    D_00446C80.alpha = 0;
    D_00446C88.blue = 0x80;
    D_00446C88.green = 0x80;
    D_00446C88.red = 0x80;
    D_00446C88.alpha = 0;
    return TRUE;
}

/* Runs the current screen handler and reports that it ran. */
static s32 Menu_RunHandler(void) {
    ((void (*)(void))D_00446B20)();
    return TRUE;
}

/* Last step of a menu: in the game's menu waits for the model load in progress with the chain cut behind it, then
   takes the lists' cursors back as they were, closes the load's file and ends the menu (step 0, returns FALSE). */
static s32 Menu_CutModelChain(void) {
    LinkNode *node;

    switch (D_00446C94) {
    case 0:
        node = D_00446C90;
        if (node == NULL) {
            D_005CE770.item = D_005CE6B0.item;
            D_005CE780.item = D_005CE6C0.item;
            D_005CE800.item = D_005CE740.item;
            D_005CE790.item = D_005CE6D0.item;
            D_005CE7A0.item = D_005CE6E0.item;
            D_005CE7B0.item = D_005CE6F0.item;
            D_005CE7C0.item = D_005CE700.item;
            D_005CE7D0.item = D_005CE710.item;
            D_005CE7E0.item = D_005CE720.item;
            D_005CE7F0.item = D_005CE730.item;
            Sound_LoadMgBank(D_005CE740.item->unk0C);
            func_001024B0(D_00446AD0);
            D_00446AC8 = 0;
            D_004472CD = 0;
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
        D_00446AC8 = 0;
        return FALSE;
    }
    // match: other steps return nothing (see the report's retail bugs)
}

/* Makes the main menu the current screen, with its parts faded out. */
static void Menu_OpenMain(void) {
    D_004472CE = 0;
    D_00446C04 = 128.0f;
    D_00446C08 = 0;
    D_004472DA = 0;
    memset(D_00425510, 0, 0x400);
    memset(D_00425910, 0, 0x400);
    D_00446B20 = (void *)Menu_UpdateMain;
}

/* The main menu: moves the cursor over its seven lines, draws the parts, and fades out to the chosen screen. */
static void Menu_UpdateMain(HandlerContext *context) {
    u32 i;
    s32 line_count;
    u32 full_alpha;
    u8 kind;

    line_count = 7;
    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        D_004472CD--;
        Sound_PlayMenu(1);
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        D_004472CD++;
        Sound_PlayMenu(1);
    } else if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        if ((Menu_OwnsAnyItem() == 0 && D_004472CD == 5) || (Menu_KnowsCastableSpell() == 0 && D_004472CD == 1)) {
            Sound_PlayMenu(3);
        } else {
            Sound_PlayMenu(0);
            D_004472C4 = 2;
            D_004472DA = 0;
            for (i = 0; i < D_00446A30; i++) {
                if (D_0044722C[i].unk3E == 1 && D_004472CD == D_0044722C[i].slot) {
                    D_00425510[i] = D_00446C04;
                }
            }
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        D_004472DA = 0;
        D_004472C4 = 3;
        Sound_PlayMenu(2);
        for (i = 0; i < D_00446A30; i++) {
            if (D_0044722C[i].unk3E == 1 && D_004472CD == D_0044722C[i].slot) {
                D_00425510[i] = D_00446C04;
            }
        }
    }
    D_004472CD = (D_004472CD + 7) % line_count;

    for (i = 0; i < D_00446A30; i++) {
        kind = D_0044722C[i].unk3E;
        Menu_SetBlendMode(D_0044722C[i].unk42);
        if (D_004472C4 != 1) {
            MenuSprite_Draw((int)&D_0044722C[i], kind + 1000, D_004472CD, (int)D_00425510[i]);
        } else {
            MenuSprite_Draw((int)&D_0044722C[i], kind, D_004472CD, (int)D_00425510[i]);
        }
        Menu_SetBlendMode(0);
    }

    switch (D_004472C4) {
    case 0:
        for (i = 0; i < D_00446A30; i++) {
            D_00425510[i] += D_0044722C[i].alpha / 10.0f;
            if (D_00425510[i] > D_0044722C[i].alpha) {
                for (i = 0; i < D_00446A30; i++) {
                    full_alpha = D_0044722C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472C4 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446A30; i++) {
            D_00425510[i] -= D_0044722C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                D_00425510[i] = 0.0f;
                if (D_0044722C[i].unk3E != 1 || D_004472CD != D_0044722C[i].slot) {
                    memset(D_00425510, 0, 0x400);
                    if (D_004472C4 == 2) {
                        if (Menu_OwnsAnyItem() == 0 && D_004472CD == 5) {
                            D_004472C4 = 0;
                            break;
                        }
                        if (Menu_KnowsCastableSpell() != 0 || D_004472CD != 1) {
                            D_00446B20 = (void *)Menu_OpenChosenScreen;
                            context->unk04 = D_004472CD;
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
                                if (D_00446C90 != NULL) {
                                    D_00446C90->next = (LinkNode *)&D_005CEEE0;
                                } else {
                                    D_00446C90 = (LinkNode *)&D_005CEEE0;
                                }
                                D_004472D8 = 0;
                                break;
                            case 5:
                                ((void (*)(s32))Menu_ChainAllItemsModels)(0); // match: the callee takes nothing
                                break;
                            }
                        }
                    } else if (D_004472C4 == 3) {
                        context->unk04 = 0;
                        D_004472CD = 0;
                        D_00446AC8++;
                    }
                    D_004472C4 = 0;
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

    D_00446C04 = 128.0f;
    D_00446B20 = main_menu_screens[*line];
    *line = 0;
    D_00446C08 = 0;
    D_00446C09 = 0;
}

/* Steps the seven-frame counter D_00446B39, wrapping back to 0. Nothing calls it. */
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

    if (D_00446C09 == 0) {
        bound = (f32) target;
        value = value + ((f32) (target - start) / 60.0f);
        if (bound < value) {
            value = bound;
            D_00446C09 = 1;
        }
    } else {
        bound = (f32) start;
        value = value - ((f32) (target - start) / 60.0f);
        if (value < bound) {
            D_00446C09 = 0;
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
    item = D_00426120[0]; // match: dead (the magic menu, Menu_UpdateCastSpell, takes its first row here); it keeps the address's high part out of a register
    if (D_00426110[0] != NULL) {
        Menu_MoveItemListCursor(context, D_004472DA == 0);
    }
    if (D_00426110[0] != NULL) {
        D_00446DF4 = item_counts[D_00426110[0]->unk0C];
    } else {
        D_00446DF4 = 0;
    }
    for (i = 0; i < D_00446A34; i++) {
        kind = D_00447230[i].unk3E;
        ((void (*)(u8))Menu_SetBlendMode)(D_00447230[i].unk42); // match: called as if it returned nothing
        if (D_00426110[0] != NULL) {
            MenuSprite_Draw((int)&D_00447230[i], kind + 1000, 0, (int)D_00425510[i]);
        } else {
            MenuSprite_Draw((int)&D_00447230[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as if it returned nothing
    }
    D_00446B40++;
    if (D_00446B40 >= 0x80) {
        D_00446B40 = 0;
    }
    if (D_00426110[0] != NULL) {
        bar_item = D_00426120[0];
        for (row = 0; row < 7; row++, bar_item = bar_item->prev) {
            if (bar_item == D_00426110[0]) {
                bar.first.f.lo16 = corner.f.lo16;
                bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
                bar.first.f.mid24 = corner.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
                bar.second.f.mid24 = bar.first.f.mid24;
                if (D_004472C5 == 1) {
                    D_00425510[253] = Menu_PulseValue(D_00425510[253], 0x10, 0x20);
                }
                bar.alpha = (s32)D_00425510[253];
                MenuDraw_Bar(&bar);
                break;
            }
        }
    }
    if (D_00426110[0] != NULL) {
        sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        D_00446B3C = wrap_angle(D_00446B3C);
        sceVu0RotMatrixY(local, view, D_00446B3C);
        sceVu0TransMatrix(local, local, D_00421000);
        Menu_DrawItemModel(local);
        item = D_00426120[0];
        MenuText_SetColor(D_00446C78);
        for (row = 0; row < 7; row++, item = item->prev) {
            text = (s32)&item_name_defs[item->unk0C];
            MenuText_SetPen(word);
            ((s32 (*)(s32, s32))MenuText_DrawWrapped)(text, 0x1A); // match: called as if it took and returned a number
            word.f.mid16 += 0xB0;
            word.f.lo16 = 0x7080;
            if (D_00426130[0]->prev == item) {
                break;
            }
        }
        item = D_00426110[0];
        table = Message_GetFontText(D_00446F38);
        slot = item->unk0C + MSG_ITEM_TEXT_FIRST + 1;
        entry = ((s32 *)table)[slot];
        word.raw = 0x000A875083207080;
        MenuText_SetPen(word);
        if (entry != 0) {
            text = table + entry;
            MenuText_SetColor(D_00446C78);
            MenuText_DrawWordWrappedLong((u16 *)text, 0x33);
        }
    }
    if ((D_00446DE8 & 0x4000) && D_004472DA != 0 && D_00426110[0] != NULL) {
        if (D_00426110[0]->unk0C < ITEM_ARROW || D_00426110[0]->unk0C > ITEM_WIDDAS_ARROW) {
            Sound_PlayMenu(0);
            game_status.bit6 = TRUE;
            game_status.unk20 = D_00426110[0]->unk0C;
            D_004472C5 = 2;
            D_004472DA = 0;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else {
            Sound_PlayMenu(2);
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472C5 = 3;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    } else if ((D_00446DE8 & 0x8000) && D_004472DA != 0) {
        Sound_PlayMenu(0);
        D_004472C5 = 4;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472C5 == 2 && D_00446C90 == NULL) {
        D_004472C5 = 5;
    }
    if (D_004472C5 == 3 && D_00446C90 == NULL) {
        D_004472C5 = 6;
    }
    if (D_004472C5 == 4 && D_00446C90 == NULL) {
        D_004472C5 = 7;
    }
    switch (D_004472C5) {
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
        for (i = 0; i < D_00446A34; i++) {
            D_00425510[i] += D_00447230[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447230[i].alpha) {
                for (i = 0; i < D_00446A34; i++) {
                    full_alpha = D_00447230[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472C5 = 1;
                D_004472DA = D_004472C5;
                return;
            }
        }
        break;
    case 5:
    case 6:
    case 7:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A34; i++) {
            D_00425510[i] -= D_00447230[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                switch (D_004472C5) {
                case 5:
                    D_00446B20 = NULL;
                    D_00446AC8++;
                    break;
                case 6:
                    ((s32 (*)(s32))Menu_ResetLoadBuffers)(0); // match: called as if it took a number and returned one
                    D_00446B20 = Menu_OpenMain;
                    break;
                case 7:
                    ((s32 (*)(s32))Menu_ResetLoadBuffers)(0); // match: called as if it took a number and returned one
                    ((void (*)(s32))Menu_ChainKeyItemModels)(0); // match: the callee takes nothing
                    D_00446B20 = Menu_UpdateKeyItems;
                    break;
                }
                D_004472C5 = 0;
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
    if (D_005CE830[0] == 0) {
        D_00446B20 = Menu_OpenMain;
        return;
    }
    if (D_004472C8 < 2) {
        ((void (*)(s32))Menu_MoveSpellCursor)(context); // match: the callee takes nothing
    }
    item = D_005CE890[0];
    word.raw = 0x0C0AAE607C307080;
    MenuText_SetPen(word);
    corner.raw = 0x0A0F42407C307000;
    pitch.raw = 0xB00140;
    MenuText_SetGlyphSize(pitch);
    i = 0;
    number = D_005CE880[0]->unk0C; /* the selected magic's number */
    page = (number - 1) / 15;
    D_00446DF4 = fx_table[number].unkFC;
    D_00446DF8 = (number - 1) % 3 + 1;
    for (; i < D_00446A38; i++) {
        kind = D_00447234[i].unk3E;
        Menu_SetBlendMode(D_00447234[i].unk42);
        MenuSprite_Draw((int)&D_00447234[i], kind + 1000, page, (int)D_00425510[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as if it returned nothing
    }
    for (row = 0; row < 7; row++, item = item->prev) {
        if (D_005CE880[0] == item) {
            bar.first.f.lo16 = corner.f.lo16;
            bar.first.f.mid16 = corner.f.mid16 + row * 0xB0;
            bar.first.f.mid24 = corner.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1220;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_004472C8 == 1) {
                D_00425510[253] = Menu_PulseValue(D_00425510[253], 0x10, 0x20);
            }
            bar.alpha = (s32)D_00425510[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }
    item = D_005CE890[0];
    MenuText_SetColor(D_00446C78);
    for (row = 0; row < 7; row++, item = item->prev) {
        MenuText_DrawLine((u16 *)&fx_table[item->unk0C], 0x1A);
        word.f.mid16 += 0xB0; // match: dead, left over from the item menu (Menu_UpdateUseItem), which sets the row's place here
        word.f.lo16 = 0x7080; // match: dead as well; without the two lines the loop is compiled differently
        if (D_005CE8A0[0]->prev == item) {
            break;
        }
    }
    selected = D_005CE880[0];
    table = Message_GetFontText(D_00446F38);
    slot = selected->unk0C + MSG_MAGIC_TEXT_FIRST + 1;
    entry = ((s32 *)table)[slot];
    word.raw = 0x000A875082E07000;
    MenuText_SetPen(word);
    if (entry != 0) {
        MenuText_DrawWordWrappedLong((u16 *)(table + entry), 0x35);
    }
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    sceVu0TransMatrix(local, view, D_00421010);
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
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 3;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    DRAW_NODE_MODEL(&D_00426470, local, D_00446C78);
    if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        if (player_state.mp_now >= ((FxTableEntry *)D_005CE880[0]->unk00)->unkFC && player_state.unkC0 >= 5000) {
            Sound_PlayMenu(0);
            game_status.unk28 = D_005CE880[0]->unk0C;
            D_004472C8 = 2;
            D_004472DA = 0;
            if (D_00446C90 != NULL) {
                D_00446C90->next = NULL;
            }
        } else {
            Sound_PlayMenu(3);
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472C8 = 2;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472C8 == 2 && D_00446C90 == NULL) {
        D_004472C8 = 3;
    }
    switch (D_004472C8) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00425510[253] += 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] > 16.0f) {
            D_00425510[255] = 16.0f; // match: retail clamps the text alpha here, not the bar's ([253])
        }
        for (i = 0; i < D_00446A38; i++) {
            D_00425510[i] += D_00447234[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447234[i].alpha) {
                D_004472DA = D_004472C8 = 1;
                for (i = 0; i < D_00446A38; i++) {
                    full_alpha = D_00447234[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                return;
            }
        }
        break;
    case 3:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A38; i++) {
            D_00425510[i] -= D_00447234[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                D_004472C8 = 0;
                if (game_status.unk28 != 0) {
                    D_00446B20 = NULL;
                    D_00446AC8++;
                } else {
                    ((s32 (*)(s32))Menu_ResetLoadBuffers)(0); // match: called as if it took a number and returned one
                    D_00446B20 = Menu_OpenMain;
                }
                return;
            }
        }
        break;
    }
}

extern char D_00446B60[8];
extern char D_00446B68[8];
extern char D_00446B70[8];
extern char D_00446B78[8];
extern char D_00446B80[8];
extern char D_00446B88[8];
extern char D_00446B90[8];
extern char D_00446B98[8];
extern char D_00446BA0[8];
extern char D_00446BA8[8];
extern char D_00446BB0[8];
/* per MenuItem.group, the LevelDef number its records start at, as the equipment screen has it */
static const EquipGroupBases equip_group_bases = { { 150, 0, 220, 220, 220, 220, 220, 220, 220, 220, 0, 0 } };
/* the names of the equipment groups ("weapons", "magic", "shield"...); copied to a local and never read */
static const EquipUnusedTable equip_group_names = { {
    D_00446B60, D_00446B68, D_00446B70, D_00446B78, D_00446B80, D_00446B88, D_00446B90, D_00446B98, D_00446BA0, D_00446BA8, D_00446BB0, D_00446BB0,
} };
/* where the three models of the equipment screen go */
static const VuVector4 equip_model_offset_low = { 55.0f, -60.0f, 400.0f, 0.0f };
static const VuVector4 equip_model_offset_middle = { 55.0f, 0.0f, 400.0f, 0.0f };
static const VuVector4 equip_model_offset_high = { 55.0f, 60.0f, 400.0f, 0.0f };
/* per MenuItem.group, the glyph that names the group */
static const EquipGroupBytes equip_group_glyphs = { { 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x63, 0x62, 0x64, 0x65 } };
/* per MenuItem.group, a slot number; copied to a local and never read */
static const EquipGroupBytes equip_group_slots = { { 4, -1, 5, 2, 1, 3, 0, -1, -1, -1, -1 } };

/* The Equip screen (main menu line 2): moves between the eleven lists and inside the list shown, draws the
   screen's parts, the worn weapon's model and the cursor item's model, the item's detail panel (button
   0x1000) or the list of everything worn (0x8000), equips the cursor item (0x4000) and fades in and out. */
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
    if ((D_00446DE8 & 0x10) && D_004472CE != 0 && D_004472DA != 0 && D_004472F0 == 0) {
        EQUIP_LIST = EQUIP_LIST->next;
        ((void (*)(s32))Menu_ShiftEquipModelsNextList)(0); // match: called with the number alone
        D_004472E0--;
        Sound_PlayMenu(1);
    } else if ((D_00446DE8 & 0x40) && D_004472CE != 0 && D_004472DA != 0 && D_004472F0 == 0) {
        EQUIP_LIST = EQUIP_LIST->prev;
        ((void (*)(s32))Menu_ShiftEquipModelsPrevList)(0); // match: called with the number alone
        D_004472E0++;
        Sound_PlayMenu(1);
    } else if (D_004472C6 < 2 && D_004472F0 == 0) {
        ((void (*)(s32))Menu_MoveEquipCursor)(0); // match: the callee takes nothing
    }
    if (D_004472E0 > 10) {
        D_004472E0 = 0;
    } else if (D_004472E0 < 0) {
        D_004472E0 = 10;
    }
    index = CURRENT_ITEM->unk0C - bases.first_item[CURRENT_ITEM->group];
    if (CURRENT_ITEM->group == 0) {
        D_00446E30 = weapon_level_records[index].unk14 + 1;
    } else if (CURRENT_ITEM->group == 1) {
        D_00446E30 = (index - 1) % 3 + 1;
    }
    for (i = 0; i < D_00446A3C; i++) {
        kind = D_00447238[i].unk3E;
        SET_BLEND(D_00447238[i].unk42);
        if (Menu_IsEquipped(D_004472E0, CURRENT_ITEM->group, CURRENT_ITEM->unk0C)) {
            MenuSprite_Draw((int)&D_00447238[i], kind + 1000, 0, (int)D_00425510[i]);
        } else {
            MenuSprite_Draw((int)&D_00447238[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        SET_BLEND(0);
    }
    if ((D_00446DE8 & 0x1000) && D_00446AD5 == 0 && D_004472DA != 0
        && Menu_IsEquipSlotFilled(index, CURRENT_ITEM->group)) {
        if (D_00446AD4 == 0) {
            D_00446B50 = 1;
            D_00446AD4 = 1;
        } else if (D_004472C7 != 2) {
            Sound_PlayMenu(1);
            D_004472C7 = 2;
        } else {
            D_004472C7 = 0;
        }
    }
    if ((D_00446DE8 & 0x8000) && D_00446AD4 == 0 && D_004472DA != 0) {
        Sound_PlayMenu(1);
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
    D_00446ACC = weapon_defs[player_state.weapon].unk7C;
    if (weapon_defs[player_state.weapon].unk63 == 0) {
        D_00446ACC += worn_defs[player_state.equipment[EQUIP_SHIELD]].unk3F;
    }
    PoolNode_Pose((RelocEntry *)D_005CEE60[D_00446ACC]->frames, D_0042A8F0, weapon_matrix, D_00446B5C);
    D_00446B5C++;
    D_00446B5C %= D_005CEE60[D_00446ACC]->frame_count;
    // match: a pointer-typed load, as in CURRENT_ITEM
    D_00446ACC = ((PlayerItemDef *)((MenuItemList *)D_005CE6B0.item)->item)->unk7D;
    switch (weapon_defs[player_state.weapon].unk63) {
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
    draw = D_0042A290[D_00446ACC];
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
    sceVu0TransMatrix((f32 *)&D_0042A8F0[21], unit, &first_offset.x);
    sceVu0TransMatrix((f32 *)&D_0042A8F0[22], unit, &second_offset.x);
    sceVu0TransMatrix((f32 *)&D_0042A8F0[23], unit, &third_offset.x);
    DRAW_SLOT(&D_0042A8F0[21]);
    DRAW_SLOT(&D_0042A8F0[22]);
    DRAW_SLOT(&D_0042A8F0[23]);
    draw(D_0042A8F0, D_00446C88);
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
        definition = &fx_table[CURRENT_ITEM->unk0C];
    } else {
        definition = &item_name_defs[CURRENT_ITEM->unk0C];
    }
    if (D_00446AD4 != 0) {
        switch (CURRENT_ITEM->group) {
        case 0:
            D_00446DF4 = (weapon_level_records[index].unk00[0] + weapon_defs[index].unk00[weapon_level_records[index].unk14].values[0]) * weapon_level_records[index].level / 100;
            D_00446DF8 = (weapon_level_records[index].unk00[1] + weapon_defs[index].unk00[weapon_level_records[index].unk14].values[1]) * weapon_level_records[index].level / 100;
            D_00446DFC = (weapon_level_records[index].unk00[2] + weapon_defs[index].unk00[weapon_level_records[index].unk14].values[2]) * weapon_level_records[index].level / 100;
            D_00446E00 = weapon_defs[index].unk00[weapon_level_records[index].unk14].values[3] * weapon_level_records[index].level / 100;
            D_00446E04 = weapon_defs[index].unk00[weapon_level_records[index].unk14].values[4] * weapon_level_records[index].level / 100;
            D_00446E08 = weapon_defs[index].unk00[weapon_level_records[index].unk14].values[5] * weapon_level_records[index].level / 100;
            D_00446E0C = weapon_defs[index].unk00[weapon_level_records[index].unk14].values[6] * weapon_level_records[index].level / 100;
            D_00446E10 = weapon_defs[index].unk00[weapon_level_records[index].unk14].values[7] * weapon_level_records[index].level / 100;
            D_00446E14 = weapon_defs[index].unk00[weapon_level_records[index].unk14].values[8] * weapon_level_records[index].level / 100;
            D_00446E1C = (s32)(weapon_defs[index].unk68 * 0.03281f * 10.0f);
            D_00446E20 = weapon_level_records[index].level;
            D_00446E18 = item_name_defs[CURRENT_ITEM->unk0C].unk3E;
            D_00446E24 = 100;
            D_00446E28 = player_weapon[EQUIP_LIST->group];
            if (D_004472C7 == 1) {
                RESET_ALPHAS(D_0044723C, D_00446A40);
            }
            if (index != 0) {
                if (D_00446B50 != 0) {
                    Sound_PlayMenu(1);
                }
                D_00446B50 = 0;
                DRAW_PARTS(D_0044723C, D_00446A40, 0);
                MenuText_SetColor(D_00446C80);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
            }
            parts = D_0044723C;
            count = D_00446A40;
            break;
        case 1:
            if (fx_table[CURRENT_ITEM->unk0C].unk102 == 0) {
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
                D_00446E1C = magic->unkFC;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00447244, D_00446A48);
                }
                if (index != 0) {
                    if (D_00446B50 != 0) {
                        Sound_PlayMenu(1);
                    }
                    D_00446B50 = 0;
                    DRAW_PARTS(D_00447244, D_00446A48, (index - 1) / 15);
                    MenuText_SetColor(D_00446C80);
                    Menu_DrawEquipItemText(MSG_MAGIC_TEXT_FIRST);
                }
                parts = D_00447244;
                count = D_00446A48;
            } else {
                D_00446E1C = fx_table[index].unkFC;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00446C6C, D_00446AC0);
                }
                if (index != 0) {
                    if (D_00446B50 != 0) {
                        Sound_PlayMenu(1);
                    }
                    D_00446B50 = 0;
                    DRAW_PARTS(D_00446C6C, D_00446AC0, (index + 1) / 15);
                    MenuText_SetColor(D_00446C80);
                    Menu_DrawEquipItemText(MSG_MAGIC_TEXT_FIRST);
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
                    Sound_PlayMenu(1);
                }
                D_00446B50 = 0;
                for (i = 0; i < D_00446A80; i++) {
                    SET_BLEND(D_0044726C[i].unk42);
                    MenuSprite_Draw((int)&D_0044726C[i], 1002, 1, (int)D_00425910[i]);
                    SET_BLEND(0);
                }
                MenuText_SetColor(D_00446C80);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
            }
            parts = D_0044726C;
            count = D_00446A80;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            D_00446DF4 = (worn_item_level_records[index].unk00[0] + worn_defs[index].unk00[0]) * worn_item_level_records[index].level / 100;
            D_00446DF8 = (worn_item_level_records[index].unk00[1] + worn_defs[index].unk00[1]) * worn_item_level_records[index].level / 100;
            D_00446DFC = (worn_item_level_records[index].unk00[2] + worn_defs[index].unk00[2]) * worn_item_level_records[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * worn_item_level_records[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * worn_item_level_records[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * worn_item_level_records[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * worn_item_level_records[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * worn_item_level_records[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * worn_item_level_records[index].level / 100;
            D_00446E18 = item_name_defs[CURRENT_ITEM->unk0C].unk3E;
            D_00446E1C = player_weapon[EQUIP_LIST->group];
            D_00446E20 = worn_item_level_records[index].level;
            D_00446E24 = 100;
            if (index != 0 && index != 30 && index != 55 && index != 80 && index != 105) {
                if (D_00446B50 != 0) {
                    Sound_PlayMenu(1);
                }
                D_00446B50 = 0;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00447240, D_00446A44);
                }
                DRAW_PARTS(D_00447240, D_00446A44, D_00446E1C);
                MenuText_SetColor(D_00446C80);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
            }
            parts = D_00447240;
            count = D_00446A44;
            break;
        case 7:
        case 8:
        case 9:
            D_00446DF4 = (worn_item_level_records[index].unk00[0] + worn_defs[index].unk00[0]) * worn_item_level_records[index].level / 100;
            D_00446DF8 = (worn_item_level_records[index].unk00[1] + worn_defs[index].unk00[1]) * worn_item_level_records[index].level / 100;
            D_00446DFC = (worn_item_level_records[index].unk00[2] + worn_defs[index].unk00[2]) * worn_item_level_records[index].level / 100;
            D_00446E00 = worn_defs[index].unk00[3] * worn_item_level_records[index].level / 100;
            D_00446E04 = worn_defs[index].unk00[4] * worn_item_level_records[index].level / 100;
            D_00446E08 = worn_defs[index].unk00[5] * worn_item_level_records[index].level / 100;
            D_00446E0C = worn_defs[index].unk00[6] * worn_item_level_records[index].level / 100;
            D_00446E10 = worn_defs[index].unk00[7] * worn_item_level_records[index].level / 100;
            D_00446E14 = worn_defs[index].unk00[8] * worn_item_level_records[index].level / 100;
            D_00446E18 = item_name_defs[CURRENT_ITEM->unk0C].unk3E;
            D_00446E1C = player_weapon[EQUIP_LIST->group];
            if (index != 130 && index != 140 && index != 150) {
                if (D_00446B50 != 0) {
                    Sound_PlayMenu(1);
                }
                D_00446B50 = 0;
                if (D_004472C7 == 1) {
                    RESET_ALPHAS(D_00447240, D_00446A44); // match: the armour panel's list; the parts drawn and faded in this case are D_00447268's
                }
                DRAW_PARTS(D_00447268, D_00446A7C, D_00446E1C);
                MenuText_SetColor(D_00446C80);
                Menu_DrawEquipItemText(MSG_ITEM_TEXT_FIRST);
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
    MenuText_SetColor(D_00446C78);
    word.raw = 0x0A0A875084106EF0;
    word.f.mid24 = 0xA8750;
    MenuText_SetPen(word);
    MenuText_DrawLineCentered(definition, 0x18);
    {
        EquipGroupBytes glyphs = equip_group_glyphs;

        glyph = glyphs.per_group[EQUIP_LIST->group];
        width = MenuDraw_GetWordStart(glyph + 1) - MenuDraw_GetWordStart(glyph) - 1;
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
        MenuText_SetColor(D_00446C78);
        ((int (*)(KindObject *))MenuDraw_Word)((KindObject *)&sprite); // match: the result register stays taken
    }
    if (D_00446AD5 != 0) {
        MenuDraw_SetGsRegister(0x47, 0x30000);
        word.raw = 0x000A87507AF07D00;
        for (i = 0; i < D_00446A80; i++) {
            SET_BLEND(D_0044726C[i].unk42);
            MenuSprite_Draw((int)&D_0044726C[i], D_0044726C[i].unk3E + 1000, 0, (int)D_00425910[i]);
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
                listed = &fx_table[(*D_00420D60[1])->unk0C];
            } else if (i == 10) {
                if ((*D_00420D60[10])->group == 1) {
                    listed = &fx_table[(*D_00420D60[10])->unk0C];
                } else {
                    listed = &item_name_defs[(*D_00420D60[10])->unk0C];
                }
            } else {
                listed = &item_name_defs[(*D_00420D60[i])->unk0C];
            }
            MenuText_SetPen(word);
            MenuText_SetColor(D_00446C80);
            ((int (*)(void *, s32))MenuText_DrawWrapped)(listed, 0x1A); // match: the result register stays taken
            word.f.mid16 += 0xD0;
        }
    }
    if ((D_00446DE8 & 0x4000) && D_004472CE != 0 && D_004472DA != 0) {
        EquipGroupBytes unused_bytes = equip_group_slots;
        f32 unused_pair[32];

        if (*D_00420D60[EQUIP_LIST->group] != *D_00420D90[EQUIP_LIST->group]) {
            Sound_PlayMenu(0);
            if (weapon_defs[player_state.weapon].unk63 == 2 && EQUIP_LIST->group == 0
                && weapon_defs[index].unk63 != 2) {
*D_00420D60[2] = D_00423650;
player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
D_0042A8F0[20].part = NULL;
            }
            (&player_state.weapon)[EQUIP_LIST->group] = index;
            *D_00420D60[EQUIP_LIST->group] = CURRENT_ITEM;
            if (weapon_defs[player_state.weapon].unk63 != 0 && EQUIP_LIST->group == 0) {
*D_00420D60[2] = D_00423650;
player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
D_0042A8F0[20].part = NULL;
            }
            if (weapon_defs[player_state.weapon].unk63 != 0 && EQUIP_LIST->group == 2) {
*D_00420D60[0] = D_004231F0;
player_state.weapon = 0;
D_0042A8F0[15].part = NULL;
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
                D_00420DC8[EQUIP_LIST->group]->unk20 = 0;
                if (D_005CED40.unk10 != NULL) {
                    D_00420DC8[EQUIP_LIST->group]->next = D_005CED40.unk10->next;
                    D_005CED40.unk10->next = D_00420DC8[EQUIP_LIST->group];
                } else {
                    D_005CED40.unk10 = D_00420DC8[EQUIP_LIST->group];
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
        Sound_PlayMenu(2);
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
                D_00446B20 = Menu_OpenMain;
                ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                PoolSlot_ClearParts();
                D_00446C80.alpha = 0;
                D_00446C80.red = D_00446C80.green = D_00446C80.blue = 0x80;
                D_004472C6 = 0;
                D_004472C7 = 0;
                D_00446AD4 = 0;
                D_00446AD5 = 0;
                D_004472F0 = 0;
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

    for (i = 0; i < D_00446A50; i++) {
        Menu_SetBlendMode(D_0044724C[i].unk42);
        MenuSprite_Draw((int)&D_0044724C[i], D_0044724C[i].unk3E + 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472DA = 0;
        D_004472C9 = 3;
    }

    switch (D_004472C9) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A50; i++) {
            D_00425510[i] += D_0044724C[i].alpha / 10.0f;
            if (D_00425510[i] > D_0044724C[i].alpha) {
                for (i = 0; i < D_00446A50; i++) {
                    full_alpha = D_0044724C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472C9 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A50; i++) {
            D_00425510[i] -= D_0044724C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_004472C9 == 3) {
                    D_00446B20 = (void *)Menu_OpenMain;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else {
                    D_00446B20 = (void *)Menu_UpdateKeyItems;
                }
                D_004472C9 = 0;
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
        D_00421050[i] = empty;
    }
}

/* Puts the key item that goes with LevelDef number `key` into the first empty key item slot. */
void Menu_AddKeyItem(s32 key) {
    s32 slot;

    for (slot = 0; slot < 12; slot++) {
        if (D_00421050[slot] < 0) {
            break;
        }
    }
    if (slot < 12) {
        switch (key) {
        case 0x31:
            D_00421050[slot] = 0;
            break;
        case 0x30:
            D_00421050[slot] = 1;
            break;
        case 0x2F:
            D_00421050[slot] = 2;
            break;
        case 0x34:
            D_00421050[slot] = 3;
            break;
        case 0x32:
            D_00421050[slot] = 4;
            break;
        case 0x3A:
            D_00421050[slot] = 5;
            break;
        case 0x39:
            D_00421050[slot] = 6;
            break;
        case 0x38:
            D_00421050[slot] = 7;
            break;
        case 0x37:
            D_00421050[slot] = 8;
            break;
        case 0x33:
            D_00421050[slot] = 9;
            break;
        case 0x3B:
            D_00421050[slot] = 10;
            break;
        case 0x35:
            D_00421050[slot] = 11;
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
static const KeyItemNameWords key_item_name_words = { {
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
    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        Sound_PlayMenu(1);
        D_00446BB8--;
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        Sound_PlayMenu(1);
        D_00446BB8++;
    }
    D_00446BB8 += 3;
    D_00446BB8 %= 3;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    for (i = 0; i < D_00446A6C; i++) {
        Menu_SetBlendMode(D_00447260[i].unk42);
        MenuSprite_Draw((int)&D_00447260[i], 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }
    D_00446BBC++;
    for (i = 0; (s32)i < 6; i++) {
        slot = D_00421050[i + D_00446BB8 * 3];
        if (slot < 0) {
            continue;
        }
        upload = (ChunkUpload *)((s32)D_005CEF30 + slot * sizeof(ChunkUpload)); // match: &D_005CEF30[slot] as a sum of numbers gives the madd
        if (upload->done != 0) {
            file = upload->pool_node->buffer;
            texture = (TextureHeader *)((char *)file + file->texture_offset);
            part = (struct PoolPart *)((char *)file + file->part_offset + 0x10);
            TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages.base[i];
            TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages.base[i];
            D_0042A8F0[31].part = part;
            // match: both words are read through unions, which keeps the two NULL stores below them
            D_0042A8F0[31].unk58 = TEXTURE_HEADER_BITS(texture)->tex0.raw;
            D_0042A8F0[31].unk60 = TEXTURE_HEADER_BITS(texture)->unk18.raw;
            D_0042A8F0[31].unk48 = NULL;
            D_0042A8F0[31].unk40 = NULL;
            MenuTexture_Upload(texture, NULL);
            sceVu0TransMatrix((f32 *)&D_0042A8F0[31], view, positions.position[i]);
            PoolSlot_Draw((RenderSlot *)&D_0042A8F0[31], slot_arg); // match: slot_arg is never set
            number = &numbers.number[D_00421050[i + D_00446BB8 * 3]];
            if (item_counts[*number] != 0) {
                KeyItemNameWords words = key_item_name_words;
                LevelDef *def = &item_name_defs[*number];

                MenuText_SetColor(D_00446C78);
                MenuText_SetPen(words.word[i]);
                MenuText_DrawLineCentered((u16 *)def, 0x10);
            }
        }
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472C9 = 3;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    } else if ((D_00446DE8 & 0x8000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472C9 = 2;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472C9 == 2 && D_00446C90 == NULL) {
        D_004472C9 = 4;
    }
    if (D_004472C9 == 3 && D_00446C90 == NULL) {
        D_004472C9 = 5;
    }

    switch (D_004472C9) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A6C; i++) {
            D_00425510[i] += D_00447260[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447260[i].alpha) {
                for (i = 0; i < D_00446A6C; i++) {
                    full_alpha = D_00447260[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472C9 = 1;
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
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A6C; i++) {
            D_00425510[i] -= D_00447260[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446BB8 = 0;
                D_00446B20 = (void *)Menu_UpdateUseItem;
                ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                ((void (*)(s32))Menu_ChainItemListModels)(0); // match: the callee takes nothing
                if (D_004472C9 == 5) {
                    D_004472C5 = 3;
                }
                D_004472C9 = 0;
                D_00446BBC = 0;
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
    if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
        do {
            D_00446BC0 += LIST_PAGES + 1; // match: two statements; one expression computes the sum in another register
            D_00446BC0 %= LIST_PAGES;
            counter++; // match: counted before the test, so it sits in the branch's delay slot
            if (recollection_pages[D_00446BC0] != 0) {
                break;
            }
        } while (counter < LIST_PAGES);
        D_004472D9 = 0;
        if (D_00446BC4 != 0 || D_00446BC0 != 0) {
            D_004472D8 = 0;
        }
        if (D_00446BC4 != D_00446BC0) {
            Sound_PlayMenu(1);
        }
        D_00446BC4 = D_00446BC0;
    } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
        do {
            D_00446BC0 += LIST_PAGES - 1;
            D_00446BC0 %= LIST_PAGES;
            counter++; // match: counted before the test, so it sits in the branch's delay slot
            if (recollection_pages[D_00446BC0] != 0) {
                break;
            }
        } while (counter < LIST_PAGES);
        D_004472D9 = 0;
        if (D_00446BC4 != 0 || D_00446BC0 != 0) {
            D_004472D8 = 0;
        }
        if (D_00446BC4 != D_00446BC0) {
            Sound_PlayMenu(1);
        }
        D_00446BC4 = D_00446BC0;
    }

    /* the entries the page has: one bit each in its row of recollection_flags, from bit 3 on */
    for (i = 0; i < LIST_PAGES; i++) {
        rows[i] = -1;
    }
    row_count = 0;
    counter = 3;
    for (i = 3; i < LIST_PAGES - 1; i++) {
        if ((recollection_flags[D_00446BC0][i / 8] >> counter) & 1) {
            rows[row_count] = i;
            row_count++;
        }
        counter++;
        counter %= 8;
    }

    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        if (row_count >= 2) {
            Sound_PlayMenu(1);
        }
        if (D_004472D8 == D_004472D9 && row_count >= 8) {
            D_004472D9--;
            if (D_004472D9 < 0) {
                D_004472D9 = row_count - 1;
            }
        }
        D_004472D8--;
        if (D_004472D8 < 0) {
            D_004472D8 = row_count - 1;
        }
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        if (row_count >= 2) {
            Sound_PlayMenu(1);
        }
        if (((D_004472D9 < D_004472D8 && D_004472D8 - D_004472D9 >= 6) ||
             (D_004472D8 < D_004472D9 && D_004472D8 + row_count - D_004472D9 >= 6)) &&
            row_count >= 8) {
            D_004472D9++;
            if (D_004472D9 >= row_count) {
                D_004472D9 = 0;
            }
        }
        D_004472D8++;
        if (D_004472D8 >= row_count) {
            D_004472D8 = 0;
        }
    }

    for (i = 0; i < D_00446A54; i++) {
        Menu_SetBlendMode(D_00447250[i].unk42);
        MenuSprite_Draw((int)&D_00447250[i], 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    show_picture = (s8)game_options[7] != 0;
    if (D_005CEEE0.done != 0 && show_picture) {
        memset(view, 0, 0x40);
        view[10] = view[5] = view[0] = 0.8f;
        view[15] = 1.0f;
        sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        sceVu0TransMatrix(local, view, D_00421080);
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
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
        chunk = (PackageChunk *)((char *)D_005CEEE0.pool_node->buffer + 0x10);
        for (i = 0; i < D_00446BC0; i++) {
            chunk = (PackageChunk *)((char *)chunk + chunk->unk0C + chunk->unk08);
        }
        if (D_004472C6 != 2 && D_004472C6 != 3 && D_00426470.unk20 != 0) {
            D_00425510[254] += 12.8f;
            if (D_00425510[254] > 128.0f) {
                D_00425510[254] = 128.0f;
            }
            D_00446C88.alpha = (s32)D_00425510[254];
        }
        D_00426470.pool_node = &node; // match: a local of this function; the node keeps the address after it returns
        D_00426470.unk20 = 1;
        node.buffer = chunk;
        DRAW_NODE_MODEL(&D_00426470, local, D_00446C88);
    }

    table = Message_GetFontText(D_00446F38);
    first_text = D_00446BC0 * LIST_PAGES + MSG_RECOLLECTION_FIRST + 1;
    entry = GetListTextOffset(table, first_text + LIST_PAGES - 1);
    word.raw = 0x000A875081106E80;
    MenuText_SetPen(word);
    if (entry != 0) {
        text = table + entry;
        MenuText_SetColor(D_00446C78);
        MenuText_DrawLineCentered((u16 *)text, 0x17);
    }
    entry = GetListTextOffset(table, first_text + recollection_pages[D_00446BC0] - 1);
    word.raw = 0x000A875083106FE0;
    MenuText_SetPen(word);
    if (entry != 0) {
        text = table + entry;
        MenuText_SetColor(D_00446C78);
        MenuText_DrawWrappedLong((u16 *)text, 0x33);
    }

    for (i = 0; i < LIST_ROWS && i < row_count; i++) {
        counter = D_004472D9 + i;
        row = counter % row_count; // match: kept in a variable, which orders the compare's operands
        if (D_004472D8 == row) {
            bar.first.f.lo16 = bar_origin.f.lo16;
            bar.first.f.mid16 = bar_origin.f.mid16 + i * 0xB0;
            bar.first.f.mid24 = bar_origin.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1250;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_004472CA == 1) {
                D_00425510[253] = Menu_PulseValue(D_00425510[253], 0x10, 0x20);
            }
            bar.alpha = (s32)D_00425510[253];
            MenuDraw_Bar(&bar);
            break;
        }
    }

    word.raw = 0x000AAE607C307F80;
    MenuText_SetPen(word);
    for (i = 0; i < LIST_ROWS && i < row_count; i++) {
        counter = D_004472D9 + i;
        entry = ((s32 *)table + first_text)[rows[counter % row_count]]; // match: the page's first text is added to the table, not to the row
        if (entry != 0) {
            text = table + entry;
            MenuText_SetColor(D_00446C78);
            MenuText_DrawLineShortened((u16 *)text, 0x1C);
        }
    }

    if ((D_00446DE8 & 0x4000) && D_004472CA == 1 && D_004472DA != 0) {
        Sound_PlayMenu(0);
        D_00446B20 = (void *)Menu_UpdateRecollectionPage;
        context->unk04 = D_00446BC0 * LIST_PAGES + rows[D_004472D8] + MSG_RECOLLECTION_FIRST;
        D_004472DA = 0;
    } else if ((D_00446DE8 & 0x2000) && D_004472CA == 1 && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472DA = 0;
        D_004472CA = 2;
    }

    switch (D_004472CA) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[253] += 1.6f;
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446A54; i++) {
            D_00425510[i] += D_00447250[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447250[i].alpha) {
                for (i = 0; i < D_00446A54; i++) {
                    full_alpha = D_00447250[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472CA = 1;
                break;
            }
        }
        break;
    case 2:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[254] -= 12.8f;
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00446C88.alpha = (s32)D_00425510[254];
        D_00425510[253] -= 1.6f;
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A54; i++) {
            D_00425510[i] -= D_00447250[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_004472D9 = 0;
                D_00446B20 = (void *)Menu_OpenMain;
                D_004472D8 = 0;
                D_00446BC0 = 0;
                if (D_005CEEE0.done != 0) {
                    D_005CEEE0.done = 0;
                    D_005CEEE0.stage = 0;
                    D_00446C90 = NULL;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                } else {
                    sceCdBreak();
                    func_001024B0(D_00446AD0);
                    D_00446AD0 = -1;
                    D_005CEEE0.done = 0;
                    D_005CEEE0.stage = 0;
                    D_00446C90 = NULL;
                    ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                }
                D_004472CA = 0;
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
    for (i = 0; i < D_00446A54; i++) {
        kind = D_00447250[i].unk3E;
        Menu_SetBlendMode(D_00447250[i].unk42);
        MenuSprite_DrawPlain((int)&D_00447250[i], kind, 1);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }
    if (D_005CEEE0.done != 0) {
        memset(view, 0, 0x40);
        view[10] = view[5] = view[0] = 0.8f;
        view[15] = 1.0f;
        sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        sceVu0TransMatrix(local, view, D_00421090);
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
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
        chunk = (PackageChunk *)((char *)D_005CEEE0.pool_node->buffer + 0x10);
        for (i = 0; (s32)i < page; i++) {
            chunk = (PackageChunk *)((char *)chunk + chunk->unk0C + chunk->unk08);
        }
        D_00426470.pool_node = &model_node; // match: a local of this function; the node keeps the address after it returns
        D_00426470.unk20 = 1;
        model_node.buffer = chunk;
        DRAW_NODE_MODEL(&D_00426470, local, D_00446C78);
    }
    table = Message_GetFontText(D_00446F38);
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
    if (D_00446DE8 & 0x2000) {
        Sound_PlayMenu(2);
        context->unk04 = 0;
        D_00446B20 = (void *)Menu_UpdateRecollection;
        D_004472DA = 1;
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
    if (D_004472CB < 2) {
        ((void (*)(HandlerContext *))Menu_MoveAllItemsCursor)(context); // match: the callee takes nothing
    }
    selected = D_005CE850[0];
    item = D_005CE860[0];
    corner.raw = 0x0A09C4007C306FE0;
    if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
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
    if (D_004472CB == 2) {
        item = D_00447210;
    } else {
        D_00447214 = selected;
        D_00447210 = item;
    }

    record = D_005CE850[0]->unk0C - bases.first_item[D_005CE850[0]->group];
    D_00446DF4 = item_counts[D_005CE850[0]->unk0C];
    if (D_005CE850[0]->group == 0) {
        D_00446E30 = weapon_level_records[record].unk14 + 1;
    }
    for (i = 0; i < D_00446A58; i++) {
        kind = D_00447254[i].unk3E;
        Menu_SetBlendMode(D_00447254[i].unk42);
        if (D_005CE850[0]->group != 12) {
            MenuSprite_Draw((int)&D_00447254[i], kind + 1000, 0, (int)D_00425510[i]);
        } else {
            MenuSprite_Draw((int)&D_00447254[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    for (i = 0; i < 7; i++, item = item->prev) {
        if (item_name_defs[item->unk0C].state & 1) {
            worn_mask |= 1 << worn_bit;
        }
        worn_bit++;
        if (item == D_00447214) {
            bar.first.f.lo16 = corner.f.lo16;
            bar.first.f.mid16 = corner.f.mid16 + i * 0xB0;
            bar.first.f.mid24 = corner.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_004472CB == 1) {
                D_00425510[253] = Menu_PulseValue(D_00425510[253], 0x10, 0x20);
            }
            bar.alpha = (s32)D_00425510[253];
            MenuDraw_Bar(&bar);
        }
        if (D_005CE870[0]->prev == item) {
            break;
        }
    }

    node = D_005CE860[0];
    cursor = D_005CE850[0];
    position.raw = 0x0A09EB107C3071C0;
    if (D_004472CB == 2) {
        node = D_00447218;
    } else {
        D_0044721C = cursor;
        D_00447218 = node;
    }
    for (row = 0; row < 7; row++, node = node->prev) {
        def = &item_name_defs[node->unk0C];
        MenuText_SetPen(position);
        MenuText_SetColor(D_00446C78);
        MenuText_DrawLine((u16 *)def, 0x1A);
        position.f.mid16 += 0xB0;
        if (D_005CE870[0]->prev == node) {
            break;
        }
    }

    if (D_004472CB != 2) {
        node = D_005CE850[0];
        D_00447220 = node;
    } else {
        node = D_00447220;
    }
    texts = Message_GetFontText(D_00446F38);
    position.raw = 0x000A875083207080;
    text_index = node->unk0C + MSG_ITEM_TEXT_FIRST + 1;
    text_offset = ((s32 *)texts)[text_index];
    MenuText_SetPen(position);
    if (text_offset != 0) {
        text = texts + text_offset;
        MenuText_SetColor(D_00446C78);
        MenuText_DrawWordWrappedLong((u16 *)text, 0x33);
    }

    for (i = 0; i < D_00446A4C; i++) {
        kind = D_00447248[i].unk3E;
        if (kind != 2) {
            ((void (*)(u8))Menu_SetBlendMode)(D_00447248[i].unk42); // match: called as returning nothing
            if (D_004472CB != 1 && ((worn_mask >> D_00447248[i].slot) & 1)) {
                MenuSprite_Draw((int)&D_00447248[i], kind + 1000, worn_mask, (int)D_00425510[i + 200]);
            } else {
                MenuSprite_DrawPlain((int)&D_00447248[i], kind, worn_mask);
            }
            Menu_SetBlendMode(0);
        }
    }

    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    D_00446BD0 += 3.1415927f;
    D_00446BD0 -= (s32)(D_00446BD0 / 6.2831855f) * 6.2831855f;
    D_00446BD0 -= 3.1415927f;
    sceVu0RotMatrixY(local, view, D_00446BD0);
    sceVu0TransMatrix(local, local, D_004210A0);
    Menu_DrawItemModel(local);

    if ((D_00446DE8 & 0x1000) && D_004472DA != 0 && Menu_IsEquipSlotFilled(record, D_005CE850[0]->group)) {
        Sound_PlayMenu(1);
        if (D_00446AD4 == 0) {
            D_00446AD4 = 1;
        } else if (D_004472CC != 2) {
            D_004472CC = 2;
        } else {
            D_004472CC = 0;
        }
    }

    if (D_00446AD4 != 0) {
        // match: groups 1 and 10 have no case, so packets and count stay unset for the fade below
        switch (D_005CE850[0]->group) {
        case 11:
        case 12:
            packets = D_00447274;
            count = D_00446A88;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            D_00446DF8 = ScaleValue(worn_item_level_records[record].unk00[0] + worn_defs[record].unk00[0], worn_item_level_records[record].level, 100);
            D_00446DFC = ScaleValue(worn_item_level_records[record].unk00[1] + worn_defs[record].unk00[1], worn_item_level_records[record].level, 100);
            D_00446E00 = ScaleValue(worn_item_level_records[record].unk00[2] + worn_defs[record].unk00[2], worn_item_level_records[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[3], worn_item_level_records[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[4], worn_item_level_records[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[5], worn_item_level_records[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[6], worn_item_level_records[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[7], worn_item_level_records[record].level, 100);
            D_00446E18 = ScaleValue(worn_defs[record].unk00[8], worn_item_level_records[record].level, 100);
            D_00446E24 = worn_item_level_records[record].level;
            D_00446E28 = 100;
            D_00446E1C = item_name_defs[D_005CE850[0]->unk0C].unk3E;
            if (D_004472CC == 1) {
                FILL_FADE(D_00425910, D_00447274, D_00446A88)
            }
            DRAW_PACKETS(D_00447274, D_00446A88, player_weapon[D_005CE850[0]->group])
            packets = D_00447274;
            count = D_00446A88;
            break;
        case 7:
        case 8:
        case 9:
            D_00446DF8 = ScaleValue(worn_item_level_records[record].unk00[0] + worn_defs[record].unk00[0], worn_item_level_records[record].level, 100);
            D_00446DFC = ScaleValue(worn_item_level_records[record].unk00[1] + worn_defs[record].unk00[1], worn_item_level_records[record].level, 100);
            D_00446E00 = ScaleValue(worn_item_level_records[record].unk00[2] + worn_defs[record].unk00[2], worn_item_level_records[record].level, 100);
            D_00446E04 = ScaleValue(worn_defs[record].unk00[3], worn_item_level_records[record].level, 100);
            D_00446E08 = ScaleValue(worn_defs[record].unk00[4], worn_item_level_records[record].level, 100);
            D_00446E0C = ScaleValue(worn_defs[record].unk00[5], worn_item_level_records[record].level, 100);
            D_00446E10 = ScaleValue(worn_defs[record].unk00[6], worn_item_level_records[record].level, 100);
            D_00446E14 = ScaleValue(worn_defs[record].unk00[7], worn_item_level_records[record].level, 100);
            D_00446E18 = ScaleValue(worn_defs[record].unk00[8], worn_item_level_records[record].level, 100);
            D_00446E1C = item_name_defs[D_005CE850[0]->unk0C].unk3E;
            if (D_004472CC == 1) {
                FILL_FADE(D_00425910, D_00447278, D_00446A8C)
            }
            DRAW_PACKETS(D_00447278, D_00446A8C, player_weapon[D_005CE850[0]->group])
            packets = D_00447278;
            count = D_00446A8C;
            break;
        case 0:
            D_00446DF8 = ScaleValue(weapon_level_records[record].unk00[0] + weapon_defs[record].unk00[weapon_level_records[record].unk14].values[0], weapon_level_records[record].level, 100);
            D_00446DFC = ScaleValue(weapon_level_records[record].unk00[1] + weapon_defs[record].unk00[weapon_level_records[record].unk14].values[1], weapon_level_records[record].level, 100);
            D_00446E00 = ScaleValue(weapon_level_records[record].unk00[2] + weapon_defs[record].unk00[weapon_level_records[record].unk14].values[2], weapon_level_records[record].level, 100);
            D_00446E04 = ScaleValue(weapon_defs[record].unk00[weapon_level_records[record].unk14].values[3], weapon_level_records[record].level, 100);
            D_00446E08 = ScaleValue(weapon_defs[record].unk00[weapon_level_records[record].unk14].values[4], weapon_level_records[record].level, 100);
            D_00446E0C = ScaleValue(weapon_defs[record].unk00[weapon_level_records[record].unk14].values[5], weapon_level_records[record].level, 100);
            D_00446E10 = ScaleValue(weapon_defs[record].unk00[weapon_level_records[record].unk14].values[6], weapon_level_records[record].level, 100);
            D_00446E14 = ScaleValue(weapon_defs[record].unk00[weapon_level_records[record].unk14].values[7], weapon_level_records[record].level, 100);
            D_00446E18 = ScaleValue(weapon_defs[record].unk00[weapon_level_records[record].unk14].values[8], weapon_level_records[record].level, 100);
            D_00446E20 = (s32)(weapon_defs[record].unk68 * 0.03281f * 10.0f);
            D_00446E24 = weapon_level_records[record].level;
            D_00446E1C = item_name_defs[D_005CE850[0]->unk0C].unk3E;
            D_00446E28 = player_weapon[D_005CE850[0]->group];
            D_00446E2C = 100;
            if (D_004472CC == 1) {
                FILL_FADE(D_00425910, D_00447270, D_00446A84)
            }
            DRAW_PACKETS(D_00447270, D_00446A84, 0)
            packets = D_00447270;
            count = D_00446A84;
            break;
        }

        switch (D_004472CC) {
        case 0:
            D_00425910[255] += 12.8f;
            if (D_00425910[255] > 128.0f) {
                D_00425910[255] = 128.0f;
            }
            D_00446C80.alpha = (s32)D_00425910[255];
            for (i = 0; i < count; i++) {
                D_00425910[i] += (f32)packets[i].alpha / 10.0f;
                if ((f32)packets[i].alpha < D_00425910[i]) {
                    FILL_FADE(D_00425910, packets, count)
                    D_004472CC = 1;
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
                D_00425910[i] -= (f32)packets[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400); // match: the panel's state is left at 2 and D_00446AD4 set
                    break;
                }
            }
            break;
        }
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472CB = 2;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472CB == 2 && D_00446C90 == NULL) {
        D_004472CB = 3;
    }

    switch (D_004472CB) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[253] += 1.6f;
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446A4C; i++) {
            D_00425510[i + 200] += (f32)D_00447248[i].alpha / 10.0f;
        }
        for (i = 0; i < D_00446A58; i++) {
            D_00425510[i] += (f32)D_00447254[i].alpha / 10.0f;
            if ((f32)D_00447254[i].alpha < D_00425510[i]) {
                FILL_FADE(D_00425510, D_00447254, D_00446A58)
                for (i = 0; i < D_00446A4C; i++) {
                    D_00425510[i + 200] = SPRITE_PACKET_COLOR(&D_00447248[i])->alpha;
                }
                D_004472DA = D_004472CB = 1;
                break;
            }
        }
        break;
    case 3:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[253] -= 1.6f;
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A4C; i++) {
            D_00425510[i + 200] -= (f32)D_00447248[i].alpha / 10.0f;
        }
        for (i = 0; i < D_00446A58; i++) {
            D_00425510[i] -= (f32)D_00447254[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                Menu_BuildItemRings();
                D_00446B20 = (void *)Menu_OpenMain;
                ((void (*)(s32))Menu_ResetLoadBuffers)(0); // match: the callee takes nothing
                D_004472CB = 0;
                D_004472CC = 0;
                D_00446AD4 = 0;
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

    if (D_00446C70 != 4) {
        if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_004472E8--;
        } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_004472E8++;
        }
    } else {
        if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_00446BD8--;
        } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
            PLAY_MENU_SOUND(1);
            D_00446BD8++;
        }
        if (D_00446BD8 < 0) {
            D_00446BD8 = 1;
        } else if (D_00446BD8 >= 2) {
            D_00446BD8 = 0;
        }
    }
    D_004472E8 = (D_004472E8 + 5) % line_count;

    for (i = 0; i < D_00446A5C; i++) {
        kind = D_00447258[i].unk3E;
        SET_PART_MODE(D_00447258[i].unk42);
        if (D_00446C70 == 4 && kind == 2) {
            MenuSprite_Draw((int)&D_00447258[i], 0x3EA, 0, D_00425510[i]);
        } else if (kind != 2) {
            if (D_00446C70 != 1) {
                MenuSprite_Draw((int)&D_00447258[i], kind + 1000, D_004472E8, D_00425510[i]);
            } else {
                MenuSprite_Draw((int)&D_00447258[i], kind, D_004472E8, D_00425510[i]);
            }
        }
        SET_PART_MODE(0);
    }

    if (D_00446C70 == 4) {
        bar.first.f.lo16 = 0x7710;
        bar.first.f.mid16 = D_00446BD8 * 0xC0 + 0x8010;
        bar.first.f.mid24 = 0x86470;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1250;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xC0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFD] = Menu_PulseValue(D_00425510[0xFD], 0x10, 0x20);
        bar.alpha = D_00425510[0xFD];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    report = Pad_GetReport(0, 0);
    if ((report->pressed & 0x4000) && D_004472D0 != 0xFF && D_004472DA != 0) {
        if (D_00446C70 != 4) {
            PLAY_MENU_SOUND(0);
            switch (D_004472E8) {
            case 0:
                D_005CF2F0.unk16 = 0;
                D_005CF2F0.unk10 = 0;
                D_005CF2F0.unk11 = 0;
                D_005CF2F0.unk14 = 0;
                D_005CF2F0.unk0C = 0;
                D_005CF2F0.unk0D = 0;
                D_005CF2F0.unk18 = 1;
                D_005CF2F0.unk1B = 0;
                D_005CF2F0.unk1E = 0;
                D_005CF2F0.unk1F = 0;
                D_005CF2F0.unk20 = 0;
                D_005CF2F0.unk00 = 0;
                D_005CF2F0.unk0E = 1;
                D_005CF2F0.unk12 = -1;
                SaveCard_Open();
                SaveCard_StartCheck(1);
                D_00446C70 = 2;
                D_004472DA = 0;
                for (i = 0; i < D_00446A5C; i++) {
                    if (D_00447258[i].unk3E == 1 && D_004472E8 == D_00447258[i].slot) {
                        D_00425510[i] = D_00446C04;
                    }
                }
                break;
            case 1:
            case 2:
            case 3:
                for (i = 0; i < D_00446A5C; i++) {
                    if (D_00447258[i].unk3E == 1 && D_004472E8 == D_00447258[i].slot) {
                        D_00425510[i] = D_00446C04;
                    }
                }
                D_004472DA = 0;
                D_00446C70 = 2;
                break;
            case 4:
                for (i = 0; i < D_00446A5C; i++) {
                    if (D_00447258[i].unk3E == 1 && D_004472E8 == D_00447258[i].slot) {
                        D_00425510[i] = D_00446C04;
                    }
                }
                D_00446C70 = 4;
                break;
            }
        } else if (D_00446BD8 == 0) {
            PLAY_MENU_SOUND(0);
            D_004472DA = 0;
            D_00446C70 = 5;
        } else {
            PLAY_MENU_SOUND(2);
            D_00446C70 = 1;
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        PLAY_MENU_SOUND(2);
        if (D_00446C70 != 4) {
            D_004472DA = 0;
            D_00446C70 = 3;
        } else {
            D_00446C70 = 1;
            D_00446BD8 = 1;
        }
    }

    if (game_status.bit9) {
        context->unk04 = 0;
        D_004472E8 = 0;
        D_00446B20 = (void *)Menu_OpenMain;
    }

    switch (D_00446C70) {
    case 0:
        D_00425510[0xFD] += 1.6f;
        if (D_00425510[0xFD] > 16.0f) {
            D_00425510[0xFD] = 16.0f;
        }
        for (i = 0; i < D_00446A5C; i++) {
            D_00425510[i] += SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha / 10.0f;
            if (SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A5C; i++) {
                    D_00425510[i] = SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha;
                }
                D_00446C70 = 1; // match: before D_004472DA, or every register of the step stores moves
                D_004472DA = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 5:
    case 6:
        D_00425510[0xFD] -= 1.6f;
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A5C; i++) {
            D_00425510[i] -= SPRITE_PACKET_COLOR_VIEW(&D_00447258[i])->alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                D_00425510[i] = 0.0f;
                if (D_00447258[i].unk3E != 1 || D_004472E8 != D_00447258[i].slot) {
                    memset(D_00425510, 0, 0x400);
                    if (D_00446C70 == 2) {
                        D_004472CF = D_0034968A[0];
                        D_00446C04 = 128.0f;
                        D_00446B20 = system_menu_screens[D_004472E8];
                        D_00446C09 = 0;
                        D_00446C08 = 0;
                        if (D_004472E8 == 2) {
                            D_005CF318[0] = D_0044692C;
                            D_005CF318[1] = D_0044692E;
                            D_005CF318[2] = D_00446932;
                            D_005CF318[3] = D_00446930;
                            D_005CF318[4] = D_00446936;
                            D_005CF318[5] = D_00446934;
                            D_005CF318[6] = D_0044693C;
                            D_005CF318[7] = D_0044693E;
                            D_005CF318[8] = D_00446940;
                            D_005CF318[9] = D_00446944;
                            D_005CF318[10] = D_00446938;
                            D_005CF318[11] = D_0044693A;
                            D_005CF318[12] = D_00446942;
                            Menu_ReadButtonSettings();
                        }
                    } else if (D_00446C70 == 3) {
                        context->unk04 = 0;
                        D_004472E8 = 0;
                        D_00446B20 = (void *)Menu_OpenMain;
                    } else if (D_00446C70 == 5) {
                        D_00446AC8++;
                        context->unk04 = 0;
                        D_004472E8 = 0;
                        game_status.bit9 = 1;
                    }
                    D_00446C70 = 0;
                    D_00446BD8 = 1;
                    break;
                }
            }
        }
        break;
    }
}

/* Moves the cursor of the item list (D_00426110) a row or a page, unless the list is locked, shifting the models
   with it. Returns TRUE when a move button was pressed. */
s32 Menu_MoveItemListCursor(HandlerContext *context, s32 locked) {
    MenuItem *cursor = D_00426110[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if ((D_00446DE8 & 0x10) && !locked) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == D_00426120[0] && next != D_00426130[0]->prev) {
                    D_00426120[0] = next;
                    D_00426130[0] = D_00426130[0]->next;
                }
                D_00426110[0] = next;
                Sound_PlayMenu(1);
            }
            Menu_ShiftListModelsNext(0);
        } else if ((D_00446DE8 & 0x40) && !locked) {
            if (cursor->prev != NULL) {
                if (D_00426130[0]->prev == D_00426110[0] && D_00426120[0]->next != D_00426130[0]->prev) {
                    D_00426130[0] = D_00426130[0]->prev;
                    D_00426120[0] = D_00426120[0]->prev;
                }
                Sound_PlayMenu(1);
                D_00426110[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((D_00446DE8 & 0x20) && D_00446C48 >= 8 && !locked) {
            if (cursor->prev != NULL) {
                if (D_00426120[0]->next != D_00426130[0]->prev) {
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
        } else if ((D_00446DE8 & 0x80) && D_00446C48 >= 8 && !locked) {
            if (cursor->next != NULL) {
                if (D_00426120[0]->next != D_00426130[0]->prev) {
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

/* Moves the cursor of the spell list (D_005CE880) a row, shifting the models with it. Returns 1 when a move
   button was pressed. */
static int Menu_MoveSpellCursor(void) {
    MenuItem *cur = D_005CE880[0];
    MenuItem *n;

    if (cur != NULL) {
        if (D_00446DE8 & 0x10) {
            n = cur->next;
            if (n != NULL) {
                if (cur == D_005CE890[0] && n != D_005CE8A0[0]->prev) {
                    D_005CE890[0] = n;
                    D_005CE8A0[0] = D_005CE8A0[0]->next;
                }
                Sound_PlayMenu(1);
                D_005CE880[0] = cur->next;
            }
            Menu_ShiftSpellModelsNext(0);
        } else if (D_00446DE8 & 0x40) {
            if (cur->prev != NULL) {
                if (cur == D_005CE8A0[0]->prev && D_005CE890[0]->next != cur) {
                    D_005CE8A0[0] = cur;
                    D_005CE890[0] = D_005CE890[0]->prev;
                }
                Sound_PlayMenu(1);
                D_005CE880[0] = cur->prev;
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

    if (D_005CE840[0] != NULL) {
        item = D_005CE840[0]->item;
    }
    if (item != NULL) {
        if (D_00446DE8 & 0x80) {
            MenuItem *next = item->next;
            if (next != NULL) {
                D_005CE840[0]->item = next;
                Menu_ShiftEquipModelsNextItem(0, item);
                Sound_PlayMenu(1);
                return 1;
            }
        }
        if (D_00446DE8 & 0x20) {
            MenuItem *prev = item->prev;
            if (prev != NULL) {
                D_005CE840[0]->item = prev;
                Menu_ShiftEquipModelsPrevItem(0, item);
                Sound_PlayMenu(1);
                return 1;
            }
        }
    }
    return 0;
}

/* Moves the cursor of the list of everything owned (D_005CE850) one row (0x10, 0x40) or one page (0x20, 0x80,
   lists of 8 and more), scrolling the window and shifting the models. */
static s32 Menu_MoveAllItemsCursor(void) {
    MenuItem *cursor = D_005CE850[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if (D_00446DE8 & 0x10) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == D_005CE860[0] && next != D_005CE870[0]->prev) {
                    D_005CE860[0] = next;
                    D_005CE870[0] = D_005CE870[0]->next;
                }
                Sound_PlayMenu(1);
                D_005CE850[0] = cursor->next;
            }
            Menu_ShiftListModelsNext(0);
        } else if (D_00446DE8 & 0x40) {
            if (cursor->prev != NULL) {
                if (D_005CE870[0]->prev == cursor && D_005CE860[0]->next != cursor) {
                    D_005CE870[0] = cursor;
                    D_005CE860[0] = D_005CE860[0]->prev;
                }
                Sound_PlayMenu(1);
                D_005CE850[0] = cursor->prev;
            }
            Menu_ShiftListModelsPrev(0);
        } else if ((D_00446DE8 & 0x20) && D_00446C38 >= 8) {
            if (cursor->prev != NULL) {
                if (D_005CE860[0]->next != D_005CE870[0]->prev) {
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
        } else if ((D_00446DE8 & 0x80) && D_00446C38 >= 8) {
            if (cursor->next != NULL) {
                if (D_005CE860[0]->next != D_005CE870[0]->prev) {
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
    if (D_005CF2F0.unk1B == 0 && SaveCard_IsChecking() == 0) {
        D_005CF2F0.unk18 = 0;
        D_005CF2F0.unk1B = 1;
        if (SaveCard_IsPresent(0) == 0 && SaveCard_IsPresent(1) != 0) {
            D_005CF2F0.unk0C = 1;
        }
    }

    for (i = 0; i < D_00446A60; i++) {
        kind = D_00446BF4[i].unk3E;
        Menu_SetBlendMode(D_00446BF4[i].unk42);
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 1, D_00425510[i]);
            break;
        case 29:
            if ((u8)D_00446BF4[i].slot < 2) {
                MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, D_00446BF4[i].slot, D_00425510[i]);
            }
            break;
        case 3:
            switch (D_005CF2F0.unk12) {
            case 4:
            case 5:
            case 9:
            case 10:
                MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
            break;
        case 31:
            if (D_005CF2F0.unk1B != 0) {
                if (D_00446C70 != 1) {
                    MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, D_005CF2F0.unk0C, D_00425510[i]);
                } else {
                    MenuSprite_Draw((int)&D_00446BF4[i], kind, D_005CF2F0.unk0C, D_00425510[i]);
                }
            }
            break;
        case 26:
            if (SaveCard_IsPresent(0) == 0) {
                if (D_00446C70 == 0) {
                    MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0x20, D_00425510[i]);
                } else if (D_00446C70 < 2) {
                    if (D_00446C94 != 1 || D_00446BDC == 0) {
                        MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 0x20);
                    }
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
                    if (D_00446C94 != 1 || D_00446BDC == 0) {
                        MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 0x20);
                    }
                }
            } else if (D_00446C70 != 1) {
                MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0x80, D_00425510[i]);
            } else {
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 0x80);
            }
            break;
        case 28:
            switch (D_005CF2F0.unk12) {
            case 4:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 4);
                break;
            case 5:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 5);
                break;
            case 9:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 9);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&D_00446BF4[i], kind, 10);
                break;
            }
            /* fallthrough */
        default:
            MenuSprite_Draw((int)&D_00446BF4[i], kind + 1000, 0, D_00425510[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    if (D_005CF2F0.unk14 != 0) {
        if (SaveCard_CheckBusy() == 0) {
            D_00447224 = SaveCard_GetSaveMask();
            if (D_00447224 != 0) {
                D_005CF2F0.unk18 = 1;
                D_00446C70 = 2;
                D_005CF2F0.unk10 = 1;
            } else {
                D_005CF2F0.unk12 = 9;
            }
            D_005CF2F0.unk14 = 0;
        } else if (card_result < 0) {
            if (SaveCard_IsPresent(D_005CF2F0.unk0C) != 0) {
                D_005CF2F0.unk12 = 9;
            } else {
                D_005CF2F0.unk12 = 10;
            }
            D_005CF2F0.unk14 = 0;
            card_result = 0;
            SaveCard_StartCheck(1);
        }
    }

    if (D_005CF2F0.unk11 != 0) {
        if (card_result < 0) {
            D_005CF2F0.unk18 = 1;
            D_00446C70 = 3;
        }
    } else if (card_result < 0) {
        D_005CF2F0.unk14 = 0;
        D_005CF2F0.unk12 = 10;
        SaveCard_StartCheck(1);
    } else if (D_005CF2F0.unk14 == 0 && D_005CF2F0.unk18 == 0) {
        if ((D_00446DE8 & 0xA0) && D_004472D0 != 0xFF && D_005CF2F0.unk12 == -1) {
            Sound_PlayMenu(1);
            D_005CF2F0.unk0C = (D_005CF2F0.unk0C + 1) & 1;
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & 0x4000) && D_004472D0 != 0xFF) {
                if (D_005CF2F0.unk12 != -1) {
                    Sound_PlayMenu(0);
                    D_005CF2F0.unk12 = -1;
                } else if (SaveCard_IsPresent(D_005CF2F0.unk0C) != 0) {
                    if (SaveCard_CheckSavesIntact() > 0) {
                        if (SaveCard_StartReadList(D_005CF2F0.unk0C, 1) == 0) {
                            Sound_PlayMenu(0);
                            D_005CF2F0.unk14 = 1;
                        }
                    } else {
                        D_005CF2F0.unk12 = 9;
                    }
                } else if (D_005CF2F0.unk0C == 0) {
                    Sound_PlayMenu(0);
                    D_005CF2F0.unk12 = 4;
                } else if (D_005CF2F0.unk0C == 1) {
                    Sound_PlayMenu(0);
                    D_005CF2F0.unk12 = 5;
                }
            } else {
                report = Pad_GetReport(0, 0);
                if (((report->pressed & 0x2000) && D_004472D0 != 0xFF) || game_status.bit9) {
                    Sound_PlayMenu(2);
                    if (D_005CF2F0.unk12 != -1) {
                        D_005CF2F0.unk12 = -1;
                    } else {
                        D_005CF2F0.unk11 = 1;
                        SaveCard_Close();
                    }
                }
            }
        }
    }

    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446A60; i++) {
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
                if (D_005CF2F0.unk1B != 0) {
                    D_005CF2F0.unk18 = 0;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446A60; i++) {
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
                if (D_00446C94 == 1) {
                    D_00446BDC++;
                } else {
                    D_00446BDC = 2;
                }
                if (D_00446BDC == 2) {
                    D_00446C04 = 128.0f;
                    D_00446BDC = 0;
                    D_00446C08 = 0;
                    if (D_00446C70 == 2) {
                        D_005CF2F0.unk13 = -1;
                        D_00446B20 = (void *)Menu_UpdateSaveList;
                        D_00446B24 = (void *)Menu_UpdateCardSlot;
                        D_005CF2F0.unk0D = 0;
                        D_005CF2F0.unk1D = 0;
                        SaveCard_ClearLoadFailed(0);
                        SaveCard_ClearLoadFailed(1);
                    } else if (D_00446C70 == 3) {
                        if (D_00446C94 != 1) {
                            D_00446B20 = (void *)Menu_UpdateSystem;
                        } else {
                            D_00446AC8++;
                        }
                    }
                    D_005CF2F0.unk12 = -1;
                    D_00446C70 = 0;
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
    info[1] = (CardSaveInfo *)SaveCard_GetSaveInfo(1);
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
    info[2] = (CardSaveInfo *)SaveCard_GetSaveInfo(2);
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

    if (D_005CF2F0.unk13 == -1) {
        for (i = 0; i < 3; i++) {
            MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] = SaveCard_GetSaveState(i);
        }
        for (i = 0; i < 3; i++) {
            if (MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] != 1) {
                switch (i) {
                case 0:
                    if (D_005CF2F0.unk1F != 0 || D_005CF2F0.unk20 != 0) {
                        first_arg = 1;
                        second_arg = 0;
                    } else {
                        first_arg = 0;
                        second_arg = 1;
                    }
                    break;
                case 1:
                    if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk20 != 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk20 == 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk20 != 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk20 == 0) {
                        second_arg = 1;
                        first_arg = 1;
                    }
                    break;
                case 2:
                    if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk1F != 0) {
                        second_arg = 0;
                        first_arg = 1;
                    } else if (D_005CF2F0.unk1E != 0 && D_005CF2F0.unk1F == 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk1F != 0) {
                        second_arg = 0;
                        first_arg = 2;
                    } else if (D_005CF2F0.unk1E == 0 && D_005CF2F0.unk1F == 0) { // match: a last test, not an else: retail keeps a path that sets neither
                        second_arg = 1;
                        first_arg = 2;
                    }
                    break;
                }
                if (SaveCard_HasRoomFor(D_005CF2F0.unk0C, first_arg, second_arg) > 0) {
                    MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] = 0;
                } else {
                    MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[i] = -1;
                }
            }
        }
    }

    for (i = 0; i < D_00446A64; i++) {
        kind = D_00446BF8[i].unk3E;
        slot = D_00446BF8[i].slot;
        Menu_SetBlendMode(D_00446BF8[i].unk42);
        switch (kind) {
        case 2:
            MenuSprite_Draw((int)&D_00446BF8[i], 0x3EA, 1, D_00425510[i]);
            break;
        case 3:
            group = slot / 3;
            if (MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[group] == 1) {
                MenuSprite_Draw((int)&D_00446BF8[i], 0x3EB, group * 3, D_00425510[i]);
            }
            break;
        case 25:
            if (MENU_SCREEN_SAVES(&D_005CF2F0)->save_state[slot] == 1) {
                MenuSprite_Draw((int)&D_00446BF8[i], 0x401, D_005CF2F0.unk0D, D_00425510[i]);
            }
            break;
        default:
            MenuSprite_Draw((int)&D_00446BF8[i], kind + 1000, D_005CF2F0.unk0D, D_00425510[i]);
            break;
        }
        SET_PART_MODE(0);
    }

    table = Message_GetFontText(D_00446F38);
    word.raw = 0x000A87507C107120;
    for (i = 0; i < 3; i++) {
        entry = ((s32 *)table)[info[i]->unk04 + 1];
        MenuText_SetPen(word);
        MenuText_SetColor(D_00446C78);
        if (entry != 0) {
            MenuText_DrawWrapped((u16 *)(table + entry), 0x18);
        }
        word.f.mid16 += 0x340;
    }

    for (i = 0; i < D_00446AAC; i++) {
        kind = D_00446C58[i].unk3E;
        SET_PART_MODE(D_00446C58[i].unk42);
        if (kind == 3) {
            switch (D_005CF2F0.unk13) {
            case 8:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 8);
                break;
            case 1:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 1);
                break;
            case 3:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 3);
                break;
            case 10:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 10);
                break;
            case 23:
                MenuSprite_DrawPlain((int)&D_00446C58[i], kind, 23);
                break;
            }
        } else {
            switch (D_005CF2F0.unk13) {
            case 1:
            case 3:
            case 8:
            case 10:
            case 23:
                MenuSprite_Draw((int)&D_00446C58[i], kind + 1000, 0, D_00425510[i]);
                break;
            }
        }
        SET_PART_MODE(0);
    }

    if (D_005CF2F0.unk13 == 8) {
        bar.first.f.lo16 = 0x7490;
        bar.first.f.mid16 = D_005CF2F0.unk0E * 0xC0 + 0x80A0;
        bar.first.f.mid24 = 0x5F370;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x1720;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        D_00425510[0xFD] = Menu_PulseValue(D_00425510[0xFD], 0x10, 0x20);
        bar.alpha = D_00425510[0xFD];
        MenuDraw_Bar(&bar); // match: the float at bar+0x24 that MenuDraw_Bar copies is never set
    }

    D_005CF2F0.unk00 = SaveCard_Update();
    if (SaveCard_HasCard(D_005CF2F0.unk0C) == 0) {
        D_005CF2F0.unk00 = -1;
    }
    if (D_005CF2F0.unk13 == 1 && SaveCard_CheckBusy() == 0) {
        D_005CF2F0.unk13 = 3;
    }
    if (D_005CF2F0.unk00 < 0) {
        if (SaveCard_GetLoadFailed(D_005CF2F0.unk0C) != 0 && !game_status.bit9) {
            D_005CF2F0.unk13 = 23;
            SaveCard_ClearLoadFailed(D_005CF2F0.unk0C);
            SaveCard_StartCheck(0);
        } else if (D_005CF2F0.unk13 != 3) {
            if (!game_status.bit9) {
                D_005CF2F0.unk13 = 10;
            }
        }
    }
    if (game_status.bit9) {
        if (D_005CF2F0.unk13 == -1 && D_00446BDD == 0) {
            SaveCard_Init();
            D_005CF2F0.unk18 = 1;
            D_00446C70 = 2;
            D_00446BDD = 1;
        }
    }

    report = Pad_GetReport(0, 0);
    if ((report->pressed & 0x4000) && D_004472D0 != 0xFF) {
        if (D_005CF2F0.unk13 != 1) {
            report = Pad_GetReport(0, 0);
            if (report->held & 0x50) {
                D_005CF2F0.unk1D = 1;
            } else {
                D_005CF2F0.unk1D = 3;
            }
        }
    }
    if (D_005CF2F0.unk1D == 2) {
        D_005CF2F0.unk1D = 3;
    }
    if (D_005CF2F0.unk1D == 1) {
        report = Pad_GetReport(0, 0);
        if (report->held & 0x50) {
            if (D_00446DE8 & 0x50) {
                D_005CF2F0.unk1D = 2;
            }
        } else {
            D_005CF2F0.unk1D = 3;
        }
    }

    if (D_005CF2F0.unk13 != 1 && D_005CF2F0.unk18 == 0) {
        if (D_005CF2F0.unk1D == 3) {
            D_005CF2F0.unk1D = 0;
            switch (D_005CF2F0.unk13) {
            case -1:
                Sound_PlayMenu(0);
                D_005CF2F0.unk13 = 8;
                break;
            case 8:
                if (D_005CF2F0.unk0E == 0) {
                    Sound_PlayMenu(0);
                    D_005CF2F0.unk13 = 1;
                    memset(save_image, 0, 0x20000);
                    SaveCard_StartLoad(D_005CF2F0.unk0C, D_005CF2F0.unk0D, (s32 *)save_image, 0x20000);
                } else {
                    Sound_PlayMenu(2);
                    D_005CF2F0.unk13 = -1;
                    D_005CF2F0.unk0E = 1;
                }
                break;
            case 3:
                Sound_PlayMenu(0);
                D_005CF2F0.unk13 = -1;
                break;
            case 10:
                Sound_PlayMenu(0);
                D_005CF2F0.unk18 = 1;
                if (!game_status.bit9) {
                    D_00446C70 = 3;
                    SaveCard_ResetCard(D_005CF2F0.unk0C);
                } else {
                    D_005CF2F0.unk13 = -1;
                    D_00446C70 = 2;
                    D_00446BDD = 1;
                }
                break;
            case 23:
                Sound_PlayMenu(0);
                D_005CF2F0.unk13 = -1;
                D_005CF2F0.unk0E = 1;
                break;
            }
        } else {
            report = Pad_GetReport(0, 0);
            if ((report->pressed & 0x2000) && D_004472D0 != 0xFF && D_005CF2F0.unk1D == 0) {
                Sound_PlayMenu(2);
                switch (D_005CF2F0.unk13) {
                case 10:
                    D_005CF2F0.unk18 = 1;
                    if (!game_status.bit9) {
                        D_00446C70 = 3;
                        SaveCard_ResetCard(D_005CF2F0.unk0C);
                    } else {
                        D_005CF2F0.unk13 = -1;
                        D_00446C70 = 2;
                        D_00446BDD = 1;
                    }
                    break;
                case -1:
                    D_005CF2F0.unk18 = 1;
                    D_00446C70 = 3;
                    break;
                case 3:
                    D_005CF2F0.unk13 = -1;
                    break;
                case 8:
                case 23:
                    D_005CF2F0.unk13 = -1;
                    D_005CF2F0.unk0E = 1;
                    break;
                }
            } else if ((D_00446DE8 & 0x40) && D_005CF2F0.unk18 == 0) {
                switch (D_005CF2F0.unk13) {
                case -1:
                    D_005CF2F0.unk0D++;
                    for (i = 0; i < 3; i++) {
                        if (SaveCard_GetSaveState((D_005CF2F0.unk0D + i) % 3) != 0) {
                            break;
                        }
                    }
                    if (i != 2) {
                        Sound_PlayMenu(1);
                    }
                    D_005CF2F0.unk0D = (D_005CF2F0.unk0D + i) % 3;
                    break;
                case 8:
                    Sound_PlayMenu(1);
                    D_005CF2F0.unk0E = (D_005CF2F0.unk0E + 1) & 1;
                    break;
                }
            } else if ((D_00446DE8 & 0x10) && D_005CF2F0.unk18 == 0) {
                switch (D_005CF2F0.unk13) {
                case -1:
                    D_005CF2F0.unk0D--;
                    for (i = 0; i < 3; i++) {
                        if (SaveCard_GetSaveState((D_005CF2F0.unk0D - i + 3) % 3) != 0) {
                            break;
                        }
                    }
                    if (i != 2) {
                        Sound_PlayMenu(1);
                    }
                    D_005CF2F0.unk0D = (D_005CF2F0.unk0D - i + 3) % 3;
                    break;
                case 8:
                    Sound_PlayMenu(1);
                    D_005CF2F0.unk0E = (D_005CF2F0.unk0E + 1) & 1;
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
                D_005CF2F0.unk18 = 0;
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
                D_00446BDD++;
                D_00446C08 = 0;
                D_00446C04 = 128.0f;
                if (D_00446C70 == 2) {
                    if (D_00446BDD != 2) {
                        D_005CF2F0.unk16 = 1;
                        D_00446C70 = 0;
                        D_00446B20 = (void *)Menu_UpdateCardSlot;
                        D_00446BDD = 0;
                        MenuHeap_FreeToMark();
                        D_00446AC8++;
                    }
                } else if (D_00446C70 == 3) {
                    D_005CF2F0.unk0E = 1;
                    D_00446B20 = (void *)Menu_UpdateCardSlot;
                    D_00446C70 = 0;
                    D_00446BDD = 0;
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

    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        context->unk04--;
        Sound_PlayMenu(1);
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        context->unk04++;
        Sound_PlayMenu(1);
    }
    context->unk04 += 7;
    context->unk04 %= 7;

    switch (context->unk04) {
    case 0:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            Sound_PlayMenu(1);
            if (D_00446BE4 != 1) {
                D_00446BE0 = game_options[0];
            }
            D_00446BE4 = 1;
            game_options[0]++;
            game_options[0] %= 2;
        } else if (D_00446BE4 == 1 && !(D_00446DE8 & 0xA0) && D_004472DA != 0) {
            if (game_options[0] == 0) {
                if (D_00446BE0 != 0) {
                    D_00446BE4 = 2;
                    func_001CBED8();
                } else {
                    D_00446BE4 = 4;
                }
            } else {
                if (D_00446BE0 == 0) {
                    D_00446BE4 = 3;
                    func_001CC018();
                } else {
                    D_00446BE4 = 4;
                }
            }
        }
        break;
    case 1:
        if ((D_00446DE8 & 0x20) && game_options[1] < 0x7F && D_004472DA != 0) {
            game_options[1] += 4;
            Sound_PlayMenu(1);
        } else if ((D_00446DE8 & 0x80) && game_options[1] != 0 && D_004472DA != 0) {
            game_options[1] -= 4;
            Sound_PlayMenu(1);
        }
        Sound_SetBgmVolume(game_options[1]);
        Sound_SyncBgmLevel();
        break;
    case 2:
        if ((D_00446DE8 & 0x20) && game_options[2] < 0x7F && D_004472DA != 0) {
            game_options[2] += 4;
            Sound_PlayMenu(1);
        } else if ((D_00446DE8 & 0x80) && game_options[2] != 0 && D_004472DA != 0) {
            game_options[2] -= 4;
            Sound_PlayMenu(1);
        }
        break;
    case 3:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            game_options[5]++;
            Sound_PlayMenu(1);
        }
        game_options[5] = (s8)game_options[5] % 2;
        break;
    case 4:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            game_options[6]++;
            Sound_PlayMenu(1);
        }
        game_options[6] = (s8)game_options[6] % 2;
        break;
    case 5:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            Sound_PlayMenu(1);
            game_options[9]++;
        }
        game_options[9] = (s8)game_options[9] % 2;
        break;
    case 6:
        if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
            Sound_PlayMenu(1);
            game_options[8]++;
        } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
            Sound_PlayMenu(1);
            game_options[8]--;
        }
        game_options[8] = (s8)(game_options[8] + 3) % 3;
        break;
    }

    for (i = 0; i < D_00446A68; i++) {
        kind = D_0044725C[i].unk3E;
        Menu_SetBlendMode(D_0044725C[i].unk42);
        MenuSprite_Draw((int)&D_0044725C[i], kind + 1000, context->unk04, (int)D_00425510[i]);
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        D_004472DA = 0;
        D_00446C70 = 2;
    }

    switch (D_00446C70) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A68; i++) {
            D_00425510[i] += D_0044725C[i].alpha / 10.0f;
            if (D_00425510[i] > D_0044725C[i].alpha) {
                for (i = 0; i < D_00446A68; i++) {
                    full_alpha = D_0044725C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_00446C70 = 1;
                break;
            }
        }
        break;
    case 2:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A68; i++) {
            D_00425510[i] -= D_0044725C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_00446C94 == 2) {
                    D_00446BE5++;
                } else {
                    D_00446BE5 = 2;
                }
                if (D_00446BE5 == 2) {
                    D_00446BE5 = 0;
                    D_00446C70 = 0;
                    context->unk04 = 0;
                    if (D_00446BE4 == 1) {
                        if (game_options[0] == 0) {
                            if (D_00446BE0 != 0) {
                                D_00446BE4 = 2;
                                func_001CBED8();
                            } else {
                                D_00446BE4 = 4;
                            }
                        } else {
                            if (D_00446BE0 == 0) {
                                D_00446BE4 = 3;
                                func_001CC018();
                            } else {
                                D_00446BE4 = 4;
                            }
                        }
                    }
                    if (D_00446C94 != 2) {
                        D_00446B20 = (void *)Menu_UpdateSystem;
                    } else {
                        D_00446AC8++;
                    }
                    D_00446C08 = 0;
                    D_00446C04 = 128.0f;
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
    &D_0044692C, &D_0044692E, &D_00446932, &D_00446930, &D_00446936, &D_00446934, &D_0044693C, &D_0044693E, &D_00446940, &D_00446938, &D_0044693A, (u16 *)&D_00446942,
};

/* Fills both 12-entry tables with the index that goes with each slot's pad button bit. */
static void Menu_ReadButtonSettings(void) {
    s32 i;

    for (i = 0; i < 12; i++) {
        switch (*button_settings[i]) {
        case 0x10:
            D_004210C0[i] = D_004210B0[i] = 0;
            break;
        case 0x40:
            D_004210C0[i] = D_004210B0[i] = 1;
            break;
        case 0x20:
            D_004210C0[i] = D_004210B0[i] = 2;
            break;
        case 0x80:
            D_004210C0[i] = D_004210B0[i] = 3;
            break;
        case 0x400:
            D_004210C0[i] = D_004210B0[i] = 4;
            break;
        case 0x100:
            D_004210C0[i] = D_004210B0[i] = 5;
            break;
        case 0x1000:
            D_004210C0[i] = D_004210B0[i] = 6;
            break;
        case 0x4000:
            D_004210C0[i] = D_004210B0[i] = 7;
            break;
        case 0x2000:
            D_004210C0[i] = D_004210B0[i] = 8;
            break;
        case 0x8000:
            D_004210C0[i] = D_004210B0[i] = 9;
            break;
        case 0x800:
            D_004210C0[i] = D_004210B0[i] = 10;
            break;
        case 0x200:
            D_004210C0[i] = D_004210B0[i] = 11;
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

    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        if (D_00446BF0 == 0) {
            Sound_PlayMenu(1);
            // match: only this loop, in the function's first run of code, is loop-optimised in retail; the others keep their entry tests until late
            for (i = 0; i < choice_count; i++) {
                FOR_PAD_BIT(*button_settings[i], SET_CHOICE)
            }
            D_00446BE8--;
        } else {
            D_00446BEC = (D_00446BEC + 1) & 1;
        }
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        if (D_00446BF0 == 0) {
            Sound_PlayMenu(1);
            for (i = 0; i < choice_count; i++) {
                FOR_PAD_BIT(*button_settings[i], SET_CHOICE)
            }
            D_00446BE8++;
        } else {
            D_00446BEC = (D_00446BEC + 1) & 1;
        }
    }
    line_count = choice_count + 1;
    D_00446BE8 += line_count;
    D_00446BE8 %= line_count;

    wrap = choice_count; // match: a second variable, set here, is what retail keeps in $16 for the two divisions
    for (i = 0; i < wrap; i++) {
        if (D_004210B0[i] == -1) {
            FOR_PAD_BIT(*button_settings[i], SET_SHOWN)
        } else {
            D_004210C0[i] = D_004210B0[i];
        }
    }

    if (D_00446BE8 != choice_count && D_00446BF0 == 0) {
        if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
            Sound_PlayMenu(1);
            D_004210B0[D_00446BE8] = (s8)(D_004210B0[D_00446BE8] + 1) % wrap;
        } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
            Sound_PlayMenu(1);
            D_004210B0[D_00446BE8] = (s8)(D_004210B0[D_00446BE8] + 11) % wrap;
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
    for (i = 0; i < D_00446A70; i++) {
        kind = D_00447264[i].unk3E;
        slot = D_00447264[i].slot;
        if (!bar_drawn && D_00447264[i].unk43 == 1) {
            bar_drawn = TRUE;
            if (D_00446BF0 == 0) {
                bar.first.f.lo16 = 0x6FE0;
                bar.first.f.mid16 = D_00446BE8 * 0xB0 + 0x7BE0;
                bar.first.f.mid24 = bar_mid24;
                bar.second.f.lo16 = bar.first.f.lo16 + 0xE60;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xA0;
                bar.second.f.mid24 = bar.first.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                if (D_00446C70 == 1) {
                    D_00425510[0xFD] = Menu_PulseValue(D_00425510[0xFD], 0x10, 0x20);
                }
                bar.alpha = D_00425510[0xFD];
                MenuDraw_Bar(&bar);
            }
        }
        ((void (*)(u8))Menu_SetBlendMode)(D_00447264[i].unk42); // match: called as returning nothing, which frees $2 for the switch
        switch (kind) {
        case 2:
            if (D_00446BF0 != 0) {
                MenuSprite_Draw((int)&D_00447264[i], kind + 1000, 0, D_00425510[i]);
            }
            break;
        case 3:
            if (D_00446BF0 == 1) {
                MenuSprite_Draw((int)&D_00447264[i], kind + 1000, 0, D_00425510[i]);
            } else if (D_00446BF0 == 2) {
                MenuSprite_Draw((int)&D_00447264[i], kind + 1000, 1, D_00425510[i]);
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
            if (slot == D_004210C0[D_00446BE8]) {
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
            MenuSprite_Draw((int)&D_00447264[i], kind + 1000, slot, D_00425510[i]);
            break;
        case 0x21:
            if (D_00446E00 == 0) {
                j = i;
            }
            D_00446E04 = D_004210C0[D_00446E00];
            // match: called as returning a value, which keeps $2 out of the count's increment
            ((int (*)(int, int, int, int))MenuSprite_Draw)((int)&(&D_00447264[j])[11 - D_00446E04], kind + 1000, D_00446E00, D_00425510[i]);
            D_00446E00++;
            break;
        default:
            MenuSprite_Draw((int)&D_00447264[i], kind + 1000, slot, D_00425510[i]);
            break;
        }
        ((void (*)(u8))Menu_SetBlendMode)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        if (D_00446BF0 != 0) {
            D_00446BF0 = 0;
            D_00446BEC = 0;
        } else if (!has_duplicate && !last_is_shared) {
            D_004472DA = 0;
            D_00446C70 = 2;
        } else if (has_duplicate && last_is_shared) { // match: both arms set the same prompt; retail still tests last_is_shared here
            D_00446BF0 = 1;
        } else {
            D_00446BF0 = 1;
        }
    } else if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        Sound_PlayMenu(0);
        if (D_00446BF0 == 0) {
            if (D_00446BE8 != choice_count) {
                *button_settings[D_00446BE8] = pad_bits.bits[D_004210B0[D_00446BE8]];
                D_005CF318[D_00446BE8] = pad_bits.bits[D_004210B0[D_00446BE8]];
            } else {
                for (i = 0; i < choice_count; i++) {
                    D_004210B0[i] = default_button_choices[i];
                    *button_settings[i] = pad_bits.bits[D_004210B0[i]];
                }
            }
        } else if (D_00446BF0 == 1) {
            D_00446BF0 = 0;
        } else if (D_00446BF0 == 2) {
            if (D_00446BEC == 0) {
                D_00446C70 = 2;
                D_004472DA = 0;
            } else {
                D_00446BF0 = 0;
                D_00446BEC = 0;
            }
        }
    }

    switch (D_00446C70) {
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
        for (i = 0; i < D_00446A70; i++) {
            D_00425510[i] += (f32)D_00447264[i].alpha / 10.0f;
            if ((f32)D_00447264[i].alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A70; i++) {
                    full_alpha = D_00447264[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_00446C70 = 1;
                break;
            }
        }
        break;
    case 2:
        D_00425510[0xFF] -= 12.8f;
        if (D_00425510[0xFF] < 0.0f) {
            D_00425510[0xFF] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[0xFF];
        D_00425510[0xFD] -= 1.6f;
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A70; i++) {
            D_00425510[i] -= (f32)D_00447264[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_00446C94 == 3) {
                    D_00446BE6++;
                } else {
                    D_00446BE6 = 2;
                }
                if (D_00446BE6 == 2) {
                    D_00446BE6 = 0;
                    D_00446C70 = 0;
                    if (D_00446C94 != 3) {
                        D_00446B20 = (void *)Menu_UpdateSystem;
                    } else {
                        D_00446AC8++;
                    }
                    D_00446C04 = 128.0f;
                    D_00446C08 = 0;
                    D_00446BE8 = 0;
                    D_00446BEC = 0;
                    D_00446BF0 = 0;
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
        memset(D_00425510, 0, 0x400);
        D_00446B20 = (void *)Menu_UpdateCardCheck;
        D_005CF2F0.unk17 = 0;
        D_00446C70 = 0;
        D_00446C09 = 0;
        D_005CF2F0.unk11 = 0;
        D_005CF2F0.unk0C = 0;
        D_005CF2F0.unk1B = 0;
        D_005CF2F0.unk1C = 0;
        D_005CF2F0.unk04 = 0;
        D_005CF2F0.unk19 = 0;
        D_005CF2F0.unk1A = 0;
        D_005CF2F0.unk12 = -1;
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

    switch (D_005CF2F0.unk17) {
    case 0:
        if (SaveCard_IsCheckDone() == 0) {
            return;
        }
        if (SaveCard_IsPresent(0) != 0 && SaveCard_IsFormatted(0) != 0) {
            SaveCard_StartReadList(0, 0);
            D_005CF2F0.unk17 = 1;
        } else if (SaveCard_IsPresent(1) != 0 && SaveCard_IsFormatted(1) != 0) {
            D_005CF2F0.unk17 = 2;
            SaveCard_StartReadList(1, 0);
        } else {
            D_005CF2F0.unk17 = 3;
            SaveCard_StartRescan();
        }
        return;
    case 1:
        if (SaveCard_CheckBusy() == 0) {
            D_005CF2F0.unk19 = SaveCard_GetSavesState();
            if (SaveCard_IsPresent(1) != 0 && SaveCard_IsFormatted(1) != 0) {
                D_005CF2F0.unk17 = 2;
                SaveCard_StartReadList(1, 0);
            } else {
                D_005CF2F0.unk17 = 3;
                SaveCard_StartRescan();
            }
        } else if (result < 0) {
            D_005CF2F0.unk19 = 0;
            if (SaveCard_IsPresent(1) != 0 && SaveCard_IsFormatted(1) != 0) {
                D_005CF2F0.unk17 = 2;
                SaveCard_StartReadList(1, 0);
            } else {
                D_005CF2F0.unk17 = 3;
                SaveCard_StartRescan();
            }
        }
        return;
    case 2:
        if (SaveCard_CheckBusy() == 0) {
            D_005CF2F0.unk1A = SaveCard_GetSavesState();
            D_005CF2F0.unk17 = 3;
            SaveCard_StartRescan();
        } else if (result < 0) {
            D_005CF2F0.unk1A = 0;
            D_005CF2F0.unk17 = 3;
            SaveCard_StartRescan();
        }
        return;
    case 3:
        if (result > 0) {
            D_005CF2F0.unk17 = 4;
            for (i = 0; (s32)i < 2; i++) {
                switch (SaveCard_GetCardProblem(i)) {
                case 0:
                    D_005CF2F0.unk04 |= 1 << (i * 4);
                    break;
                case 8:
                    D_005CF2F0.unk04 |= 2 << (i * 4);
                    break;
                default:
                    D_005CF2F0.unk04 |= 4 << (i * 4);
                    break;
                }
            }
            if (D_005CF2F0.unk04 == 0x44) {
                D_005CF2F0.unk12 = 0x10;
            } else if (D_005CF2F0.unk04 == 0x22) {
                D_005CF2F0.unk12 = 0x13;
            } else if (D_005CF2F0.unk04 == 0x42) {
                D_005CF2F0.unk12 = 0x11;
            } else if (D_005CF2F0.unk04 == 0x24) {
                D_005CF2F0.unk12 = 0x12;
            } else {
                D_005CF2F0.unk1C = D_005CF2F0.unk11 = 1;
                SaveCard_Close();
            }
        }
        return;
    }

    if (D_005CF2F0.unk1C == 0) {
        for (i = 0; i < D_00446ABC; i++) {
            kind = D_00446C68[i].unk3E;
            ((void (*)(u8))Menu_SetBlendMode)(D_00446C68[i].unk42); // match: called as returning nothing, which frees $2 for the switch
            switch (D_005CF2F0.unk12) {
            case 0x10:
                MenuSprite_Draw((int)&D_00446C68[i], kind + 1000, 0x10, D_00425510[i]);
                break;
            case 0x11:
                MenuSprite_Draw((int)&D_00446C68[i], kind + 1000, 0x11, D_00425510[i]);
                break;
            case 0x12:
                MenuSprite_Draw((int)&D_00446C68[i], kind + 1000, 0x12, D_00425510[i]);
                break;
            case 0x13:
                MenuSprite_Draw((int)&D_00446C68[i], kind + 1000, 0x13, D_00425510[i]);
                break;
            }
            Menu_SetBlendMode(0);
        }
    }

    if (D_005CF2F0.unk1C == 0) {
        bar.first.f.lo16 = 0x75D0;
        bar.first.f.mid16 = D_005CF2F0.unk0C * 0xE0 + 0x8250;
        bar.first.f.mid24 = 0xDBBA0;
        bar.second.f.lo16 = bar.first.f.lo16 + 0x14A0;
        bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
        bar.second.f.mid24 = bar.first.f.mid24;
        bar.red = 0xFF;
        bar.green = 0xFF;
        bar.blue = 0x78;
        if (D_00446C70 == 1) {
            D_00425510[0xFD] = Menu_PulseValue(D_00425510[0xFD], 0x10, 0x20);
        } else {
            D_00425510[0xFD] = 0.0f;
        }
        bar.alpha = D_00425510[0xFD];
        MenuDraw_Bar(&bar);
    }

    if (D_005CF2F0.unk11 != 0) {
        if (result < 0) {
            if (D_00446C70 == 2) {
                D_00446C70 = 4;
            } else if (D_00446C70 == 3) {
                D_00446C70 = 5;
            } else if (D_00446C70 == 0) {
                MenuHeap_FreeToMark();
                D_00446AC8++;
            }
        }
    } else if (result < 0) {
        D_005CF2F0.unk11 = 1;
        SaveCard_StartCheck(1);
        SaveCard_Close();
    } else {
        report = Pad_GetReport(0, 0);
        if ((report->pressed & 0x4000) && D_004472D0 != 0xFF) {
            Sound_PlayMenu(0);
            D_005CF2F0.unk11 = 1;
            SaveCard_StartCheck(1);
            SaveCard_Close();
            if (D_005CF2F0.unk0C == 0) {
                D_00446C70 = 2;
            } else {
                D_00446C70 = 3;
            }
        } else if ((D_00446DE8 & 0x50) && D_004472D0 != 0xFF) {
            Sound_PlayMenu(1);
            D_005CF2F0.unk0C = (D_005CF2F0.unk0C + 1) & 1;
        }
    }

    if (D_005CF2F0.unk1C != 0) {
        return;
    }
    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446ABC; i++) {
            D_00425510[i] += (f32)D_00446C68[i].alpha / 10.0f;
            if ((f32)D_00446C68[i].alpha < D_00425510[i]) {
                for (i = 0; i < D_00446ABC; i++) {
                    full_alpha = D_00446C68[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00446C70 = 1;
                break;
            }
        }
        break;
    case 4:
    case 5:
        for (i = 0; i < D_00446ABC; i++) {
            D_00425510[i] -= (f32)D_00446C68[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_005CF2F0.unk1B++;
                if (D_005CF2F0.unk1B == 1) {
                    break;
                }
                if (D_00446C70 == 4) {
                    MenuHeap_FreeToMark();
                    D_00446AC8++;
                } else {
                    D_00446AC8--;
                }
                D_00446C70 = 0;
                break;
            }
        }
        break;
    }
}

/* TRUE when the card check (Menu_UpdateCardCheck) found a save on the card of either slot. */
s32 Menu_CardsHaveSaves(void) {
    if (D_005CF2F0.unk19 > 0 || D_005CF2F0.unk1A > 0) {
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

    if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
        Sound_PlayMenu(1);
        if (game_options[10] < 0xFC) {
            game_options[10] += 4;
        } else {
            game_options[10] = 0x20;
        }
    } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
        Sound_PlayMenu(1);
        if (game_options[10] > 0x20) {
            game_options[10] -= 4;
        } else {
            game_options[10] = 0xFC;
        }
    } else if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        Sound_PlayMenu(0);
        D_004472DA = 0;
        D_00446C70 = 2;
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        Sound_PlayMenu(2);
        game_options[10] = D_004472CF;
        D_00446C70 = 3;
        D_004472DA = 0;
    }
    D_00447228 = (game_options[10] - 32.0f) * 402.0f / 220.0f;
    for (i = 0; i < D_00446AB0; i++) {
        kind = D_00446C5C[i].unk3E;
        Menu_SetBlendMode(D_00446C5C[i].unk42);
        MenuSprite_Draw((int)&D_00446C5C[i], kind + 1000, (int)D_00447228 + 0x72C, (int)D_00425510[i]);
        Menu_SetBlendMode(0);
    }
    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446AB0; i++) {
            D_00425510[i] += D_00446C5C[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C5C[i].alpha) {
                for (i = 0; i < D_00446AB0; i++) {
                    full_alpha = D_00446C5C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00446C70 = 1;
                D_004472DA = 1;
                return;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446AB0; i++) {
            D_00425510[i] -= D_00446C5C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446C04 = 128.0f;
                D_00446C70 = 0;
                D_00446C08 = 0;
                if (D_00446C94 != 5) {
                    D_00446B20 = (void *)Menu_UpdateSystem;
                } else {
                    D_00446AC8++;
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
    if (Pad_GetReport(0, 0)->pressed & 0x2000) {
        if (D_004472D0 != 0xFF) {
            D_00446B20 = (void *)Menu_OpenMain;
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

    node = (DataNode *)D_005CE840[0]->item;
    table = Message_GetFontText(D_00446F38);
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
    if (D_00426110[0] != 0) {
        Menu_SetItemListModelNames();
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

/* Builds the model file names of the five link nodes: the selected item, its two neighbours, and the items that
   come into view next on either side (found by walking both ways from the page mark, seven steps in all). */
static void Menu_SetItemListModelNames(void) {
    MenuItem *backward;
    MenuItem *forward;
    s32 i;

    forward = backward = D_00426120[0];
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
    sprintf(D_00426498, "menu/select2/i%04d.mod", item_name_defs[D_00426110[0]->unk0C].file_number);
    sprintf(D_00426538, "menu/select2/i%04d.mod", item_name_defs[D_00426110[0]->prev->unk0C].file_number);
    sprintf(D_004264E8, "menu/select2/i%04d.mod", item_name_defs[D_00426110[0]->next->unk0C].file_number);
    sprintf(D_00426588, "menu/select2/i%04d.mod", item_name_defs[forward->unk0C].file_number);
    sprintf(D_004265D8, "menu/select2/i%04d.mod", item_name_defs[backward->unk0C].file_number);
}

/* A list cursor went on one row: the model shown moves to node 2, node 1's becomes the one shown, nodes 1, 3
   and 4 are made fresh and the five are chained again. */
void Menu_ShiftListModelsNext(s32 value) {
    LinkNode *pending = NULL;

    if (D_00446C90 == &D_004264C0 || D_00446C90 == &D_00426470) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_00426510);
    D_00426510 = D_00426470;
    D_00426510.next = NULL;
    if (D_00426510.unk20 == 0) {
        pending = &D_00426510;
    }
    D_00426470 = D_004264C0;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_004264C0);
    D_004264C0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_004264C0.unk08 = -1;
    D_004264C0.next = NULL;
    if (pending == NULL) {
        pending = &D_004264C0;
    }
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426560.next = NULL;
    if (D_00446C90 != NULL) {
        D_00446C90->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = &D_004265B0;
    D_004265B0.next = &D_00426560;
    D_00426560.next = NULL;
}

/* A list cursor went back one row: the model shown moves to node 1, node 2's becomes the one shown, nodes 2, 3
   and 4 are made fresh and the five are chained again. */
void Menu_ShiftListModelsPrev(s32 value) {
    LinkNode *pending = NULL;

    if (D_00446C90 == &D_00426510 || D_00446C90 == &D_00426470) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_004264C0);
    D_004264C0 = D_00426470;
    D_004264C0.next = NULL;
    if (D_004264C0.unk20 == 0) {
        pending = &D_004264C0;
    }
    D_00426470 = D_00426510;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    Menu_TakeLoadBuffer(&D_00426510);
    D_00426510.unk20 = 0;
    D_00426510.unk1C = 0;
    D_00426510.unk08 = -1;
    D_00426510.next = NULL;
    if (pending == NULL) {
        pending = &D_00426510;
    }
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426560.next = NULL;
    if (D_00446C90 != NULL) {
        D_00446C90->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = &D_00426560;
    D_00426560.next = &D_004265B0;
    D_004265B0.next = NULL;
}

/* A list went up a page: node 4's model becomes the one shown, the others are made fresh and the five are
   chained again. */
void Menu_ShiftListModelsPageUp(s32 value) {
    LinkNode *pending = NULL;

    if (D_00446C90 == &D_004265B0) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_00426470);
    D_00426470 = D_004265B0;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    D_004264C0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_004264C0.unk08 = -1;
    D_004264C0.next = NULL;
    D_00426510.unk20 = 0;
    D_00426510.unk1C = 0;
    D_00426510.unk08 = -1;
    D_00426510.next = NULL;
    Menu_TakeLoadBuffer(&D_004265B0);
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426560.next = NULL;
    if (pending == NULL) {
        pending = &D_00426560;
    }
    if (D_00446C90 != NULL) {
        D_00446C90->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_00426560;
    D_00426560.next = &D_004265B0;
    D_004265B0.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
}

/* A list went down a page: node 3's model becomes the one shown, the others are made fresh and the five are
   chained again. */
void Menu_ShiftListModelsPageDown(s32 value) {
    LinkNode *pending = NULL;

    if (D_00446C90 == &D_00426560) {
        ((void (*)(s32))Menu_CopyLoadToSpare)(value); // match: the callee takes nothing, yet the argument is passed on
    } else {
        ((void (*)(s32))Menu_MoveLoadToSpare)(value); // match: as above
    }
    Menu_FreeLoadBuffer((MenuFreeRequest *)&D_00426470);
    D_00426470 = D_00426560;
    D_00426470.next = NULL;
    if (D_00426470.unk20 == 0) {
        pending = &D_00426470;
    }
    D_004264C0.unk20 = 0;
    D_004264C0.unk1C = 0;
    D_004264C0.unk08 = -1;
    D_004264C0.next = NULL;
    D_00426510.unk20 = 0;
    D_00426510.unk1C = 0;
    D_00426510.unk08 = -1;
    D_00426510.next = NULL;
    D_004265B0.unk20 = 0;
    D_004265B0.unk1C = 0;
    D_004265B0.unk08 = -1;
    D_004265B0.next = NULL;
    Menu_TakeLoadBuffer(&D_00426560);
    D_00426560.unk20 = 0;
    D_00426560.unk1C = 0;
    D_00426560.unk08 = -1;
    D_00426560.next = NULL;
    if (pending == NULL) {
        pending = &D_00426510;
    }
    if (D_00446C90 != NULL) {
        D_00446C90->next = pending;
    } else {
        D_00446C90 = pending;
    }
    D_00426470.next = &D_004265B0;
    D_004265B0.next = &D_00426560;
    D_00426560.next = &D_004264C0;
    D_004264C0.next = &D_00426510;
    D_00426510.next = NULL;
}

/* Resets the memory card screens' state, starts the card check and makes Menu_UpdateCardSlot the current handler. */
static void Menu_OpenCardSlot(void) {
    D_005CF2F0.unk16 = 0;
    D_005CF2F0.unk10 = 0;
    D_005CF2F0.unk11 = 0;
    D_005CF2F0.unk14 = 0;
    D_005CF2F0.unk0C = 0;
    D_005CF2F0.unk0D = 0;
    D_005CF2F0.unk18 = 1;
    D_005CF2F0.unk1B = 0;
    D_005CF2F0.unk1E = 0;
    D_005CF2F0.unk1F = 0;
    D_005CF2F0.unk20 = 0;
    D_005CF2F0.unk00 = 0;
    D_005CF2F0.unk0E = 1;
    D_005CF2F0.unk12 = -1;
    SaveCard_Open();
    SaveCard_ClearLoadFailed(0);
    SaveCard_ClearLoadFailed(1);
    SaveCard_StartCheck(1);
    D_00446B20 = (void *)Menu_UpdateCardSlot;
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

/* Sends a five-quadword packet that draws one sprite in the grey level given, into the frame buffer D_00446F78 selects. */
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
        if (D_00446F78 == 0) {
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
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 5;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x105;
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

/* Sets the blend mode of ALPHA_1 from mode and writes it; the value is never seeded. */
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
            *D_00420D60[0] = D_004231F0;
            player_state.weapon = 0;
            break;
        case 1:
            *D_00420D60[1] = D_00424050;
            player_state.equipped_magic = 0;
            break;
        case 2:
            *D_00420D60[2] = D_00423650;
            player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
            break;
        case 3:
            *D_00420D60[3] = D_00423830;
            player_state.equipment[EQUIP_HELM] = ITEM_WORN_INDEX(ITEM_HELM_NONE);
            break;
        case 4:
            *D_00420D60[4] = D_004239C0;
            player_state.equipment[EQUIP_ARMOR] = ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
            break;
        case 5:
            *D_00420D60[5] = D_00423B50;
            player_state.equipment[EQUIP_GAUNTLET] = ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
            break;
        case 6:
            *D_00420D60[6] = D_00423CE0;
            player_state.equipment[EQUIP_BOOT] = ITEM_WORN_INDEX(ITEM_BOOT_NONE);
            break;
        case 7:
            *D_00420D60[7] = D_00423E70;
            player_state.equipment[EQUIP_RING] = ITEM_WORN_INDEX(ITEM_RING_NONE);
            break;
        case 8:
            *D_00420D60[8] = D_00423F10;
            player_state.equipment[EQUIP_BRACELET] = ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
            break;
        case 9:
            *D_00420D60[9] = D_00423FB0;
            player_state.equipment[EQUIP_AMULET] = ITEM_WORN_INDEX(ITEM_AMULET_NONE);
            break;
        case 11:
            *D_00420D60[10] = D_00424600;
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
