#include "common.h"

/* Converts a float vector to integers. */
void func_00216C18(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vftoi0.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}
