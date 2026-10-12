#include "common.h"
#include "kf_packet.h"

extern SpritePacket D_005DD5E0[];

/* Copies a sprite packet to D_005DD5E0 and shifts the copy right by amount / unk41 of the screen width. */
SpritePacket *func_00205058(SpritePacket *src_packet, int amount) {
    float ratio = (float)amount / (float)src_packet->unk41;
    int offset;

    D_005DD5E0[0] = *src_packet;
    offset = (int)(ratio * 320.0f) * 0x10;
    D_005DD5E0[0].xyz0.X += offset;
    D_005DD5E0[0].xyz1.X += offset;
    return D_005DD5E0;
}
