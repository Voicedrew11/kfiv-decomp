#include "kf_packet.h"
#include "kf_player.h"

extern LevelRecord D_003679A0[];
extern PlayerItemDef D_0041B9B8[];
extern s32 D_00446E28;
extern SpritePacket D_005DDCA0;

int func_001FC820(int);

/* Copies the packet and tints it blue when the number it stands for is above what the item D_00446E28 gives, red when below. */
SpritePacket *func_00206450(SpritePacket *packet) {
    int current;
    int base;

    D_005DDCA0 = *packet;
    current = func_001FC820(packet->unk34);
    switch (packet->unk34) {
    case 0x2D:
        base = (D_003679A0[D_00446E28].unk00[0] + D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[0]) * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x2E:
        base = (D_003679A0[D_00446E28].unk00[1] + D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[1]) * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x2F:
        base = (D_003679A0[D_00446E28].unk00[2] + D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[2]) * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x30:
        base = D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[3] * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x31:
        base = D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[4] * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x32:
        base = D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[5] * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x33:
        base = D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[6] * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x34:
        base = D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[7] * D_003679A0[D_00446E28].level / 100;
        break;
    case 0x35:
        base = D_0041B9B8[D_00446E28].unk00[D_003679A0[D_00446E28].unk14].values[8] * D_003679A0[D_00446E28].level / 100;
        break;
    default:
        base = 0;
        break;
    }
    if (current - base > 0) {
        D_005DDCA0.red = 0x40;
        D_005DDCA0.green = 0x40;
        D_005DDCA0.blue = 0xFF;
    } else if (current - base < 0) {
        D_005DDCA0.red = 0xFF;
        D_005DDCA0.green = D_005DDCA0.blue = 0x20;
    }
    return &D_005DDCA0;
}
