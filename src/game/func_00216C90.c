#include "common.h"

/* dst = lhs - rhs, all four components. */
void func_00216C90(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vsub.xyzw $vf16, $vf16, $vf17\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}
