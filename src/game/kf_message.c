#include "kf_message.h"
#include "kf_file.h"
#include "kf_gs.h"
#include "kf_memory.h"
#include "kf_texture.h"

/* EE EABI varargs: the argument registers after the named ones are saved just below the stack arguments */
typedef char *va_list;
#define VA_START(ap, last, named_count) ((ap) = (char *)__builtin_next_arg(last) - 8 * (8 - (named_count)))
#define va_arg(ap, type) ((ap) += 8, *(type *)((ap) - 8))

#define NOTICE_COUNT 6
#define DEBUG_LOG_LINES 0x80
#define DEBUG_LOG_LINE_SIZE 0x108

/* glyph codes: below 0x800 a full-width glyph, then flags */
#define GLYPH_HALF_WIDTH 0x800
#define GLYPH_NUMBER 0x1000      /* the next argument, in digit glyphs padded with blanks; width in bits 8-11 */
#define GLYPH_RAW_NUMBER 0x2000  /* the next argument, in raw digits padded with 0x4AF */
#define GLYPH_COLOR 0x4000      /* colour in bits 8-11 */
#define GLYPH_NEW_LINE -0x8000
#define GLYPH_END -1

#define GLYPH_GIF_NREG (7 << 28) /* high word of the glyphs' GIF tag: seven registers each */

int Timer_GetTicks(void);
void func_0010B238(TextureTableHeader *);
void func_00109E88(void);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
int func_001D0758(int, u8 *);
void *memset(void *, int, u32);
int vsprintf(char *, const char *, char *);

extern s32 D_00446F38;
extern u16 pad_pressed[];
extern u16 D_00446940;

static TextColor text_colours[10] = {     /* 0x420B40 */
    { 0x80, 0x80, 0x80, 0x80 }, { 0x00, 0x00, 0x00, 0x80 }, { 0x00, 0x00, 0x80, 0x80 }, { 0x00, 0x80, 0x00, 0x80 },
    { 0x00, 0x80, 0x80, 0x80 }, { 0x80, 0x00, 0x00, 0x80 }, { 0x80, 0x00, 0x80, 0x80 }, { 0x80, 0x80, 0x00, 0x80 },
    { 0x80, 0x80, 0x80, 0x80 }, { 0x50, 0x50, 0x50, 0x80 },
};
static FontMetrics font_metrics[3] = {     /* 0x420BE0 */
    { 20, 20, 22, 5 },
    { 24, 24, 22, 5 },
    { 34, 34, 15, 3 },
};
static GlyphSheet glyph_sheets[3] = {      /* 0x420C10 on the 512x512 font texture */
    { 20.0f / 512, 20.0f / 512, 20.0f / 512, 20.0f / 512, 0.0f, 0.0f, 25 },
    { 10.0f / 512, 20.0f / 512, 10.0f / 512, 20.0f / 512, 0.0f, 460.0f / 512, 50 },
    { 10.0f / 512, 20.0f / 512, 10.0f / 512, 20.0f / 512, 0.0f, 460.0f / 512, 50 },
};
static GlyphBox glyph_boxes[3][3] = {      /* 0x420C68 [font][kind] */
    { { -320, -224, -300, -204 }, { -320, -224, -310, -204 }, { -320, -214, -310, -198 } },
    { { -320, -224, -296, -200 }, { -320, -224, -308, -200 }, { -320, -214, -310, -198 } },
    { { -320, -224, -286, -190 }, { -320, -224, -303, -190 }, { -320, -214, -310, -198 } },
};

static s16 notice_texts[NOTICE_COUNT][0x200];                     /* 0x5BF8C8 */
static s16 box_text[0x2000];                                      /* 0x5C10C8 */
static s32 glyph_alphas[0x200];                                   /* 0x5C50C8 alpha of each glyph drawn */
static GsRegisterPair font_tex0[2];                               /* 0x5C58C8 TEX0 of the two texture pages */
static MessageBox message_box;                                    /* 0x5C58E8 */
static MessageSlot notices[NOTICE_COUNT];                         /* 0x5C5940 */
static u8 debug_log[DEBUG_LOG_LINES][DEBUG_LOG_LINE_SIZE];        /* 0x5C5A18 */

