#include "kf_packet.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

/* Queue a flat-coloured (untextured) screen sprite. */
void func_0010A338(void *sprite_arg) {
    ScreenSprite *sprite = sprite_arg;
    ScreenSpriteVertex *vertex;
    u32 *packet_start;
    u32 *packet;
    s32 prim;

    prim = sprite->prim & 0xFFFFFFE9;
    packet_start = packet = func_00109990(9);
    prim |= 6;
    *packet++ = 0x10000007;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C05C000;
    *(u64 *)packet = ((s64)prim << 47) | 0x4000400000008001L;
    packet += 2;
    *(u64 *)packet = 0x5151;
    packet += 2;
    vertex = &sprite->vertex[0];
    *packet++ = sprite->rgba[0];
    *packet++ = sprite->rgba[1];
    *packet++ = sprite->rgba[2];
    *packet++ = sprite->rgba[3];
    *packet++ = vertex->x;
    *packet++ = vertex->y;
    *packet++ = vertex->z;
    *packet++ = 1;
    vertex = &sprite->vertex[1];
    *packet++ = sprite->rgba[0];
    *packet++ = sprite->rgba[1];
    *packet++ = sprite->rgba[2];
    *packet++ = sprite->rgba[3];
    *packet++ = vertex->x;
    *packet++ = vertex->y;
    *packet++ = vertex->z;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x15000276;
    func_00109650(packet_start, packet);
}
