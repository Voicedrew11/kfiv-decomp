#include "kf_text_log.h"
#include "kf_game.h"
#include <eeregs.h>
#include <libdma.h>
#include <libgraph.h>

int FlushCache(int);
int vsprintf(char *, const char *, char *);
u32 strlen(char *);
void *memcpy(void *, const void *, u32);

extern char D_004337E0[0x40];
extern char D_00433820[];

extern TextLog D_01E0E9C0;
extern u32 D_01E0E940[8];
extern int D_01E0E960[0x18];
extern TextLogPacket D_01E0F210;

/* match: retail lays out D_01E0E940, D_01E0E960, D_01E0E9C0 and D_01E0F210 as one .bss stretch. */
__asm__(
    "\t.section .bss\n"
    "\t.align 4\n"
    "\t.globl D_01E0E940\n"
    "D_01E0E940:\n"
    "\t.space 0x20\n"
    "\t.globl D_01E0E960\n"
    "D_01E0E960:\n"
    "\t.space 0x60\n"
    "\t.globl D_01E0E9C0\n"
    "D_01E0E9C0:\n"
    "\t.space 0x850\n"
    "\t.globl D_01E0F210\n"
    "D_01E0F210:\n"
    "\t.space 0xE08\n");

/* EE EABI varargs: the register save area a1..a7 is spilled to, as in libsdr.c */
typedef char *va_list;
#define va_start(ap, last) ((ap) = (char *)__builtin_next_arg(last) - 0x38)

void KF_TextLog_PutChar(char character, int pos_x, int pos_y, int pos_z);
void KF_TextLog_SendGsRegs(void);

/* Loads the text log's font (D_00433820, 256 by 128 at 4 bits) and its 16-colour palette (D_004337E0) into
   GS memory at the given pages, then tells the GS to flush its texture cache. */
void KF_TextLog_LoadFont(int font_base, int palette_base) {
    sceGsLoadImage image;
    u32 *packet;

    sceDmaGetChan(2);
    D_01E0E9C0.font_base = font_base;
    D_01E0E9C0.font_buffer_width = 4;
    D_01E0E9C0.palette_base = palette_base;
    sceGsSetDefLoadImage(&image, D_01E0E9C0.font_base, D_01E0E9C0.font_buffer_width, 0x14, 0, 0, 0x100, 0x80);
    FlushCache(0);
    sceGsExecLoadImage(&image, D_00433820);
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    sceGsSetDefLoadImage(&image, D_01E0E9C0.palette_base, 1, 0, 0, 0, 8, 2);
    FlushCache(0);
    sceGsExecLoadImage(&image, D_004337E0);
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    packet = (u32 *)((u32)D_01E0E940 | 0x20000000);
    *packet++ = 0x8001;
    *packet++ = 0x10004000;
    *packet++ = 0xE;
    *packet++ = 0;
    *packet++ = 0xFFFFFFFF;
    *packet++ = 0;
    *packet++ = 3;
    *packet = 0;
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = 2;
    *D2_MADR = (int)D_01E0E940;
    *D2_CHCR = 0x100;
}

/* Places the text log's window (x, y, width and height in frame pixels, on a screen of the given size whose
   centre is at the GS offset), makes the text white and empties the log. */
void KF_TextLog_SetWindow(int window_x, int window_y, int width, int height, int offset_x, int offset_y,
                          int screen_width, int screen_height, int depth) {
    TextLog *log = &D_01E0E9C0;

    log->left = window_x + offset_x - screen_width / 2;
    log->top = window_y / 2 + offset_y - screen_height / 2;
    log->width = width;
    log->height = height / 2;
    log->depth = depth;
    log->red = log->green = log->blue = 0xFF;
    if (width < 0x10) {
        log->width = 0x10;
    }
    log->used = 0;
    log->packet = (TextLogGlyph *)&D_01E0F210;
}

/* Queues one character of the text log's font (16 characters a row, starting at '!') at a GS position.
   Anything up to the space character is drawn as character 0x80. */
void KF_TextLog_PutChar(char character, int pos_x, int pos_y, int pos_z) {
    TextLog *log = &D_01E0E9C0;
    TextLogGlyph *glyph = log->packet;
    int code;
    int tex_u;
    int tex_v;

    code = character;
    if (code <= 0x20) {
        code = 0x80;
    }
    glyph->corners[0].red = log->red;
    glyph->corners[0].green = log->green;
    glyph->corners[0].blue = log->blue;
    glyph->corners[0].alpha = 0x80;
    tex_u = (((code - 0x21) & 0xF) << 8) | 0x10;
    tex_v = (((code - 0x21) / 16) << 8) | 0x10;
    glyph->corners[0].tex_u = tex_u;
    glyph->corners[0].tex_v = tex_v;
    glyph->corners[0].pos_x = pos_x << 4;
    glyph->corners[0].pos_y = pos_y << 4;
    glyph->corners[0].pos_z = pos_z << 4;
    glyph->corners[1].red = log->red;
    glyph->corners[1].green = log->green;
    glyph->corners[1].blue = log->blue;
    glyph->corners[1].alpha = 0x80;
    glyph->corners[1].tex_u = tex_u + 0x100;
    glyph->corners[1].tex_v = tex_v + 0x100;
    glyph->corners[1].pos_x = (pos_x + 16) << 4;
    glyph->corners[1].pos_y = (pos_y + 8) << 4;
    glyph->corners[1].pos_z = pos_z << 4;
    log->packet = glyph + 1;
}

