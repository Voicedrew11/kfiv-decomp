/* The menus' drawing layer (kf_menu_draw.c). */
#ifndef KF_MENU_DRAW_H
#define KF_MENU_DRAW_H

#include "common.h"
#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_state.h"
#include "kf_menu.h"
#include "kf_game.h"
#include "kf_screen.h"
#include "kf_load.h"
#include "kf_texture.h"

/* a sprite handler (sprite_handlers): takes the sprite's address and a value, gives the sprite to draw or NULL */
typedef KindObject *(*SpriteHandler)(int sprite, int param);

/* Empties the menus' heap: its mark and its top both go back to the base. */
void MenuHeap_Reset(void);

/* Sets the heap's mark at its top: what is allocated so far survives MenuHeap_FreeToMark. */
void MenuHeap_Mark(void);

/* Takes size bytes from the top of the menus' heap, leaving the top 16-byte aligned. */
void *MenuHeap_Alloc(int size);

/* Frees everything allocated since the mark was set. */
void MenuHeap_FreeToMark(void);

/* Starts the second allocator at `value` and takes the part drawers' work buffer from it; no result is set. */
s32 MenuArena_Init(s32 value);

/* Takes `size` bytes at a multiple of `align` from the second allocator; NULL when it has no room. */
void *MenuArena_Alloc(s32 size, s32 align);

/* Hands out the next of the two packet buffers in the scratchpad. */
void *MenuDraw_NextPacket(void);

/* Draws a sprite of kind 0, a textured rectangle. */
void MenuDraw_Sprite(KindObject *object);

/* Returns the low byte of where word `index` starts in the glyph codes MenuDraw_Word draws from. */
s8 MenuDraw_GetWordStart(s32 index);

/* Draws a sprite of kind 1, a word of the menus' word table. */
void MenuDraw_Word(KindObject *object);

/* Draws a flat rectangle (the cursor bar). */
void MenuDraw_Bar(MenuCursorBar *bar);

/* Gives the current value of the number a menu sprite stands for (SpritePacket.unk34). */
int MenuDraw_GetNumber(int number);

/* Draws a sprite of kind 2 or 3, a number; no result is set. */
int MenuDraw_Number(KindObject *object);

/* Sets the text pen: x and y in 12.4 fixed point, the depth and a top byte. Returns 1. */
int MenuText_SetPen(GsPackedWord word);

/* Sets the size of the cell a full-width glyph is drawn in (the word's x and y fields). Returns 1. */
int MenuText_SetGlyphSize(GsPackedWord word);

/* Sets the colour text is drawn in. Returns 1. */
s32 MenuText_SetColor(ColorBytes bytes);

/* Draws text at the pen, wrapping at `limit` glyphs a line and at the line breaks in the text; returns the number
   of glyphs drawn (like all the text drawers). */
s32 MenuText_DrawWrapped(u16 *text, s32 limit);

/* MenuText_DrawWrapped for a long text (built in a large buffer, not in the scratchpad). */
s32 MenuText_DrawWrappedLong(u16 *text, s32 limit);

/* Draws one line of text at the pen, at most `limit` glyphs. */
s32 MenuText_DrawLine(u16 *text, s32 limit);

/* MenuText_DrawLine shortened to three words, with dots for what is left out. */
s32 MenuText_DrawLineShortened(u16 *text, s32 limit);

/* MenuText_DrawLine centred in the width of `limit` glyphs. */
s32 MenuText_DrawLineCentered(u16 *text, s32 limit);

/* Draws a long text at the pen, wrapping at `limit` glyphs a line and only between words. */
s32 MenuText_DrawWordWrappedLong(u16 *text, s32 limit);

/* Turns the file offsets in a loaded part file (each part's strips, points and normals) into pointers. */
void PoolPartFile_Relocate(PoolPartFile *file);

/* Turns the file offsets in a loaded node file (each node's keys, layers, child and sibling) into pointers. */
s32 PoolNodeFile_Relocate(RelocFile *file);

