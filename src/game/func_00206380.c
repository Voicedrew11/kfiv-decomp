#include "kf_packet.h"

extern SpritePacket D_005DDBE0;

/* Copies the packet and makes it a 24-pixel wide sprite starting at pixel column. */
SpritePacket *func_00206380(SpritePacket *packet, int column) {
    D_005DDBE0 = *packet;
    D_005DDBE0.xyz0.X = column << 4;
    D_005DDBE0.xyz1.X = (column << 4) + 0x180;
    return &D_005DDBE0;
}