/* Sends the GS registers the text log draws with: its font texture (256 by 128, 4-bit, palette in the texture
   page), point sampling, alpha blending. Each register is a 64-bit value followed by its address. */
void KF_TextLog_SendGsRegs(void) {
    int *packet;

    packet = (int *)((u32)D_01E0E960 | 0x20000000);
    *packet++ = 0x8005;
    *packet++ = 0x10004000;
    *packet++ = 0xE;
    *packet++ = 0;
    /* TEX0: TBP0, TBW, PSM, TW, TH, TCC, TFX, CBP, CPSM, CSM, CLD */
    *(u64 *)packet = (u64)D_01E0E9C0.font_base | ((u64)D_01E0E9C0.font_buffer_width << 14) | ((u64)0x14 << 20) |
                     ((u64)8 << 26) | ((u64)7 << 30) | ((u64)1 << 34) | ((u64)0 << 35) |
                     ((u64)D_01E0E9C0.palette_base << 37) | ((u64)0 << 51) | ((u64)0 << 55) | ((u64)4 << 61);
    packet += 2;
    *(u64 *)packet = 6;
    packet += 2;
    *(u64 *)packet = 0x60;
    packet += 2;
    *(u64 *)packet = 0x14;
    packet += 2;
    *(u64 *)packet = 0;
    packet += 2;
    *(u64 *)packet = 0x49;
    packet += 2;
    *(u64 *)packet = 0x44;
    packet += 2;
    *(u64 *)packet = 0x42;
    packet += 2;
    *(u64 *)packet = 1;
    ((u64 *)packet)[1] = 0x1C;
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = 6;
    *D2_MADR = (int)D_01E0E960;
    *D2_CHCR = 0x100;
}

/* printf-style append to the text log, cut off at its 0x800 bytes. */
void KF_TextLog_AppendV(char *format, char *args) {
    char line[0x400];
    TextLog *log;
    char *text;
    int length;
    int used;

    vsprintf(line, format, args);
    length = strlen(line);
    log = &D_01E0E9C0;
    used = log->used;
    if (length + used >= 0x801) {
        length = 0x800 - used;
    }
    text = log->text;
    memcpy(text + used, line, length);
    log->used += length;
}

/* printf into the text log. */
void KF_TextLog_Printf(char *format, ...) {
    va_list args;

    va_start(args, format);
    KF_TextLog_AppendV(format, args);
}

/* Draws the text log in its window, wrapping at the right edge and stopping at the bottom, then empties it. */
void KF_TextLog_Flush(void) {
    TextLogPacket *packet = &D_01E0F210;
    int vertex_count = 0;
    int remaining = D_01E0E9C0.used;
    char *text = D_01E0E9C0.text;
    int pos_x;
    int pos_y;

    D_01E0E9C0.packet = packet->glyphs;
    pos_x = D_01E0E9C0.left;
    pos_y = D_01E0E9C0.top;
    while (remaining-- != 0) {
        if (*text == '\n') {
            pos_y += 8;
            pos_x = D_01E0E9C0.left;
        } else {
            KF_TextLog_PutChar(*text, pos_x, pos_y, D_01E0E9C0.depth);
            pos_x += 16;
            vertex_count += 2;
            if (D_01E0E9C0.left + D_01E0E9C0.width - 16 < pos_x) {
                pos_x = D_01E0E9C0.left;
                pos_y += 8;
            }
        }
        if (D_01E0E9C0.top + D_01E0E9C0.height - 8 < pos_y) {
            break;
        }
        text++;
    }
    if (vertex_count != 0) {
        KF_TextLog_SendGsRegs();
        packet->tag[0] = vertex_count | 0x8000;
        packet->tag[1] = 0x30AB4000;
        packet->tag[2] = 0x413;
        packet->tag[3] = 0;
        FlushCache(0);
        while (*D2_CHCR & 0x100) {
        }
        *D2_QWC = ((int)D_01E0E9C0.packet - (int)&D_01E0F210) >> 4;
        *D2_MADR = (u32)&D_01E0F210 & 0x7FFFFFFF;
        *D2_CHCR = 0x100;
    }
    D_01E0E9C0.used = 0;
}

/* Sets the text log's colour from three ints: red, green, blue. */
void KF_TextLog_SetColor(int *colour) {
    TextLog *log = &D_01E0E9C0;

    log->red = colour[0];
    log->green = colour[1];
    log->blue = colour[2];
}