/* Draws a part in one flat colour (the four bytes of the third argument). */
void PoolPart_DrawTinted(PoolPart *part, s32 parent, RenderSlotArg color);

/* Draws a part with the colours of its points. */
void PoolPart_Draw(PoolPart *part, s32 parent);

/* Uploads a loaded model's texture and draws its parts with PoolPart_DrawTinted. */
void MenuModel_DrawTinted(ChunkUpload *upload, s32 draw_param, RenderSlotArg slot_arg);

/* Uploads a loaded model's texture and draws its parts with PoolPart_Draw. */
void MenuModel_Draw(ChunkUpload *upload, s32 draw_param);

/* Draws a slot's part like PoolPart_DrawTinted does a part, but with the slot's own matrix, and, when the part has
   a record, with its first points and normals moved by the parent slot's matrix instead. */
void PoolSlot_Draw(RenderSlot *slot, RenderSlotArg unused);

/* Runs PoolSlot_Draw on the first 21 slots of the slot array. */
void PoolSlot_DrawAll(RenderSlot *slots, RenderSlotArg slot_arg);

/* Runs PoolSlot_Draw on the first 20 slots of the slot array. */
void PoolSlot_DrawFirst20(RenderSlot *slots, RenderSlotArg slot_arg);

/* Runs PoolSlot_Draw on slots 0-14 and 16-20 of the slot array. */
void PoolSlot_DrawAllBut15(RenderSlot *slots, RenderSlotArg slot_arg);

/* Runs PoolSlot_Draw on slots 0-14 and 16-19 of the slot array. */
void PoolSlot_DrawFirst20But15(RenderSlot *slots, RenderSlotArg slot_arg);

/* Gives the slots of one group the file's parts in order. */
void PoolSlot_SetGroupParts(PoolPartFile *file, s32 group);

/* Uploads a group's texture to its place in GS memory and gives the group's slots the file's parts. */
void PoolSlot_LoadGroup(PoolPartSource *source, u32 group);

/* Takes the part away from every slot of the pool. */
s32 PoolSlot_ClearParts(void);

/* Poses a node of a skeleton at a frame of its animation, into its slot's matrix, then its children and siblings. */
void PoolNode_Pose(RelocEntry *node, PoolSlot *slots, f32 *parent_matrix, s32 frame);

/* Copies a part file with every strip point expanded to its position and normal (0x40 bytes a point). */
void PoolPartFile_Expand(PoolPartFile *source, PoolPartFile *dest);

/* Draws a sprite after running sprite handler `index` on it (owner: the SpritePacket's address). */
void MenuSprite_DrawPlain(int owner, int index, int param);

/* Reads a whole file into buffer; returns buffer, or 0 when the file does not open. */
int MenuFile_Load(char *name, void *buffer);

/* Notes the blocks of the loaded menu file: how many, and where each one is. */
void MenuFile_IndexBlocks(char *file);

/* Sends a texture's image to GS memory, in two halves when it is large, and then its CLUT when the pixel format
   has one. */
void MenuTexture_Upload(TextureHeader *header, LoadedBlockRef *block);

/* Sends a texture's image to GS memory a strip of rows at a time, then its CLUT and a second CLUT from the file. */
void MenuTexture_UploadTwoCluts(TextureHeader *header, u64 *tex0_out, u64 *trailer_out);

/* Copies out a texture's TEX0 word and, for a palettized PSM, the word after its CLUT. */
void MenuTexture_GetRegisters(TextureHeader *tex, u64 *tex0_out, u64 *trailer_out);

/* Builds the menus' buttons for this frame (D_00446DE8), directions as repeating steps; returns them. */
s16 MenuPad_Update(void);

/* Writes one GS register and waits for it; returns what sceGsSyncPath says. */
s32 MenuDraw_SetGsRegister(int reg_address, s64 reg_value);

/* Draws a sprite like MenuSprite_DrawPlain, with handler entry_id % 1000; from 1000 on, with `variant` as its alpha. */
void MenuSprite_Draw(int owner, int entry_id, int param, int variant);

#endif
