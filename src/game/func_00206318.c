#include "kf_packet.h"

extern SpritePacket D_005DDB80;

/* Copies the packet and greys its colour to 0x40 when its slot matches. */
SpritePacket *func_00206318(SpritePacket *packet, int slot) {
    D_005DDB80 = *packet;
    if (packet->slot == slot) {
        D_005DDB80.red = 0x40;
        D_005DDB80.green = 0x40;
        D_005DDB80.blue = 0x40;
    }
    return &D_005DDB80;
}
