#include "common.h"

/* Converts a fixed point vector with 4 fraction bits to floats. */
void func_00216C28(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vitof4.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}
