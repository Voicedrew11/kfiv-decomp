#include "common.h"

/* Writes a GS packet that copies a rectangle from one buffer to another (BITBLTBUF, TRXPOS, TRXREG, TRXDIR,
   TEXFLUSH) and returns where the next packet goes. */
u32 *func_001174D0(u32 *packet, int end_of_packet, int format, int buffer_width, int width, int height,
                   int source_base, int source_x, int source_y, int dest_base, int dest_x, int dest_y) {
    *((u64 *)packet)++ = ((s64)end_of_packet << 15) | 0x1000000000000005L;
    *((u64 *)packet)++ = 0xE;
    *((u64 *)packet)++ = (s64)source_base | ((s64)buffer_width << 16) | ((s64)format << 24) |
                         ((s64)dest_base << 32) | ((s64)buffer_width << 48) | ((s64)format << 56);
    *packet++ = 0x50;
    *packet++ = 0;
    *((u64 *)packet)++ = (s64)source_x | ((s64)source_y << 16) | ((s64)dest_x << 32) | ((s64)dest_y << 48);
    *packet++ = 0x51;
    *packet++ = 0;
    *((u64 *)packet)++ = (s64)width | ((s64)height << 32);
    *packet++ = 0x52;
    *packet++ = 0;
    *((u64 *)packet)++ = 2;
    *packet++ = 0x53;
    *packet++ = 0;
    *((u64 *)packet)++ = 0xFFFFFFFF;
    *packet++ = 0x3F;
    *packet++ = 0;
    return packet;
}
