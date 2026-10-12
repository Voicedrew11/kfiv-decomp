#include "kf_packet.h"

extern SpritePacket D_005DDE40;

/* Copies the packet and gives the copy the alpha passed in. */
SpritePacket *func_00207428(SpritePacket *packet, int alpha) {
    D_005DDE40 = *packet;
    D_005DDE40.alpha = alpha;
    return &D_005DDE40;
}