static u32 *glyph_packet;           /* 0x4471D8 where the next glyph goes in the text packet */
static int glyph_count;             /* 0x4471DC glyphs in this frame's text packet */
static char box_open;               /* 0x4471E0 */
static char notices_active;         /* 0x4471E1 */
static MessageSlot *notice_shown;   /* 0x4471E4 */
static MessageSlot *notice_free;    /* 0x4471E8 */
static int debug_line_count;        /* 0x4471EC */

static void Message_Draw(void);
static void Message_DrawDebugLog(void);
static int Message_Format(u16 *format, u16 *out, va_list args);
static void Message_DrawGlyphs(char kind, int font, int left, int top, s16 *text, int count);
static void Message_DrawGlyph(int pos_x, int pos_y, int glyph, int colour, int alpha, int kind, int font);
static s32 Message_GetGlyphWidth(u16 *glyph, s32 font);
static int Message_GetTextWidth(s16 *text, int font);
static s16 *Message_LayoutPage(s16 *text, s32 font);

/* Loads font.tmr and font.txc, empties the notice ring and the debug log, and sets up the TEX0 writes of the
   two texture pages of the font. */
void Message_Init(void) {
    char unused[0x60]; // match: retail reserves 0x60 bytes it never uses
    int file_handle;
    int i;

    file_handle = func_00102238("font.tmr", 0, 1);
    func_001025C0(file_handle, (void *)Message_GetFontTexture(D_00446F38), -1);
    func_001024B0(file_handle);
    file_handle = func_00102238("font.txc", 0, 1);
    func_001025C0(file_handle, (void *)Message_GetFontText(D_00446F38), -1);
    func_001024B0(file_handle);
    glyph_count = 0;
    notice_free = notices;
    notice_shown = notices;
    debug_line_count = 0;
    box_open = 0;
    notices_active = 0;
    for (i = 0; i < NOTICE_COUNT - 1; i++) {
        notices[i].text = notice_texts[i];
        notices[i].next = &notices[i + 1];
    }
    notices[NOTICE_COUNT - 1].text = notice_texts[NOTICE_COUNT - 1];
    notices[NOTICE_COUNT - 1].next = notices;
    // match: the callee returns a value here; $2 is then taken when the table address is formed
    ((int (*)(void *, u32))Memory_ClearWords)(glyph_alphas, 0x200);
    font_tex0[0].first = 0x4007FC0665423B40L;
    font_tex0[0].second = GS_REG_TEX0_1;
    font_tex0[1].first = 0x4007FC2665423B40L;
    font_tex0[1].second = GS_REG_TEX0_1;
}

/* Per-frame update of the message box (typing, paging, closing) and of the notice ring, then draws them. */
void Message_Update(void) {
    MessageBox *box;

    Timer_GetTicks();
    if (box_open == 0 && notices_active == 0) {
        Message_Draw();
        return;
    }
    if (box_open == 1) {
        box = &message_box;
        if (box->reveal_frames < box->delay * box->typed + 8) {
            box->reveal_frames++;
        }
        if (box->closing == 1) {
            if (--box->close_timer <= 0) {
                box_open = 0;
            }
        } else if (*box->cursor == GLYPH_END) {
            if (pad_pressed[0] & D_00446940) {
                box->closing = 1;
                box->close_timer = 8;
            }
        } else if (box->waiting == 1) {
            if (pad_pressed[0] & D_00446940) {
                box->waiting = 0;
                box->start = box->end + 1;
                box->reveal_frames = 0;
                box->end = Message_LayoutPage(box->start, box->font);
                box->typed = 0;
            }
        } else if (pad_pressed[0] & D_00446940) {
            while (message_box.cursor < message_box.end) {
                if ((*message_box.cursor & ~0xFFF) == 0) {
                    message_box.typed++;
                }
                message_box.cursor++;
            }
            message_box.waiting = 1;
            message_box.timer = 0;
            message_box.reveal_frames = message_box.delay * message_box.typed + 8;
        } else {
            box->timer++;
            if (box->timer >= box->delay) {
                if (box->cursor >= box->end) {
                    box->waiting = 1;
                } else {
                    if ((*box->cursor & ~0xFFF) == 0) {
                        box->typed++;
                    }
                    box->timer = 0;
                    box->cursor++;
                }
            }
        }
    } else if (notices_active == 1) {
        MessageSlot *slot = notice_shown;

        if (--slot->frames_left < 0) {
            if (slot == notice_free) {
                notices_active = 0;
            } else {
                notice_shown = slot->next;
                if (slot->next == notice_free) {
                    notices_active = 0;
                }
            }
        }
    }
    Message_Draw();
}

