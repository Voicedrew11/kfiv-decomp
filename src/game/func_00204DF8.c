#include "kf_packet.h"

extern SpritePacket D_005DD400;

SpritePacket *func_00204DF8(SpritePacket *src, s32 wanted_kind) {
    if (src->slot == wanted_kind) {
        D_005DD400 = *src;
        return &D_005DD400;
    }
    return NULL;
}
