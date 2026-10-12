#include "kf_game.h"

extern s32 D_00420BE0[][4];
extern MessageSlot *D_004471E4;
extern s32 D_005C50C8[0x80];
extern MessageBox D_005C58E8;

void *memset(void *, int, u32);
void func_001D0298(int, int, int, int, int, int, int);

/* Draws `count` glyphs of a message. kind 0 is the message box, typed out glyph by glyph; kind 1 a message
   of the ring, fading in and out. D_005C50C8 holds the alpha of each glyph. */
void func_001CFFA0(char kind, int font, int left, int top, s16 *text, int count) {
    int pos_x = left;
    int pos_y = top;
    int shown = 0;
    int i;
    int drawn = 0;
    int colour = 0;
    int line = 0;
    int alpha;

    if (D_005C58E8.unk34 != 0 && kind == 0) {
        pos_x = D_005C58E8.line_offsets[0];
    }
    if (kind == 1) {
        MessageSlot *slot = D_004471E4;
        int fade_start = slot->unk08 - 8;
        int fade = slot->unk04 - fade_start;

        alpha = 0x80;
        if (fade > 0) {
            alpha -= fade << 4;
        } else if (slot->unk04 < 8) {
            alpha = slot->unk04 << 4;
        }
        memset(D_005C50C8, alpha, 0x200); // match: fills the bytes of a table of ints, so each entry is alpha * 0x01010101
    } else if (kind == 0) {
        if (D_005C58E8.closing == 1) {
            // match: the same byte fill of the int table
            memset(D_005C50C8, 0x80 - ((8 - D_005C58E8.close_timer) << 4), 0x200);
        } else {
            if (D_005C58E8.delay > 0) {
                shown = (D_005C58E8.unk14 - 8) / D_005C58E8.delay;
            }
            if (shown >= 0) {
                for (i = 0; i < shown; i++) {
                    D_005C50C8[i] = 0x80;
                }
            }
            for (i = 0; i < 8; i++) {
                if (shown + i >= 0) {
                    D_005C50C8[shown + i] = 0x80 - i * 0x10;
                }
            }
        }
    }
    while (drawn < count) {
        if (*text == -0x8000) {
            line++;
            if (D_005C58E8.unk34 != 0) {
                pos_x = D_005C58E8.line_offsets[line];
            } else {
                pos_x = left;
            }
            pos_y += D_00420BE0[font][1];
        } else if (*text & 0x4000) {
            colour = (*text & 0xF00) >> 8;
        } else if (*text & 0x800) {
            func_001D0298(pos_x, pos_y, *text - 0x800, colour, D_005C50C8[drawn], 1, font);
            drawn++;
            pos_x += D_00420BE0[font][0] / 2;
        } else {
            func_001D0298(pos_x, pos_y, *text, colour, D_005C50C8[drawn], 0, font);
            drawn++;
            pos_x += D_00420BE0[font][0];
        }
        text++;
    }
}
