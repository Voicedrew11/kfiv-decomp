#ifndef KF_MESSAGE_H
#define KF_MESSAGE_H

#include "common.h"

/* one of the six notices of the ring, the short messages KF_Message_ShowNotice puts on screen */
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

/* the message box (KF_Message_OpenBox opens it, KF_Message_Update types it out page by page) */
typedef struct {
    s16 *start;                     /* 0x00 first glyph of the page on screen */
    s16 *cursor;                    /* 0x04 glyph being typed */
    s16 *end;                       /* 0x08 end of the page (KF_Message_LayoutPage) */
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
    int line_offsets[8];            /* 0x38 left edge of each line (KF_Message_LayoutPage) */
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

void KF_Message_Init(void);
void KF_Message_Update(void);
int KF_Message_IsBoxOpen(void);
void KF_Message_OpenBox(int x, int y, int font, int centred, int delay, int message_id, ...);
void KF_Message_ShowNotice(int x, int y, int font, int kind, int frames, int message_id, ...);
void KF_Message_DebugPrintf(char *format, ...);
void KF_Message_DebugSetColor(char colour);
s32 KF_Message_GetFontTexture(s32 font_base);
s32 KF_Message_GetFontText(s32 font_base);
char *KF_Message_GetColor(s32 colour);
void KF_Message_ClearNotices(void);
void KF_Message_CloseBox(void);

#endif
