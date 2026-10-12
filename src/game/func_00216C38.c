#include "common.h"

/* Converts an integer vector to floats. */
void func_00216C38(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vitof0.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}
