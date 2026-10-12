#include "common.h"

/* dst = matrix * src (all four components). */
void func_00216BC0(void *dst, void *matrix, void *src) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x10(%1)\n"
        "lqc2 $vf3, 0x20(%1)\n"
        "lqc2 $vf4, 0x30(%1)\n"
        "lqc2 $vf16, 0x0(%2)\n"
        "vmulax.xyzw ACC, $vf1, $vf16x\n"
        "vmadday.xyzw ACC, $vf2, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(matrix), "r"(src) : "memory");
}
