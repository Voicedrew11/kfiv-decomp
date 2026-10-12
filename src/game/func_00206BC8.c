#include "kf_packet.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern SpritePacket D_005DDD60;

/* Copies the packet and tints it red while the player's 0xA8 value is below 0xA4. */
SpritePacket *func_00206BC8(SpritePacket *packet) {
    D_005DDD60 = *packet;
    if (D_00413F80.unkA4 > D_00413F80.unkA8) {
        D_005DDD60.red = 0xFF;
        D_005DDD60.green = D_005DDD60.blue = 0x20;
    }
    return &D_005DDD60;
}
