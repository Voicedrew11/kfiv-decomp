#include "kf_packet.h"
#include <eeregs.h>


/* Sends a fifteen-quadword packet: GS settings, one textured sprite, GS settings again. */
void func_001D6A00(void) {
    GsPacketWord *packet;
    GsPacketWord *cursor;

    packet = (GsPacketWord *)func_001FBE70();
    cursor = packet;
    cursor++;
    (cursor++)->as_int = 0x10000000;
    (cursor++)->as_int = 0xE;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x30000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x47;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x68;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x56;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    *(u64 *)cursor = 0x620012240;
    cursor += 2;
    (cursor++)->as_int = 6;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x80202020;
    (cursor++)->as_float = 1.0f;
    (cursor++)->as_int = 1;
    (cursor++)->as_int = 0;
    cursor[1].as_float = cursor[0].as_float = 0.01f;
    cursor += 2;
    (cursor++)->as_int = 2;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x7B708310;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 5;
    (cursor++)->as_int = 0;
    cursor[1].as_float = cursor[0].as_float = 0.99f;
    cursor += 2;
    (cursor++)->as_int = 2;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x83709310;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 5;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x50000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x47;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0x80;
    (cursor++)->as_int = 0x42;
    (cursor++)->as_int = 0;
    packet[0].as_int = 0x8000 | ((cursor - packet) / 4 - 1);
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
}
