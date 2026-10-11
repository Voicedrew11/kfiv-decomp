#include "common.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

void func_00120698(char *arg0) {
    if ((D_0038CC74[0] >> 1) & 1) {
        if (D_00420D50[0] == arg0) {
            arg0[0x11] = 2;
            *(int *)(arg0 + 0x10) = (*(int *)(arg0 + 0x10) & 0xFFFDFFFF) | ((((*(int *)(arg0 + 0x10)) >> 17) ^ 1) & 1) << 17;
        }
    }
}
