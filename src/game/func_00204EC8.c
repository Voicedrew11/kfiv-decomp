#include "kf_packet.h"

extern SpritePacket D_005DD4C0;

/* Copies the packet to D_005DD4C0 with both corners moved left 0xF0, if its slot matches. */
SpritePacket *func_00204EC8(SpritePacket *packet, int slot) {
    if (packet->slot == slot) {
        D_005DD4C0 = *packet;
        D_005DD4C0.xyz0.X -= 0xF0;
        D_005DD4C0.xyz1.X -= 0xF0;
        return &D_005DD4C0;
    }
    return NULL;
}
