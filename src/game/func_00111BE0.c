#include "common.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

/* Queues a 12-quadword packet: a three-vertex primitive whose vertices all have 0x8000 at
   offset 0x0E, with the rest left for the caller to fill in. */
void func_00111BE0(void) {
    u32 *packet_start;
    u32 *packet;
    u32 *after_vertices;
    u16 *vertices;

    packet_start = packet = func_00109990(0xC);
    *packet = 0x1000000A;
    packet += 7;
    *packet++ = 0x6C01C000;
    *(u64 *)packet = 0x3000000000008003;
    packet += 2;
    *packet++ = 0x412;
    *packet = 0;
    packet += 4;
    *packet++ = 0x6D0C8001;
    vertices = (u16 *)packet;
    after_vertices = packet + 0x18;
    vertices[0x27] = vertices[0x17] = vertices[7] = 0x8000;
    packet = after_vertices;
    packet[3] = 0x15000000;
    packet += 4;
    func_00109650(packet_start, packet);
}
