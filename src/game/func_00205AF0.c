#include "kf_effect.h"
#include "kf_packet.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern FxTableEntry D_00402DA8[];
extern SpritePacket D_005DD880;

int func_001FC820(int);

/* Copies the packet and tints it blue when the number it stands for is above what the selected magic gives, red when below. */
SpritePacket *func_00205AF0(SpritePacket *packet) {
    int current;
    int base;

    D_005DD880 = *packet;
    current = func_001FC820(packet->unk34);
    switch (packet->unk34) {
    case 0x2C:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[0];
        break;
    case 0x2D:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[1];
        break;
    case 0x2E:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[2];
        break;
    case 0x2F:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[3];
        break;
    case 0x30:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[4];
        break;
    case 0x31:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[5];
        break;
    case 0x32:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[6];
        break;
    case 0x33:
        base = D_00402DA8[D_00413F80.unk3D].hit_checks[0].strengths.unk00[7];
        break;
    case 0x34:
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        D_005DD880.red = 0x40;
        D_005DD880.green = 0x40;
        D_005DD880.blue = 0xFF;
    } else if (current - base < 0) {
        D_005DD880.red = 0xFF;
        D_005DD880.green = D_005DD880.blue = 0x20;
    }
    return &D_005DD880;
}
