#include "common.h"

extern u8 D_0034C350[50][8];
extern u8 D_0034C4E0[50];

void func_00103858(int id) {
    int area;
    u8 mask;

    if (id == 0x138B) {
        func_00103858(0x138C);
        func_00103858(0x138D);
        func_00103858(0x138E);
        return;
    }
    if (id == 0x13F0) {
        func_00103858(0x13F1);
        func_00103858(0x13F2);
        func_00103858(0x13F3);
        return;
    }
    if (id == 0x1421) {
        func_00103858(0x1422);
        func_00103858(0x1423);
        func_00103858(0x1424);
        return;
    }
    area = (id - 3000) / 50;
    id %= 50;
    mask = 1 << (id % 8);
    if (id < 3) {
        D_0034C350[area][0] = (D_0034C350[area][0] & 0xF8) | mask;
    } else {
        D_0034C350[area][id / 8] |= mask;
    }
    if (D_0034C4E0[area] == 0) {
        D_0034C4E0[area] = 1;
    }
}
