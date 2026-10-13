#include "kf_pad.h"

void *memset(void *, int, u32);
extern u16 D_00413DA8[];
extern char D_00413DB8[];

void func_001B5390(void) {
    Pad_Init(0);
    Pad_OpenPort(0, 0);
    Pad_Update();
    memset(D_00413DA8, 0, 0x10);
    memset(D_00413DB8, 0, 0x20);
}
