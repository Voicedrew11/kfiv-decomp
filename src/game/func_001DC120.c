#include "common.h"

extern s32 D_00421050[12];

/* Puts the value that goes with a key code into the first empty entry slot. */
void func_001DC120(s32 key) {
    s32 slot;

    for (slot = 0; slot < 12; slot++) {
        if (D_00421050[slot] < 0) {
            break;
        }
    }
    if (slot < 12) {
        switch (key) {
        case 0x31:
            D_00421050[slot] = 0;
            break;
        case 0x30:
            D_00421050[slot] = 1;
            break;
        case 0x2F:
            D_00421050[slot] = 2;
            break;
        case 0x34:
            D_00421050[slot] = 3;
            break;
        case 0x32:
            D_00421050[slot] = 4;
            break;
        case 0x3A:
            D_00421050[slot] = 5;
            break;
        case 0x39:
            D_00421050[slot] = 6;
            break;
        case 0x38:
            D_00421050[slot] = 7;
            break;
        case 0x37:
            D_00421050[slot] = 8;
            break;
        case 0x33:
            D_00421050[slot] = 9;
            break;
        case 0x3B:
            D_00421050[slot] = 10;
            break;
        case 0x35:
            D_00421050[slot] = 11;
            break;
        }
    }
}
