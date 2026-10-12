#include "eeregs.h"
#include "kf_gs.h"

extern u64 *D_00420DC0[];
extern GsCornerBits D_00446D60;
extern GsCornerBits D_00446D68;
extern u8 D_00446D70[4];
extern s32 D_00446D78;
extern u32 D_005DDF00[2][0x3700];

char *func_001CFC68(s32);
int FlushCache(int);

/* Draws a string of glyphs at the pen position D_00446D60, wrapping at `limit` glyphs a line and at the line
   breaks in the text, and sends the packet to the GS; returns the number of glyphs drawn. */
s32 func_001FE350(u16 *text, s32 limit) {
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

    packet = D_005DDF00[D_00446D78];
    D_00446D78 = D_00446D78 == 0;
    color[0] = D_00446D70[0];
    color[1] = D_00446D70[1];
    color[2] = D_00446D70[2];
    color[3] = D_00446D70[3];
    top_left.pos_x = bottom_right.pos_x = D_00446D60.pos_x;
    top_left.pos_y = D_00446D60.pos_y;
    bottom_right.pos_y = D_00446D60.pos_y + D_00446D68.pos_y;
    top_left.depth = bottom_right.depth = D_00446D60.depth;
    line_end = limit + 1;
    cursor = packet + 4;
    packet[0] = 0x8000;
    packet[1] = 0x84000000;
    packet[2] = 0xF5252106;
    packet[3] = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & 0x4000) {
            /* a colour change, not a glyph */
            count--;
            color_index = text[i];
            palette = func_001CFC68((color_index & 0xFFF) >> 8);
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
                top_left.pos_x = bottom_right.pos_x = D_00446D60.pos_x;
                top_left.pos_y += D_00446D68.pos_y;
                bottom_right.pos_y += D_00446D68.pos_y;
                i--;
            }
            count--;
            column = 0;
            continue;
        }
        if ((text[i] & 0x8000) && column != 0) {
            /* a line break in the text */
            top_left.pos_x = bottom_right.pos_x = D_00446D60.pos_x;
            top_left.pos_y += D_00446D68.pos_y;
            bottom_right.pos_y += D_00446D68.pos_y;
            count--;
            column = 0;
            continue;
        }
        column = next_column;
        if (text[i] & 0x800) {
            /* a half-width glyph */
            glyph = text[i]; // match: three statements on the one variable, not one expression
            glyph &= 0x7FF;
            glyph += 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += (s32)D_00446D68.pos_x >> 1;
            *((u64 *)cursor)++ = *D_00420DC0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
        } else {
            glyph = text[i];
            *((u64 *)cursor)++ = *D_00420DC0[(glyph / 625) % 2]; // match: % 2, not & 1
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            top_left.pos_x = bottom_right.pos_x;
            bottom_right.pos_x += D_00446D68.pos_x;
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
    packet[0] = count | 0x8000;
    D_00446D60.pos_y = bottom_right.pos_y;
    FlushCache(0);
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = (u32)packet & 0x7FFFFFFF;
    *D2_CHCR = 0x100;
    return count;
}
