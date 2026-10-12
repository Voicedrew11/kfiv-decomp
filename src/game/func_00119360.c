#include "kf_map.h"

extern MapSlot D_0038CD20[];

void func_00119590(int);
void func_00119730(int);
void func_00119770(int);
void func_001197B0(int);
void func_001197F0(int);
void func_00119A48(int);
void func_00119B20(int);
void func_00119D90(int);
void func_00119E18(int);
void func_00119E90(int);
void func_0011A048(int);
void func_0011A0B8(int);
void func_0011A138(int);
void func_0011A210(int);
void func_0011A2B8(int);
void func_0011A370(int);
void func_0011A3E8(int);
void func_0011A428(int); // match: the definition takes nothing, yet it is handed the slot like the others

/* Runs the set-up function of the map in the slot, picked by the map's tag. */
void func_00119360(int slot) {
    switch (D_0038CD20[slot].tag) {
    case 0xB:
        func_00119590(slot);
        break;
    case 0x11:
        func_00119730(slot);
        break;
    case 0x12:
        func_00119770(slot);
        break;
    case 0x13:
        func_001197B0(slot);
        break;
    case 0x14:
        func_001197F0(slot);
        break;
    case 0x18:
        func_00119A48(slot);
        break;
    case 0x19:
        func_00119B20(slot);
        break;
    case 0x1B:
        func_00119D90(slot);
        break;
    case 0x1D:
        func_00119E18(slot);
        break;
    case 0x23:
        func_00119E90(slot);
        break;
    case 0x26:
        func_0011A048(slot);
        break;
    case 0x29:
        func_0011A0B8(slot);
        break;
    case 0x2A:
        func_0011A138(slot);
        break;
    case 0x64:
        func_0011A210(slot);
        break;
    case 0x68:
        func_0011A2B8(slot);
        break;
    case 0x6B:
        func_0011A370(slot);
        break;
    case 0xFB:
        func_0011A3E8(slot);
        break;
    case 0xFC:
        func_0011A428(slot);
        break;
    }
}
