#include "common.h"
#include "kf_packet.h"

extern SpritePacket D_005DD700;

/* Copies a sprite packet to D_005DD700 and sets the copy's alpha. */
void func_00205270(SpritePacket *src_packet, int alpha) {
    D_005DD700 = *src_packet;
    D_005DD700.alpha = alpha;
}
