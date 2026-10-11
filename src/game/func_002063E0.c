#include "kf_packet.h"

extern SpritePacket D_005DDC40;

SpritePacket *func_002063E0(SpritePacket *source, int row) {
    int step;

    D_005DDC40 = *source;
    step = (row * 11) << 4;
    D_005DDC40.xyz0.Y += step;
    D_005DDC40.xyz1.Y += step;
    return &D_005DDC40;
}
