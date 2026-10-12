#include "common.h"
#include "kf_packet.h"

extern SpritePacket D_005DD640;

/* Copies a sprite packet to D_005DD640 and shifts the copy up by amount / unk41 of the screen height. */
SpritePacket *func_002050F8(SpritePacket *src_packet, int amount) {
    float ratio = (float)amount / (float)src_packet->unk41;
    int offset;

    D_005DD640 = *src_packet;
    offset = (int)(ratio * 224.0f) * 0x10;
    D_005DD640.xyz0.Y -= offset;
    D_005DD640.xyz1.Y -= offset;
    // match: the two clamps below can never run (Y is an unsigned field), yet they leave retail's 0x10 frame
    if (D_005DD640.xyz0.Y < 0) {
        D_005DD640.xyz0.Y = 0;
    }
    if (D_005DD640.xyz1.Y < 0) {
        D_005DD640.xyz1.Y = 0;
    }
    return &D_005DD640;
}
