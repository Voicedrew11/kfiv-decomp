#include "common.h"
#include "kf_packet.h"

extern SpritePacket D_005DD760[];

SpritePacket *func_002052C0(SpritePacket *src_packet, int amount) {
    float ratio = (float)amount / (float)src_packet->unk41;
    int offset;

    D_005DD760[0] = *src_packet;
    offset = (int)(ratio * 320.0f) * 0x10;
    D_005DD760[0].xyz0.X += offset;
    D_005DD760[0].xyz1.X += offset;
    return D_005DD760;
}
