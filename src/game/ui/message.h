#ifndef KF_MESSAGE_H
#define KF_MESSAGE_H

#include "common.h"

/* glyph codes: below 0x800 a full-width glyph, then flags */
#define GLYPH_HALF_WIDTH 0x800
#define GLYPH_NUMBER 0x1000      /* the next argument, in digit glyphs padded with blanks; width in bits 8-11 */
#define GLYPH_RAW_NUMBER 0x2000  /* the next argument, in raw digits padded with 0x4AF */
#define GLYPH_COLOR 0x4000      /* colour in bits 8-11 */
#define GLYPH_NEW_LINE -0x8000
#define GLYPH_END -1
#define GLYPH_FIELD_MASK 0xF00      /* the colour or the width of a control code, bits 8-11 */
#define GLYPH_CONTROL_MASK 0xF000   /* set on every control code (number, raw number, colour, new line) */

/* one of the six notices of the ring, the short messages Message_ShowNotice puts on screen */
typedef struct MessageSlot {
    s16 *text;                      /* 0x00 glyph stream, ended by -1 */
    int frames_left;                /* 0x04 */
    int frames;                     /* 0x08 how long it stays on screen */
    int length;                     /* 0x0C glyphs, control codes not counted */
    struct MessageSlot *next;       /* 0x10 */
    int left;                       /* 0x14 */
    int top;                        /* 0x18 */
    int font;                       /* 0x1C */
    int kind;                       /* 0x20 */
} MessageSlot;

/* the message box (Message_OpenBox opens it, Message_Update types it out page by page) */
typedef struct {
    s16 *start;                     /* 0x00 first glyph of the page on screen */
    s16 *cursor;                    /* 0x04 glyph being typed */
    s16 *end;                       /* 0x08 end of the page (Message_LayoutPage) */
    int delay;                      /* 0x0C frames per glyph */
    int timer;                      /* 0x10 frames since the last glyph */
    int reveal_frames;              /* 0x14 frames since the page started, drives the fade-in behind the cursor */
    int waiting;                    /* 0x18 1 = page typed out, waiting for a button */
    int typed;                      /* 0x1C glyphs typed on this page (control codes not counted) */
    int closing;                    /* 0x20 1 = closing */
    int close_timer;                /* 0x24 frames left of the closing */
    int left;                       /* 0x28 */
    int top;                        /* 0x2C */
    int font;                       /* 0x30 */
    int centred;                    /* 0x34 nonzero: each line starts at its line_offsets entry */
    int line_offsets[8];            /* 0x38 left edge of each line (Message_LayoutPage) */
} MessageBox;

/* one of the ten text colours (glyph code 0x4000 | colour << 8) */
typedef struct {
    s32 red;                        /* 0x00 */
    s32 green;                      /* 0x04 */
    s32 blue;                       /* 0x08 */
    s32 alpha;                      /* 0x0C */
} TextColor;

/* the size of the glyphs of one font */
typedef struct {
    s32 advance;                    /* 0x00 from one full-width glyph to the next */
    s32 line_height;                /* 0x04 */
    s32 unk08;                      /* 0x08 */
    s32 lines_per_page;             /* 0x0C of the message box */
} FontMetrics;

/* how the glyphs of one kind (0 full width, 1 half width, 2 the debug font) lie on their texture */
typedef struct {
    f32 step_s;                     /* 0x00 from one column to the next */
    f32 step_t;                     /* 0x04 from one row to the next */
    f32 size_s;                     /* 0x08 of one glyph */
    f32 size_t;                     /* 0x0C */
    f32 base_s;                     /* 0x10 of the first glyph */
    f32 base_t;                     /* 0x14 */
    s32 columns;                    /* 0x18 glyphs in a row */
} GlyphSheet;

/* the screen box of a glyph of one font and kind, relative to its position */
typedef struct {
    s32 left;                       /* 0x00 */
    s32 top;                        /* 0x04 */
    s32 right;                      /* 0x08 */
    s32 bottom;                     /* 0x0C */
} GlyphBox;

/* Message ids, as Message_OpenBox and Message_ShowNotice take them: the message's place in the offset table of
   font.txc, counted from 0 at the table's second word (Message_GetText). The menus index the table
   themselves, with the id plus 1. Only the ids the code itself names are listed. */
#define MSG_NOTHING_HAPPENED        10
#define MSG_UNABLE_TO_USE_HERE      11
#define MSG_GOLD                    20      /* takes the amount */
#define MSG_CANT_CARRY_ANY_MORE     30
#define MSG_LEVEL_UP                40
#define MSG_MAGIC_LEARNED           41
#define MSG_PHYSICAL_UP             42
#define MSG_MAGIC_POWER_UP          43
#define MSG_WEAPON_LEVEL_UP         44
#define MSG_MAGIC_LEVEL_UP          45
#define MSG_LOCKED                  62
#define MSG_EMPTY                   70
#define MSG_KEY_USED_FIRST          87      /* "... Key used": one for each key item, in the keys' order */
#define MSG_ITEM_TEXT_FIRST         500     /* what the menus say of item 0; the items follow in order */
#define MSG_MAGIC_TEXT_FIRST        900     /* what the menus say of magic 0 (none); the magics follow */
#define MSG_RECOLLECTION_FIRST      3000    /* the pages of the menu's Recollection screen, 50 ids each */

void Message_Init(void);
void Message_Update(void);
int Message_IsBoxOpen(void);
void Message_OpenBox(int x, int y, int font, int centred, int delay, int message_id, ...);
void Message_ShowNotice(int x, int y, int font, int kind, int frames, int message_id, ...);
void Message_DebugPrintf(char *format, ...);
void Message_DebugSetColor(char colour);
s32 Message_GetFontTexture(s32 font_base);
s32 Message_GetFontText(s32 font_base);
char *Message_GetColor(s32 colour);
void Message_ClearNotices(void);
void Message_CloseBox(void);
int Message_GetText(int message_id);

#endif
