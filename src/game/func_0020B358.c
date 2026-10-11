#include "common.h"

typedef u32 u128 __attribute__((mode(TI)));

void func_0020B358(char *src, char *dst, u32 n) {
    int q = n >> 4;

    while (q--) {
        *(u128 *)dst = *(u128 *)src;
        src += 0x10;
        dst += 0x10;
    }
    q = n & 0xF;
    while (q--) {
        *dst++ = *src++;
    }
}
