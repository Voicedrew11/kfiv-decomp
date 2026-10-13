#include "eeregs.h"
#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_message.h"

extern u64 *D_00420DC0[];
extern GsCornerBits D_00446D60;
extern GsCornerBits D_00446D68;
extern u8 D_00446D70[4];

static inline s32 text_color(u16 code) {
    return (code & 0xFFF) >> 8;
}

/* Like func_001FE9A8 (one line of text at the pen position D_00446D60, at most `limit` glyphs), but centred:
   the text is first measured, and the pen starts half of the unused width (a cell is half of D_00446D68's
   width) further right. Returns the number of glyphs drawn. */
s32 func_001FF428(u16 *text, s32 limit) {
    GsCornerBits top_left;
    GsCornerBits bottom_right;
    s32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    f32 scale[2];
    u8 color[4];
    u32 *packet;
    u32 *cursor;
    char *palette;
    s32 count = 0; // match: all nine cleared here; most are dead, but the number of insns decides a register tie
    s32 i = 0;
    s32 column = 0;
    s32 length = 0;
    s32 offset = 0;
    s32 next_column = 0;
    s32 glyph = 0;
    s32 glyph_row = 0;
    s32 glyph_column = 0;

    packet = func_001FBE70();
    cursor = packet; // match: set here, used after the loop below
    top_left.pos_x = bottom_right.pos_x = D_00446D60.pos_x;
    top_left.pos_y = D_00446D60.pos_y;
    bottom_right.pos_y = D_00446D60.pos_y + D_00446D68.pos_y;
    top_left.depth = bottom_right.depth = D_00446D60.depth;
    for (length = 0; text[length] != 0xFFFF; length++) {
    }
    if (limit < length) {
        length = limit;
    }
    length = limit - length; // match: reused for the number of cells left over
    offset = ((s32)(D_00446D68.pos_x * length) >> 1) / 2;
    top_left.pos_x += offset;
    bottom_right.pos_x += offset;
    color[0] = D_00446D70[0];
    color[1] = D_00446D70[1];
    color[2] = D_00446D70[2];
    color[3] = D_00446D70[3];
    limit++;
    *cursor++ = 0x8000;
    *cursor++ = 0x84000000;
    *cursor++ = 0xF5252106;
    *cursor++ = 0xFFFFFFFF;
    for (i = 0, count = 0, column = 0; text[i] != 0xFFFF; i++, count++) {
        if (text[i] & 0x4000) {
            count--;
            palette = KF_Message_GetColor(text_color(text[i]));
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
        if (text[i] & 0x800) {
            glyph = text[i]; // match: read again as an int, then two statements
            glyph = (glyph & 0x7FF) + 0x47E;
            glyph_column = glyph % 50;
            glyph_row = glyph / 50;
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *D_00420DC0[1];
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
            bottom_right.pos_x += (s32)D_00446D68.pos_x >> 1;
        } else {
            glyph = text[i];
            top_left.pos_x = bottom_right.pos_x;
            *((u64 *)cursor)++ = *D_00420DC0[glyph / 625];
            glyph %= 625;
            glyph_column = glyph % 25;
            glyph_row = glyph / 25;
            bottom_right.pos_x += D_00446D68.pos_x;
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
    packet[0] = count | 0x8000;
    D_00446D60.pos_y = bottom_right.pos_y;
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    return count;
}
