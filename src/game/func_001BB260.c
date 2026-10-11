#include "common.h"

int func_001BB308(int);
void func_001BB360(void);
void func_001B57A0(u8, u8, u8);
extern int D_00413FD8[];
extern int D_0044695C;

void func_001BB260(int arg0)
{
    int v;

    if (func_001BB308(arg0) != 0) {
        func_001BB360();
        v = ((arg0 << 7) / (D_00413FD8[0] / 2)) + 0x7F;
        if (v >= 0x100)
            v = 0xFF;
        func_001B57A0(v, 4, 0);
        D_0044695C = 6;
    }
}
