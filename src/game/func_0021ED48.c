#include "kf_movie.h"
#include <eeregs.h>
#include <libgraph.h>

extern u32 D_01E50100[];            /* packet buffer */

int func_0021EF38(int, s32 *, u32);
int func_0021F040(int, s64, s32, s32, s32, s32, s32, s32);
int func_0021F3F0(s64, u32, u32, u32, u32); // match: a 64-bit last parameter adds a move at each call

/* Fills both fields' frame buffers with one color. */
void func_0021ED48(MovieDisplay *display, u32 color) {
    s32 rect[4];
    s32 packet_start = (u32)D_01E50100 & 0x0FFFFFFF;
    s32 packet;

    rect[0] = 0;
    rect[1] = 0;
    rect[2] = display->width << 4;
    rect[3] = (((display->height + 15) / 16) * 16 * 2) << 4;
    packet = func_0021F040(packet_start | 0x20000000, 0xE, 1, 0, 0, 0, 1, 6);
    packet = func_0021F3F0(packet, 0, display->width, 0, display->height << 1);
    packet = func_0021EF38(packet, rect, color);
    func_0021F3F0(packet, 0, display->width - 1, 0, display->height - 1);
    *D2_MADR = packet_start;
    *D2_QWC = 7;
    *D2_CHCR = 0x101;
    sceGsSyncPath(0, 0);
}
