#include "common.h"

void *memset(void *, int, u32);
extern s32 D_00447178;
extern s32 D_0044717C;
extern s32 D_00447180;
extern s32 D_00447184;
extern u8 D_0057DB40[];
extern u8 D_0057DF40[];

void func_001C0840(void) {
    memset(D_0057DB40, 0, 0x400);
    memset(D_0057DF40, 0, 0x100);
    D_0044717C = 0;
    D_00447178 = 1;
    D_00447180 = 0;
    D_00447184 = 0;
}
