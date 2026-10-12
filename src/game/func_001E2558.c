#include "common.h"

extern u16 *D_00440200[12];
extern s8 D_004210B0[12];
extern s8 D_004210C0[12];

/* Fills both 12-entry tables with the index that goes with each slot's pad button bit. */
void func_001E2558(void) {
    s32 i;

    for (i = 0; i < 12; i++) {
        switch (*D_00440200[i]) {
        case 0x10:
            D_004210C0[i] = D_004210B0[i] = 0;
            break;
        case 0x40:
            D_004210C0[i] = D_004210B0[i] = 1;
            break;
        case 0x20:
            D_004210C0[i] = D_004210B0[i] = 2;
            break;
        case 0x80:
            D_004210C0[i] = D_004210B0[i] = 3;
            break;
        case 0x400:
            D_004210C0[i] = D_004210B0[i] = 4;
            break;
        case 0x100:
            D_004210C0[i] = D_004210B0[i] = 5;
            break;
        case 0x1000:
            D_004210C0[i] = D_004210B0[i] = 6;
            break;
        case 0x4000:
            D_004210C0[i] = D_004210B0[i] = 7;
            break;
        case 0x2000:
            D_004210C0[i] = D_004210B0[i] = 8;
            break;
        case 0x8000:
            D_004210C0[i] = D_004210B0[i] = 9;
            break;
        case 0x800:
            D_004210C0[i] = D_004210B0[i] = 10;
            break;
        case 0x200:
            D_004210C0[i] = D_004210B0[i] = 11;
            break;
        }
    }
}
