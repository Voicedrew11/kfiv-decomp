#include "kf_packet.h"
#include "kf_player.h"

extern LevelRecord D_003682F0[];
extern LevelRecordDef D_0041E118[];
extern SpritePacket D_005DDD00;

int func_001FC820(int);

/* Copies the packet and tints it blue when the number it stands for is above what item `index` gives, red when below. */
SpritePacket *func_00206880(SpritePacket *packet, int index) {
    int current;
    int base;

    D_005DDD00 = *packet;
    current = func_001FC820(packet->unk34);
    switch (packet->unk34) {
    case 0x2D:
        base = (D_003682F0[index].unk00[0] + D_0041E118[index].unk00[0]) * D_003682F0[index].level / 100;
        break;
    case 0x2E:
        base = (D_003682F0[index].unk00[1] + D_0041E118[index].unk00[1]) * D_003682F0[index].level / 100;
        break;
    case 0x2F:
        base = (D_003682F0[index].unk00[2] + D_0041E118[index].unk00[2]) * D_003682F0[index].level / 100;
        break;
    case 0x30:
        base = D_0041E118[index].unk00[3] * D_003682F0[index].level / 100;
        break;
    case 0x31:
        base = D_0041E118[index].unk00[4] * D_003682F0[index].level / 100;
        break;
    case 0x32:
        base = D_0041E118[index].unk00[5] * D_003682F0[index].level / 100;
        break;
    case 0x33:
        base = D_0041E118[index].unk00[6] * D_003682F0[index].level / 100;
        break;
    case 0x34:
        base = D_0041E118[index].unk00[7] * D_003682F0[index].level / 100;
        break;
    case 0x35:
        base = D_0041E118[index].unk00[8] * D_003682F0[index].level / 100;
        break;
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        D_005DDD00.red = 0x40;
        D_005DDD00.green = 0x40;
        D_005DDD00.blue = 0xFF;
    } else if (current - base < 0) {
        D_005DDD00.red = 0xFF;
        D_005DDD00.green = D_005DDD00.blue = 0x20;
    }
    return &D_005DDD00;
}
