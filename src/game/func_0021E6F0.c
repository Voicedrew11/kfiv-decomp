#include "kf_movie.h"
#include <eeregs.h>
#include <libgraph.h>

extern volatile s32 D_00437824[];            /* vblank handler: on */
extern volatile s32 D_00437828[];
extern volatile s32 D_0043782C[];
extern volatile s32 D_00437830[];
extern volatile s32 D_00437834[];
extern MovieThreadState D_00437850;
extern u32 D_01E4FFC0[];            /* packet buffer */

int FlushCache(int);
int func_0021F040(int, s64, s32, s32, s32, s32, s32, s32);
int func_0021F330(s64, u32, u32, u32, u64);
int func_0021F378(int, u32, u32, u32, u32, u32, u32, u32, u32);
int func_0021F3F0(s64, u32, u32, u32, u32); // match: a 64-bit last parameter adds a move at each call
int func_0021F430(int, u32, u32);
int func_0021F460(int, u32);
int func_0021F500(s64, u64, u64, u64, u64, u64, u64);

#define ROUND_UP(value, unit) ((((value) + (unit) - 1) / (unit)) * (unit))

/* Sets the movie's display up: two fields of half the height, one after the other in GS memory. */
void func_0021E6F0(MovieDisplay *display, s32 screen_x, s32 screen_y, s32 width, s32 height, s32 picture_width, s32 picture_height) {
    s32 field_height = height / 2;
    s32 buffer_width = (width + 63) / 64;
    s32 packet_start;
    s32 packet;

    D_00437824[0] = 0;
    D_00437828[0] = 0;
    D_0043782C[0] = 0;
    D_00437830[0] = 0;
    D_00437834[0] = 0;
    display->screen_x = screen_x;
    display->width = width;
    display->frame_base[1] = (buffer_width * 64 * ROUND_UP(field_height, 32)) >> 11;
    display->height = height;
    display->picture_width = picture_width;
    display->picture_height = picture_height;
    display->frame_base[0] = 0;
    display->screen_y = screen_y;
    sceGsResetGraph(0, 1, D_00437850.unkD4, 1);
    sceGsSetDefDispEnv((sceGsDispEnv *)display, 0, width, field_height, D_00437850.unkD6, D_00437850.unkD8);
    display->dispfb = (display->dispfb & ~0x7E00) | ((buffer_width & 0x3F) << 9);
    FlushCache(0);
    sceGsPutDispEnv(display);
    packet_start = (u32)D_01E4FFC0 & 0x0FFFFFFF;
    packet = func_0021F040(packet_start | 0x20000000, 0xE, 1, 0, 0, 0, 1, 6);
    packet = func_0021F460(packet, 1);
    packet = func_0021F330(packet, display->frame_base[0], ROUND_UP(display->width, 64) / 64, 0, 0);
    packet = func_0021F378(packet, 0, 0, 0, 0, 0, 0, 0, 0);
    packet = func_0021F430(packet, 0, 0);
    packet = func_0021F3F0(packet, 0, display->width - 1, 0, display->height - 1);
    packet = func_0021F500(packet, 1, 1, 0, 0, 0, 0);
    *D2_MADR = packet_start;
    *D2_QWC = 7;
    *D2_CHCR = 0x101;
    sceGsSyncPath(0, 0);
}
