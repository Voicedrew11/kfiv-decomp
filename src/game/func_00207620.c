#include "common.h"

/* Stores the first vector as integers, with w taken from the second vector's w. */
void func_00207620(void *colour, void *alpha_source, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "lqc2 $vf19, 0x0(%1)\n"
        "vadd.xyzw $vf17, $vf0, $vf16\n"
        "vmulw.w $vf17, $vf0, $vf19w\n"
        "vftoi0.xyzw $vf17, $vf17\n"
        "sqc2 $vf17, 0x0(%2)"
        : : "r"(colour), "r"(alpha_source), "r"(result));
}
