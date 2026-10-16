/* A 256x256 draw environment the game can switch to and back. */
#include "kf_packet.h"
#include "kf_draw_env.h"

void *memset(void *, int, u32);
int FlushCache(int);
extern GsDataBlock offscreen_square_corners; /* two corners of a 256x256 square around (2048, 2048), 12.4 fixed point */
extern int context1_buffer_half;
extern GsDrawEnv game_dbuff_draw0;          /* game_dbuff.draw0 */
extern GsDrawEnv game_dbuff_draw1;          /* game_dbuff.draw1 */

static void func_0019B4A0(void);
static void func_0019B4A8(void);
static void DrawEnv_PutScreen(void);
static void func_0019B618(void);

/* Puts a 256x256 16-bit draw environment at frame page 0x1C2, waits, and puts the normal one back. The copy of
   offscreen_square_corners in front of the packet is not sent. Nothing calls it by name. */
void DrawEnv_PutOffscreenAndBack(void) {
    GsDrawFrame frame;
    GsDrawPacket *packet;
    memset(&frame, 0, 0x10);
    frame.block = offscreen_square_corners;
    packet = &frame.packet;
    frame.packet.tag[0] = 0x8008;
    frame.packet.tag[1] = 0x10000000;
    frame.packet.tag[2] = 0xE;
    frame.packet.tag[3] = 0;
    sceGsSetDefDrawEnv(&frame.packet.env, 0xA, 0x100, 0x100, 0, 0);
    frame.packet.env.scissor = (((s64)0xFF0000 << 16) | 0xFF) << 16;
    frame.packet.env.frame = 0x0A0401C2;
    FlushCache(0);
    sceGsPutDrawEnv(packet);
    sceGsSyncPath(0, 0);
    frame.packet.env = *(context1_buffer_half != 0 ? &game_dbuff_draw0 : &game_dbuff_draw1);
    FlushCache(0);
    sceGsPutDrawEnv(packet);
}

/* Does nothing. */
static void func_0019B4A0(void) {
}

/* Does nothing. */
static void func_0019B4A8(void) {
}

/* Puts the draw environment of DrawEnv_PutOffscreenAndBack and waits, without putting the normal one back. */
void DrawEnv_PutOffscreen(void) {
    GsDrawFrame frame;
    GsDrawPacket *packet;
    memset(&frame, 0, 0x10);
    frame.block = offscreen_square_corners;
    packet = &frame.packet;
    frame.packet.tag[0] = 0x8008;
    frame.packet.tag[1] = 0x10000000;
    frame.packet.tag[2] = 0xE;
    frame.packet.tag[3] = 0;
    sceGsSetDefDrawEnv(&frame.packet.env, 0xA, 0x100, 0x100, 0, 0);
    frame.packet.env.scissor = (((s64)0xFF0000 << 16) | 0xFF) << 16;
    frame.packet.env.frame = 0x0A0401C2;
    FlushCache(0);
    sceGsPutDrawEnv(packet);
    sceGsSyncPath(0, 0);
}

/* Puts the normal draw environment of the current frame buffer back. */
static void DrawEnv_PutScreen(void) {
    GsDrawPacket packet;

    packet.tag[0] = 0x8008;
    packet.tag[1] = 0x10000000;
    packet.tag[2] = 0xE;
    packet.tag[3] = 0;
    packet.env = *(context1_buffer_half != 0 ? &game_dbuff_draw0 : &game_dbuff_draw1);
    FlushCache(0);
    sceGsPutDrawEnv(&packet);
}

/* Does nothing. */
static void func_0019B618(void) {
}
