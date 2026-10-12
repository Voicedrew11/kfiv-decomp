#include "kf_packet.h"

void *memset(void *, int, u32);
int FlushCache(int);
extern GsDataBlock D_0043EB90;
extern int D_00446F78;
extern GsDrawEnv D_00432D80;
extern GsDrawEnv D_00432E70;

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
