#include "kf_packet.h"
#include <eeregs.h>

extern int D_00446F78;


/* Sends a five-quadword packet that draws one sprite in the grey level given, into the frame buffer D_00446F78 selects. */
void func_001E5070(s32 level) {
    GsPacketWord *packet;
    GsPacketWord *cursor;
    u32 colour;

    packet = (GsPacketWord *)func_001FBE70();
    packet[0].as_int = 0x8001;
    packet[1].as_int = 0x740B4000;
    packet[2].as_int = 0xF5252160;
    packet[3].as_int = 0;
    packet[4].as_int = 0x16;
    packet[5].as_int = 0;
    // match: the do/while (a macro in the original, most likely) keeps the stores above ahead of the load below
    do {
        cursor = packet + 8;
        if (D_00446F78 == 0) {
            *(u64 *)&packet[6] = 0x668028000;
        } else {
            *(u64 *)&packet[6] = 0x6680288C0;
        }
    } while (0);
    colour = level | 0x80000000;
    colour |= (level << 16) | (level << 8);
    (cursor++)->as_int = colour;
    (cursor++)->as_float = 1.0f;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x79006C00;
    (cursor++)->as_int = 0xFFFFF;
    (cursor++)->as_float = 0.625f;
    (cursor++)->as_float = 0.4375f;
    cursor[0].as_int = 0x86F89400;
    cursor[1].as_int = 0xFFFFF;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 5;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x105;
}
