#include "common.h"

/* dst = the four given matrix rows applied to vector: row0 * x + row1 * y + row2 * z + row3 * w (xyz only). */
void func_00216CF0(void *dst, void *row0, void *row1, void *row2, void *row3, float *vector) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%5)\n"
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x0(%2)\n"
        "lqc2 $vf3, 0x0(%3)\n"
        "lqc2 $vf4, 0x0(%4)\n"
        "vmulax.xyz ACC, $vf1, $vf16x\n"
        "vmadday.xyz ACC, $vf2, $vf16y\n"
        "vmaddaz.xyz ACC, $vf3, $vf16z\n"
        "vmaddw.xyz $vf16, $vf4, $vf16w\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(row0), "r"(row1), "r"(row2), "r"(row3), "r"(vector) : "memory");
}
