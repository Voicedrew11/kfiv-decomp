#include "kf_packet.h"

extern SpritePacket D_005DD8E0;

/* Copies the packet, opaque (alpha 0x80) when its slot's bit is set in the mask, else clear. */
SpritePacket *func_00205D08(SpritePacket *packet, s32 slot_mask) {
    D_005DD8E0 = *packet;
    if ((slot_mask >> packet->slot) & 1) {
        D_005DD8E0.alpha = 0x80;
    } else {
        D_005DD8E0.alpha = 0;
    }
    return &D_005DD8E0;
}
