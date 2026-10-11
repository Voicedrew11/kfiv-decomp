#include "common.h"

extern int D_004467B0[2];

void func_0011E608(char *arg0)
{
    u8 t;

    t = **(u8 **)(arg0 + 4);
    if ((((u32)(t - 3) < 2U) || t == 0x15)
        && ((u32)((*(int *)(arg0 + 8) - D_004467B0[*(u8 *)(arg0 + 0x10) & 1]) >> 7) >> 7) < 0x200U)
    {
        *(int *)(arg0 + 0x10) |= 0x40000;
    }
}
