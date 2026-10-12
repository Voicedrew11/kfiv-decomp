#include "kf_packet.h"

extern int D_00446F78;
extern GsDrawEnv D_00432D80;
extern GsDrawEnv D_00432E70;
int FlushCache(int);

void func_0019B580(void) {
    GsDrawPacket packet;

    packet.tag[0] = 0x8008;
    packet.tag[1] = 0x10000000;
    packet.tag[2] = 0xE;
    packet.tag[3] = 0;
    packet.env = *(D_00446F78 != 0 ? &D_00432D80 : &D_00432E70);
    FlushCache(0);
    sceGsPutDrawEnv(&packet);
}
