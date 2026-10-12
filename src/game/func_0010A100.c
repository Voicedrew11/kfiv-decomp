#include "kf_packet.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

/* Queue a textured screen sprite. */
void func_0010A100(void *sprite_arg) {
    ScreenSprite *sprite = sprite_arg;
    ScreenSpriteVertex *vertex;
    u32 *packet_start;
    u32 *packet;
    s32 prim;

    prim = sprite->prim;
    packet_start = packet = func_00109990(0xD);
    *packet++ = 0x1000000B;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C09C000;
    *(u64 *)packet = ((s64)(prim | 6) << 47) | 0x8000400000008001L;
    packet += 2;
    *(u64 *)packet = 0x513513EE;
    packet += 2;
    *(u64 *)packet = sprite->tex0;
    packet += 2;
    *packet++ = 6;
    *packet++ = 0;
    *(u64 *)packet = sprite->tex1;
    packet += 2;
    *packet++ = 0x14;
    *packet++ = 0;
    vertex = &sprite->vertex[0];
    *packet++ = vertex->s;
    *packet++ = vertex->t;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = sprite->rgba[0];
    *packet++ = sprite->rgba[1];
    *packet++ = sprite->rgba[2];
    *packet++ = sprite->rgba[3];
    *packet++ = vertex->x;
    *packet++ = vertex->y;
    *packet++ = vertex->z;
    *packet++ = 1;
    vertex = &sprite->vertex[1];
    *packet++ = vertex->s;
    *packet++ = vertex->t;
    *packet++ = 0;
    *packet++ = 0;
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
