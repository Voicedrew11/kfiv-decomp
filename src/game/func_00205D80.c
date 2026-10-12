#include "kf_packet.h"

extern u8 D_00349680[];
extern s8 D_00349685[];
extern s8 D_00349686[];
extern s8 D_00349688[];
extern s8 D_00349689[];
extern SpritePacket D_005DD940;

/* Gives the packet back when its slot is the chosen one of its pair of options (each pair follows one setting
   byte), NULL when it is the other one. */
SpritePacket *func_00205D80(SpritePacket *packet) {
    D_005DD940 = *packet;
    switch (packet->slot) {
    case 0:
    case 1:
        if (packet->slot == D_00349680[0]) {
            return packet; // match: the packet itself, not the copy just made
        }
        break;
    case 2:
    case 3:
        if (packet->slot - 2 == D_00349685[0]) {
            return packet;
        }
        break;
    case 4:
    case 5:
        if (packet->slot - 4 == D_00349686[0]) {
            return packet;
        }
        break;
    case 6:
    case 7:
        if (packet->slot - 6 == D_00349689[0]) {
            return packet;
        }
        break;
    case 8:
    case 9:
    case 10:
        if (packet->slot - 8 == D_00349688[0]) {
            return packet;
        }
        break;
    }
    return NULL;
}
