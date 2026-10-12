#include "common.h"

/* VU0 macro-mode blend: dst = a + (b * t) across the xyz lanes, written from inline asm. */
void func_00194F18(void *dst, void *src_a, void *src_b, f32 scale) {
    __asm__ volatile(
        "mfc1 $2, %3\n"
        "qmtc2.ni $2, $vf3\n"
        "vsubx.w $vf4, $vf0, $vf3x\n"
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x0(%2)\n"
        "vmulax.xyz ACC, $vf2, $vf3x\n"
        "vmaddw.xyz $vf1, $vf1, $vf4w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf1, 0x0(%0)\n"
        : : "r"(dst), "r"(src_a), "r"(src_b), "f"(scale) : "$2");
}
