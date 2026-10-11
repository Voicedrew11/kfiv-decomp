#include "common.h"

#define HW_1000B000 (*(volatile u32 *)0x1000B000)
#define HW_1000B010 (*(volatile u32 *)0x1000B010)
#define HW_1000B020 (*(volatile u32 *)0x1000B020)
#define HW_1000B400 (*(volatile u32 *)0x1000B400)
#define HW_1000B410 (*(volatile u32 *)0x1000B410)
#define HW_1000B420 (*(volatile u32 *)0x1000B420)
#define HW_1000B430 (*(volatile u32 *)0x1000B430)
#define HW_10002010 (*(volatile u32 *)0x10002010)
#define HW_10002020 (*(volatile u32 *)0x10002020)

int WaitSema(int);
int SignalSema(int);
void func_0021CD50(int);
void func_0021CDB8(int);

int func_0021D3F8(int *arg0) {
    WaitSema(arg0[0x40 / 4]);
    arg0[0x44 / 4] = 0;
    func_0021CDB8(5);
    arg0[0x1C / 4] = HW_1000B410;
    arg0[0x20 / 4] = HW_1000B430;
    arg0[0x24 / 4] = HW_1000B420;
    arg0[0x28 / 4] = HW_1000B400;
    while (HW_10002010 & 0xF0);
    func_0021CD50(0);
    arg0[0x2C / 4] = HW_1000B010;
    arg0[0x30 / 4] = HW_1000B020;
    arg0[0x34 / 4] = HW_1000B000;
    arg0[0x38 / 4] = HW_10002020;
    arg0[0x3C / 4] = HW_10002010;
    SignalSema(arg0[0x40 / 4]);
    return 1;
}