/* Builds and queues the GS packet that draws the message box or the shown notice, and the debug log. */
static void Message_Draw(void) {
    char unused[0x60]; // match: retail reserves 0x60 bytes it never uses
    u32 *packet;
    u32 *words;
    int textured = TRUE;
    u64 *blend;
    u64 *texture;

    if (box_open != 0 || notices_active != 0 || debug_line_count != 0) {
        func_0010B238((TextureTableHeader *)Message_GetFontTexture(D_00446F38));
        func_00109E88();
        if (box_open != 0) {
            glyph_count += message_box.typed;
        } else if (notices_active != 0) {
            glyph_count += notice_shown->length;
        }
        /* seven quadwords for each glyph after a header of 12 */
        words = packet = func_00109990(glyph_count * 7 + 0xD);
        *words++ = (glyph_count * 7 + 0xB) | 0x10000000;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = (glyph_count * 7 + 0xA) | 0x51000000;
        /* GIF tag: three register writes */
        *words++ = 0x8003;
        *words++ = 0x10004000;
        *words++ = 0xE;
        *words++ = 0;
        blend = (u64 *)words;
        blend[0] = 0;
        blend[1] = 0x49; /* PABE */
        blend[2] = 0x44;
        blend[3] = GS_REG_ALPHA_1;
        words = (u32 *)(blend + 4);
        *(u64 *)words = 0x30000;
        words += 2;
        *(u64 *)words = GS_REG_TEST_1;
        words += 2;
        if (textured) { // match: see `texture` below
            /* GIF tag: four register writes */
            *words++ = 0x8004;
            *words++ = 0x10004000;
            *words++ = 0xE;
            *words++ = 0;
        }
        // match: the tag after these registers is only addressed from `texture` (and not from `packet`) when the
        // compiler loses track of where `texture` points, which a condition it works out later does
        if (textured) {
            texture = (u64 *)words;
            texture[0] = 0;
            texture[1] = 0; /* PRIM */
            texture[4] = 0x4007FC0665423B40L;
            texture[5] = GS_REG_TEX0_1;
            texture[2] = 0;
            texture[3] = GS_REG_TEX1_1;
            texture[6] = 0;
            texture[7] = 8; /* CLAMP_1 */
        }
        /* GIF tag: the glyphs, seven registers each (PRIM, a register write, then RGBAQ, ST, XYZF2, ST, XYZF2) */
        glyph_packet = (u32 *)(texture + 8);
        *glyph_packet++ = glyph_count | 0x8000;
        *glyph_packet++ = GLYPH_GIF_NREG;
        *glyph_packet++ = 0x42421E0;
        *glyph_packet++ = 0;
        if (box_open == 1) {
            Message_DrawGlyphs(0, message_box.font, message_box.left, message_box.top, message_box.start,
                                  message_box.typed);
        } else if (notices_active == 1) {
            Message_DrawGlyphs(1, notice_shown->font, notice_shown->left, notice_shown->top, notice_shown->text,
                                  notice_shown->length);
        }
        if (debug_line_count > 0) {
            Message_DrawDebugLog();
        }
        func_00109650(packet, glyph_packet);
        glyph_count = 0;
        debug_line_count = 0;
    }
}

/* Whether the message box is still on screen. */
int Message_IsBoxOpen(void) {
    return box_open;
}

/* Opens the message box on message `message_id`, formatted with the arguments, unless it is open already.
   It types out a glyph every `delay` frames; `centred` centres each line. */
void Message_OpenBox(int x, int y, int font, int centred, int delay, int message_id, ...) {
    va_list args;
    int format;

    format = Message_GetText(message_id);
    VA_START(args, message_id, 6);
    if (box_open == 0) {
        box_open = 1;
        Message_Format((u16 *)format, (u16 *)box_text, args);
        message_box.reveal_frames = 0;
        message_box.delay = delay;
        message_box.timer = delay;
        message_box.start = box_text;
        message_box.left = x + 0x38;
        message_box.top = y + 0xF0;
        message_box.close_timer = 8;
        message_box.centred = centred;
        message_box.waiting = 0;
        message_box.typed = 0;
        message_box.cursor = box_text;
        message_box.font = font;
        message_box.closing = 0;
        message_box.end = Message_LayoutPage(box_text, font);
    }
}

