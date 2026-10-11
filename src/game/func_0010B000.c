#include "common.h"

extern u8 D_004681A0[0x9C40];
extern void *memset(void *dst, int c, u32 n);

void func_0010B000(void) {
    memset(D_004681A0, 0, 0x9C40);
}
