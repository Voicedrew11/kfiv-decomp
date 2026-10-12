#include "common.h"

/* dst = left + right (x, y, z only). */
void func_00216C48(void *dst, void *left, void *right) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vadd.xyz $vf16, $vf16, $vf17\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(left), "r"(right) : "memory");
}