/* Formats message `message_id` into a notice of the ring for `frames` frames, centred on (x + 320, y + 400).
   kind 1 overwrites the notice being shown; any other kind takes the next free slot, or does nothing when the
   ring is full. */
void Message_ShowNotice(int x, int y, int font, int kind, int frames, int message_id, ...) {
    va_list args;
    int format;
    int width;

    format = Message_GetText(message_id);
    if (kind == 1) {
        int left; // match: the two branches have their own left and top
        int top;

        VA_START(args, message_id, 6);
        notice_shown->length = Message_Format((u16 *)format, (u16 *)notice_shown->text, args);
        width = Message_GetTextWidth(notice_shown->text, font);
        left = x + 0x140;
        top = y + 0x190;
        notice_shown->frames = frames;
        notice_shown->frames_left = frames;
        notice_shown->font = font;
        notice_shown->left = left - width / 2;
        notice_shown->top = top;
        notice_shown->kind = kind;
    } else {
        int left;
        int top;

        if (notice_free->next == notice_shown) {
            return;
        }
        VA_START(args, message_id, 6);
        notice_free->length = Message_Format((u16 *)format, (u16 *)notice_free->text, args);
        width = Message_GetTextWidth(notice_free->text, font);
        left = x + 0x140;
        top = y + 0x190;
        notice_free->frames = frames;
        notice_free->frames_left = frames;
        notice_free->font = font;
        notice_free->left = left - width / 2;
        notice_free->top = top;
        notice_free->kind = 0;
        notice_free = notice_free->next;
    }
    notices_active = 1;
}

/* Formats a line into the next debug log slot. */
void Message_DebugPrintf(char *format, ...) {
    va_list args;
    int length;

    VA_START(args, format, 1);
    length = vsprintf((char *)debug_log[debug_line_count], format, args);
    glyph_count += length;
    debug_line_count++;
}

/* Adds a debug log line holding only a colour byte, the colour of the lines after it. */
void Message_DebugSetColor(char colour) {
    debug_log[debug_line_count][0] = colour;
    debug_line_count++;
}

/* Where font.tmr, the font texture, is loaded in the font buffer. */
s32 Message_GetFontTexture(s32 font_base) {
    return font_base + 0x180000;
}

/* Where font.txc, the message texts, is loaded in the font buffer. */
s32 Message_GetFontText(s32 font_base) {
    return font_base + 0x1A0400;
}

/* The red, green, blue, alpha ints of text colour `colour`. */
char *Message_GetColor(s32 colour) {
    return (char *)&text_colours[colour];
}

/* Takes every notice off the ring. */
void Message_ClearNotices(void) {
    MessageSlot *slot = notice_shown;

    notices_active = 0;
    notice_free = slot;
    slot->frames_left = 0;
}

/* Takes the message box off the screen at once. */
void Message_CloseBox(void) {
    box_open = 0;
}

/* Adds the debug log lines to the text packet in the debug font, 10 pixels a character, 16 a line. */
static void Message_DrawDebugLog(void) {
    int i;
    int colour = 0;
    int pos_x = 0;
    int pos_y = 0;
    u8 *line;
    u8 first;

    for (i = 0; i < debug_line_count; i++) {
        line = debug_log[i];
        first = *line;
        if (first < 0x20) {
            colour = (char)first;
        } else {
            for (; *line != 0; line++) {
                char character = *line;

                if (character == '\n') {
                    Message_DrawGlyph(pos_x, pos_y, 0, colour, 0x80, 2, 1);
                    pos_x = 0;
                    pos_y += 16;
                } else {
                    Message_DrawGlyph(pos_x, pos_y, character - ' ', colour, 0x80, 2, 1);
                    pos_x += 10;
                }
            }
        }
    }
}

/* Expands a message's glyph stream: copies it to `out` up to the 0xFFFF end mark, replacing each number code
   by the next argument printed in the width the code gives. Returns the number of glyphs written, not counting
   control codes. */
