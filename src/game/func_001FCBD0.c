#include "common.h"
#include <eeregs.h>
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_state.h"

extern MenuGlyphCell D_00446D58[];
extern u64 *D_00420DC0[];

int func_001FC820(int);

/* Draws a sprite of kind 2 or 3, a number: unk34 (kind 2: what func_001FC820 makes of it) in decimal,
   right-aligned in unk33 glyphs and filled with blanks (glyph 0x49), or, when unk33 is 0, as three digits,
   glyph 0xD and one more digit; one textured quad per glyph between the sprite's two corners, each 6 pixels
   narrower than its share. Nothing is drawn for a negative number. */
int func_001FCBD0(KindObject *object) { // match: returns int, though no path sets a value
    SpritePacket *sprite = (SpritePacket *)object;
    f32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    MenuGlyphCell cell = D_00446D58[0];
    u16 glyphs[16];
    GsPackedWord first;
    GsPackedWord second;
    GsPackedWord step;
    u32 *packet;
    u32 *cursor;
    s32 value;
    s32 count;
    s32 index;
    s32 code;
    s32 column;
    s32 row;
    s32 base;
    s32 left;
    s32 right;

    if (sprite == NULL) {
        return;
    }
    if (sprite->unk31 == 2) {
        value = func_001FC820(sprite->unk34);
    } else {
        value = sprite->unk34;
    }
    if (value < 0) {
        return;
    }
    if (value <= 999999999) {
        if (sprite->unk33 != 0) {
            index = sprite->unk33;
            do {
                glyphs[index - 1] = value % 10;
                value /= 10;
                index--;
                if (value == 0) {
                    break;
                }
            } while (index > 0);
            while (index > 0) {
                glyphs[index - 1] = 0x49;
                index--;
            }
        } else {
            glyphs[4] = value % 10;
            value /= 10;
            glyphs[3] = 0xD;
            index = 2;
            do {
                glyphs[index] = value % 10;
                index--;
                value /= 10;
                if (value == 0) {
                    break;
                }
            } while (index >= 0);
            while (index >= 0) {
                glyphs[index] = 0x49;
                index--;
            }
        }
    } else {
        for (index = sprite->unk33; index > 0; index--) {
            glyphs[index] = 0; // match: one too far up; glyphs[0] stays unset
        }
    }
    packet = func_001FBE70();
    count = 5;
    if (sprite->unk33 != 0) {
        count = sprite->unk33;
    }
    first.f.lo16 = sprite->xyz0.X;
    second.f.lo16 = sprite->xyz1.X;
    first.f.mid16 = sprite->xyz0.Y;
    second.f.mid16 = sprite->xyz1.Y;
    first.f.mid24 = sprite->xyz0.Z;
    second.f.mid24 = sprite->xyz0.Z; // match: the first corner's depth, not the second's
    step.f.lo16 = ((s32)second.f.lo16 - (s32)first.f.lo16) / count;
    step.f.mid16 = ((s32)second.f.mid16 - (s32)first.f.mid16) / count; // match: worked out, never used
    cursor = packet;
    *cursor++ = count | 0x8000;
    *cursor++ = 0xC4000000;
    *cursor++ = 0x25252106;
    *cursor++ = 0xFFFFF525;
    for (index = 0; index < count; index++) {
        code = glyphs[index];
        column = code % 25;
        row = code / 25;
        *((u64 *)cursor)++ = *D_00420DC0[0];
        base = (s32)first.f.lo16 + (count << 5);
        left = index * (s32)step.f.lo16 - ((index * 6) << 4) + (count << 6);
        right = (index + 1) * (s32)step.f.lo16 - ((index * 6) << 4) + (count << 6);
        *cursor++ = 0x54;
        *cursor++ = 0;
        *cursor++ = sprite->red | (sprite->green << 8) | (sprite->blue << 16) | (sprite->alpha << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = (f32)column * cell.step[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * cell.step[1];
        *cursor++ = (base + left) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)column * cell.step[0];
        *((f32 *)cursor)++ = (f32)row * cell.step[1];
        *cursor++ = (base + left) | ((s32)first.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell.step[0];
        *((f32 *)cursor)++ = (f32)(row + 1) * cell.step[1];
        *cursor++ = (base + right) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell.step[0];
        *((f32 *)cursor)++ = (f32)row * cell.step[1];
        *cursor++ = (base + right) | ((s32)first.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *cursor++ = 0;
        *cursor++ = 0;
    }
    while (*D2_CHCR & 0x100) {
    }
    *D2_QWC = count * 6 + 1;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
}
