#include "common.h"

/* Converts a float vector to fixed point with 4 fraction bits. */
void func_00216C08(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vftoi4.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}
