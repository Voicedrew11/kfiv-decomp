#include "kf_packet.h"
#include <eeregs.h>


/* Sends a fifteen-quadword packet of GS settings around one flat sprite. */
void func_001D67A0(void) {
    GsPacketWord *packet;
    GsPacketWord *cursor;

    packet = (GsPacketWord *)func_001FBE70();
    cursor = packet;
    cursor++;
    (cursor++)->as_int = 0x10000000;
    (cursor++)->as_int = 0xE;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x40112;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x4D;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xA000132;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x4F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x7800;
    (cursor++)->as_int = 0x7800;
    (cursor++)->as_int = 0x19;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xFF000F;
    (cursor++)->as_int = 0xFF000E;
    (cursor++)->as_int = 0x41;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x30000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x48;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xA9;
    (cursor++)->as_int = 0x60;
    (cursor++)->as_int = 0x43;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x246;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x80000000;
    (cursor++)->as_float = 1.0f;
    (cursor++)->as_int = 0x1;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x77F077F0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x5;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x98108810;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x5;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x50000;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x48;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x3F;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x44;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0x43;
    (cursor++)->as_int = 0;
    (cursor++)->as_int = 0xF0000F;
    (cursor++)->as_int = 0xF2000E;
    (cursor++)->as_int = 0x41;
    (cursor++)->as_int = 0;
    packet[0].as_int = 0x8000 | ((cursor - packet) / 4 - 1);
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = (cursor - packet) / 4;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
}