static int Message_Format(u16 *format, u16 *out, va_list args) {
    u8 digits[0x20];
    int count;
    int width;
    int digit_count;
    int i;

    count = 0;
    while (*format != 0xFFFF) {
        if (*format & (GLYPH_NUMBER | GLYPH_RAW_NUMBER)) {
            width = (*format & 0xF00) >> 8;
            digit_count = func_001D0758(va_arg(args, int), digits);
            if (*format & GLYPH_NUMBER) {
                if (digit_count < width) {
                    for (i = 0; i < width - digit_count; i++) {
                        *out++ = 0x800;
                        count++;
                    }
                }
                for (i = digit_count - 1; i >= 0; i--) {
                    *out++ = (s8)digits[i] + 0x810;
                    count++;
                }
            } else if (*format & GLYPH_RAW_NUMBER) {
                if (digit_count < width) {
                    for (i = 0; i < width - digit_count; i++) {
                        *out++ = 0x4AF;
                        count++;
                    }
                }
                for (i = digit_count - 1; i >= 0; i--) {
                    *out++ = (s8)digits[i];
                    count++;
                }
            }
        } else {
            *out = *format;
            if (!(*out++ & 0xF000)) {
                count++;
            }
        }
        format++;
    }
    *out = *format;
    return count;
}

/* Draws `count` glyphs of a message. kind 0 is the message box, typed out glyph by glyph; kind 1 the shown
   notice, fading in and out. */
static void Message_DrawGlyphs(char kind, int font, int left, int top, s16 *text, int count) {
    int pos_x = left;
    int pos_y = top;
    int shown = 0;
    int i;
    int drawn = 0;
    int colour = 0;
    int line = 0;
    int alpha;

    if (message_box.centred != 0 && kind == 0) {
        pos_x = message_box.line_offsets[0];
    }
    if (kind == 1) {
        MessageSlot *slot = notice_shown;
        int fade_start = slot->frames - 8;
        int fade = slot->frames_left - fade_start;

        alpha = 0x80;
        if (fade > 0) {
            alpha -= fade << 4;
        } else if (slot->frames_left < 8) {
            alpha = slot->frames_left << 4;
        }
        memset(glyph_alphas, alpha, 0x200); // match: fills the bytes of a table of ints, so each entry is alpha * 0x01010101
    } else if (kind == 0) {
        if (message_box.closing == 1) {
            // match: the same byte fill of the int table
            memset(glyph_alphas, 0x80 - ((8 - message_box.close_timer) << 4), 0x200);
        } else {
            if (message_box.delay > 0) {
                shown = (message_box.reveal_frames - 8) / message_box.delay;
            }
            if (shown >= 0) {
                for (i = 0; i < shown; i++) {
                    glyph_alphas[i] = 0x80;
                }
            }
            for (i = 0; i < 8; i++) {
                if (shown + i >= 0) {
                    glyph_alphas[shown + i] = 0x80 - i * 0x10;
                }
            }
        }
    }
    while (drawn < count) {
        if (*text == GLYPH_NEW_LINE) {
            line++;
            if (message_box.centred != 0) {
                pos_x = message_box.line_offsets[line];
            } else {
                pos_x = left;
            }
            pos_y += font_metrics[font].line_height;
        } else if (*text & GLYPH_COLOR) {
            colour = (*text & 0xF00) >> 8;
        } else if (*text & GLYPH_HALF_WIDTH) {
            Message_DrawGlyph(pos_x, pos_y, *text - GLYPH_HALF_WIDTH, colour, glyph_alphas[drawn], 1, font);
            drawn++;
            pos_x += font_metrics[font].advance / 2;
        } else {
            Message_DrawGlyph(pos_x, pos_y, *text, colour, glyph_alphas[drawn], 0, font);
            drawn++;
            pos_x += font_metrics[font].advance;
        }
        text++;
    }
}

/* Adds one glyph to the text packet: PRIM, the TEX0 write of its texture page, the colour and the two corners
   (ST and XYZ each). */
