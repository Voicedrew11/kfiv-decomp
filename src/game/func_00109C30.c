#include "common.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

/* Queues a GS local-to-local image transfer (BITBLTBUF, TRXPOS, TRXREG, TRXDIR). */
void func_00109C30(int source_base, int source_format, int source_width, int source_x, int source_y, int width,
                   int height, int dest_base, int dest_format, int dest_width, int dest_x, int dest_y,
                   int direction) {
    u32 *cursor = func_00109990(8);
    u32 *packet = cursor;

    *cursor++ = 0x10000006;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x51000005;
    *(u64 *)cursor = 0x1000000000008004;
    cursor += 2;
    *(u64 *)cursor = 0xE;
    cursor += 2;
    *(u64 *)cursor = (u64)source_base | ((u64)source_width << 16) | ((u64)source_format << 24) |
                     ((u64)dest_base << 32) | ((u64)dest_width << 48) | ((u64)dest_format << 56);
    cursor += 2;
    *cursor++ = 0x50;
    *cursor++ = 0;
    *(u64 *)cursor = (u64)source_x | ((u64)source_y << 16) | ((u64)dest_x << 32) | ((u64)dest_y << 48) |
                     ((u64)direction << 59);
    cursor += 2;
    *cursor++ = 0x51;
    *cursor++ = 0;
    *(u64 *)cursor = (u64)width | ((u64)height << 32);
    cursor += 2;
    *cursor++ = 0x52;
    *cursor++ = 0;
    *(u64 *)cursor = 2;
    cursor += 2;
    *cursor++ = 0x53;
    *cursor++ = 0;
    func_00109650(packet, cursor);
}
