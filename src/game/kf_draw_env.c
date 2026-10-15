/* A 256x256 draw environment the game can switch to and back. */
#include "kf_packet.h"
#include "kf_draw_env.h"

void *memset(void *, int, u32);
int FlushCache(int);
extern GsDataBlock D_0043EB90;
extern int D_00446F78;
extern GsDrawEnv D_00432D80;
extern GsDrawEnv D_00432E70;

static void func_0019B4A0(void);
static void func_0019B4A8(void);
static void func_0019B580(void);
static void func_0019B618(void);

/* Puts a 256x256 16-bit draw environment at frame page 0x1C2 with the data block D_0043EB90, waits, and puts
   the normal one back. Nothing calls it by name. */
void func_0019B360(void) {
    GsDrawFrame frame;
    GsDrawPacket *packet;
    memset(&frame, 0, 0x10);
    frame.block = D_0043EB90;
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
    frame.packet.env = *(D_00446F78 != 0 ? &D_00432D80 : &D_00432E70);
    FlushCache(0);
    sceGsPutDrawEnv(packet);
}

/* Does nothing. */
static void func_0019B4A0(void) {
}

/* Does nothing. */
static void func_0019B4A8(void) {
}

/* Puts the draw environment of func_0019B360 and waits, without putting the normal one back. */
void func_0019B4B0(void) {
    GsDrawFrame frame;
    GsDrawPacket *packet;
    memset(&frame, 0, 0x10);
    frame.block = D_0043EB90;
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
static void func_0019B580(void) {
    GsDrawPacket packet;

    packet.tag[0] = 0x8008;
    packet.tag[1] = 0x10000000;
    packet.tag[2] = 0xE;
    packet.tag[3] = 0;
    packet.env = *(D_00446F78 != 0 ? &D_00432D80 : &D_00432E70);
    FlushCache(0);
    sceGsPutDrawEnv(&packet);
}

/* Does nothing. */
static void func_0019B618(void) {
}
