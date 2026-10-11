#include "common.h"

int memset(int, int, int);
extern int D_00446748;
extern int D_0044674C;
extern u32 D_00446750;
extern int D_00446F58;
extern int D_00447018;
extern int D_0044701C;
extern int D_00447020;
extern int D_00447024;
extern int D_00447030;
extern int D_00447034;
extern int D_00447038;
extern int D_0044703C;
extern int D_00447040;
extern int D_00447044;

void func_00109878(int arg0) {
    u32 size;
    int base;
    int half;

    D_00446748 = 0;
    D_0044701C = 0;
    D_00447018 = 0;
    D_00447024 = 0;
    D_00447020 = 0;
    switch (arg0) {
    case 0:
        D_00446750 = 0x280000;
        break;
    case 1:
        D_00446750 = 0x180000;
        break;
    case 2:
        D_00446750 = 0x100000;
        break;
    case 3:
        D_00446750 = 0x80000;
        break;
    }
    size = D_00446750;
    base = D_00446F58;
    D_00447030 = base;
    half = (int)(size + (size >> 31)) >> 1;
    base += half;
    D_00447034 = base;
    memset(D_00447030, 0, half);
    base += half;
    memset(D_00447034, 0, half);
    D_0044674C = 0;
    D_00447038 = D_00447030;
    D_0044703C = 0;
    if (arg0 == 0) {
        D_00447040 = 0;
        D_00447044 = 0;
        return;
    }
    D_00447040 = base;
    D_00447044 = 0x280000 - D_00446750;
}
