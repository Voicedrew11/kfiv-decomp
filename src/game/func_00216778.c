#include "common.h"

/* Copies one 16-byte vector (the same code as libvu0's sceVu0CopyVector). */
void func_00216778(void *dst, void *src) {
    __asm__ volatile(
        "lq $6, 0x0(%1)\n"
        "sq $6, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "$6", "memory");
}
