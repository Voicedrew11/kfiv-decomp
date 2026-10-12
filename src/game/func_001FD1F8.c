#include "common.h"
#include <eeregs.h>
#include "kf_gs.h"
#include "kf_packet.h"

extern u64 *D_00420DC0[];

/* Draws a line of text at a screen position (origin: x, y, z and the line height in 16ths): one textured
   quad per glyph code, built in a packet buffer and sent to the GS over DMA channel 2. The text ends at
   0xFFFF or at any code with bit 0x8000. Codes with bit 0x800 are half-width glyphs of the first font page
   (50 columns of 5/256, from cell 0x47E on); the others are full-width ones, 625 to a page, 25 to a row. */
void func_001FD1F8(u16 *text, GsPackedWord origin) {
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

    packet = func_001FBE70();
    top.f.lo16 = bottom.f.lo16 = origin.f.lo16;
    top.f.mid16 = origin.f.mid16;
    top.f.mid24 = origin.f.mid24;
    bottom.f.mid16 = origin.f.mid16 + (u8)origin.f.hi8 * 16;
    bottom.f.mid24 = origin.f.mid24;
    cursor = packet;
    *cursor++ = glyph_count | 0x8000; // match: never set; the real count is stored after the loop
    *cursor++ = 0xC4000000;
    *cursor++ = 0x25252106;
    *cursor++ = 0xFFFFF525;
    index = 0;
    count = 0;
    while (text[index] != 0xFFFF && !(text[index] & 0x8000)) {
        if (text[index] & 0x800) {
            glyph = text[index]; // match: read again into a 16-bit local
            code = glyph & 0x7FF; // match: two statements
            code += 0x47E;
            column = code % 50;
            row = code / 50;
            *((u64 *)cursor)++ = *D_00420DC0[0];
            top.f.lo16 = bottom.f.lo16;
            bottom.f.lo16 += 0xA0;
            scale[0] = 0.01953125f;
            scale[1] = 0.0390625f;
        } else {
            code = text[index];
            *((u64 *)cursor)++ = *D_00420DC0[code / 625];
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
    *packet = count | 0x8000;
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = count * 6 + 1;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
}