static void Message_DrawGlyph(int pos_x, int pos_y, int glyph, int colour, int alpha, int kind, int font) {
    s32 corners[2][4];
    f32 tex[2][2];
    s32 first_page = glyph < 0x271;
    s32 index;
    s32 column;
    s32 row;
    s32 i;
    s32 *corner;
    f32 *coord;
    TextColor *rgb;

    index = glyph;
    if (!first_page) {
        index -= 0x271;
    }
    column = index % glyph_sheets[kind].columns;
    tex[0][0] = column * glyph_sheets[kind].step_s + glyph_sheets[kind].base_s;
    row = index / glyph_sheets[kind].columns;
    tex[0][1] = row * glyph_sheets[kind].step_t + glyph_sheets[kind].base_t;
    tex[1][0] = tex[0][0] + glyph_sheets[kind].size_s;
    tex[1][1] = tex[0][1] + glyph_sheets[kind].size_t;
    corners[0][0] = (pos_x + 0x800 + glyph_boxes[font][kind].left) << 4;
    corners[0][1] = (pos_y + 0x800 + glyph_boxes[font][kind].top) << 4;
    corners[1][0] = (pos_x + 0x800 + glyph_boxes[font][kind].right) << 4;
    corners[1][1] = (pos_y + 0x800 + glyph_boxes[font][kind].bottom) << 4;
    corners[0][2] = -1;
    corners[1][2] = -1;
    corners[0][3] = 0;
    corners[1][3] = 0;
    *glyph_packet++ = 0x56;
    *glyph_packet++ = 0;
    *glyph_packet++ = 0;
    *glyph_packet++ = 0;
    if (first_page && kind == 0) {
        *((GsRegisterPair *)glyph_packet)++ = font_tex0[0];
    } else {
        *((GsRegisterPair *)glyph_packet)++ = font_tex0[1];
    }
    rgb = &text_colours[colour];
    *glyph_packet++ = rgb->red;
    *glyph_packet++ = rgb->green;
    *glyph_packet++ = rgb->blue;
    *glyph_packet++ = alpha;
    corner = corners[0];
    coord = tex[0];
    for (i = 0; i < 2; i++) {
        *(f32 *)glyph_packet++ = coord[0];
        *(f32 *)glyph_packet++ = coord[1];
        *(f32 *)glyph_packet++ = 1.0f;
        *(f32 *)glyph_packet++ = 0.0f;
        *glyph_packet++ = corner[0];
        *glyph_packet++ = corner[1];
        *glyph_packet++ = corner[2];
        *glyph_packet++ = corner[3];
        corner += 4;
        coord += 2;
    }
}

/* Width of one glyph in `font`: the advance, half of it for a half-width glyph, nothing for a control code. */
static s32 Message_GetGlyphWidth(u16 *glyph, s32 font) {
    u32 advance; // match: unsigned, so the halving is spelled out

    if (!(*glyph & 0xF000)) {
        if (!(*glyph & GLYPH_HALF_WIDTH)) {
            return font_metrics[font].advance;
        }
        advance = font_metrics[font].advance;
        return (s32)(advance + (advance >> 31)) >> 1;
    }
    return 0;
}

/* Width of a glyph stream (ended by -1) in `font`. */
static int Message_GetTextWidth(s16 *text, int font) {
    int width = 0;

    while (*text != GLYPH_END) {
        width += Message_GetGlyphWidth((u16 *)text, font);
        text++;
    }
    return width;
}

/* Lays out one page of the message box: stores the centring offset of each line and returns the page's last
   glyph (the new line that ends it, or the end mark). */
static s16 *Message_LayoutPage(s16 *text, s32 font) {
    s32 line = 0;
    MessageBox *box = &message_box; // match: the box address is formed before the metrics table
    s32 *offsets = box->line_offsets;
    s32 width = 0;
    s16 glyph;

    while (TRUE) {
        width += Message_GetGlyphWidth((u16 *)text, font);
        glyph = *text;
        if (glyph == GLYPH_NEW_LINE) {
            line++;
            *offsets++ = 0x140 - width / 2;
            width = 0;
            if (line >= font_metrics[font].lines_per_page) {
                break;
            }
        }
        if (glyph == GLYPH_END) {
            *offsets = 0x140 - width / 2;
            break;
        }
        text++;
    }
    return text;
}

/* The decimal digits of `value` into `digits`, least significant first; gives how many. */
int func_001D0758(int value, u8 *digits) {
    int n;
    int i;

    *digits = value % 10;
    n = value / 10;
    i = 1;
    while (n != 0) {
        digits[i] = n % 10;
        n /= 10;
        i++;
    }
    return i;
}

/* Address of message message_id: the offset table entry after the table's first word, then the text it points to. */
int Message_GetText(int message_id) {
    s32 *offset = (s32 *)Message_GetFontText(D_00446F38 + (message_id + 1) * 4);

    return Message_GetFontText(D_00446F38 + *offset);
}
