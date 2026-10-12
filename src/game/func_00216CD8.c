#include "common.h"

/* dst = lhs x rhs (cross product). */
void func_00216CD8(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vopmula.xyz ACC, $vf16, $vf17\n"
        "vopmsub.xyz $vf16, $vf17, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}
