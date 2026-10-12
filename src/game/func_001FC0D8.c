#include "common.h"
#include <eeregs.h>
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_state.h"

extern MenuGlyphCell D_00446D58[];
extern u16 D_0042A078[];
extern u16 D_00428C50[];
extern u64 *D_00420DC0[];

/* Draws a sprite of kind 1, a word: the glyph codes D_00428C50 holds from D_0042A078[unk32] up to the next
   entry (less one), one textured quad per glyph, spread evenly between the sprite's two corners, in the
   sprite's colour; then sends the packet to the GS over DMA channel 2. */
void func_001FC0D8(KindObject *object) {
    SpritePacket *sprite = (SpritePacket *)object;
    f32 unused[8]; // match: 0x20 bytes of frame that nothing uses
    MenuGlyphCell cell = D_00446D58[0];
    GsPackedWord first;
    GsPackedWord second;
    GsPackedWord step;
    u32 *packet;
    u32 *cursor;
    s32 start;
    s32 count;
    s32 index;
    s32 code;
    s32 column;
    s32 row;

    if (sprite == NULL) {
        return;
    }
    packet = func_001FBE70();
    count = D_0042A078[sprite->unk32 + 1] - D_0042A078[sprite->unk32] - 1; // match: before `start`; 0 traps in the divisions below
    start = D_0042A078[sprite->unk32];
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
    for (index = 0; index < count && (code = D_00428C50[start + index]) != 0xFFFF; index++) {
        *((u64 *)cursor)++ = *D_00420DC0[code / 625];
        code %= 625;
        column = code % 25;
        row = code / 25;
        *cursor++ = 0x54;
        *cursor++ = 0;
        *cursor++ = sprite->red | (sprite->green << 8) | (sprite->blue << 16) | (sprite->alpha << 24);
        *((f32 *)cursor)++ = 1.0f;
        *((f32 *)cursor)++ = (f32)column * cell.step[0] + 0.009765625f;
        *((f32 *)cursor)++ = (f32)(row + 1) * cell.step[1];
        *cursor++ = ((s32)first.f.lo16 + index * (s32)step.f.lo16) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)column * cell.step[0] + 0.009765625f;
        *((f32 *)cursor)++ = (f32)row * cell.step[1];
        *cursor++ = ((s32)first.f.lo16 + index * (s32)step.f.lo16) | ((s32)first.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell.step[0] - 0.009765625f;
        *((f32 *)cursor)++ = (f32)(row + 1) * cell.step[1];
        *cursor++ = ((index + 1) * (s32)step.f.lo16 + (s32)first.f.lo16) | ((s32)second.f.mid16 << 16);
        *cursor++ = ((GsPackedWord *)&sprite->xyz0)->f.mid24;
        *((f32 *)cursor)++ = (f32)(column + 1) * cell.step[0] - 0.009765625f;
        *((f32 *)cursor)++ = (f32)row * cell.step[1];
        *cursor++ = ((index + 1) * (s32)step.f.lo16 + (s32)first.f.lo16) | ((s32)first.f.mid16 << 16);
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
