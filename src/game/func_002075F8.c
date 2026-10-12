#include "common.h"

/* Applies the colour matrix in vf8..vf11 to the light factors, scales by the colour and stores integers. */
void func_002075F8(void *factors, void *colour, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "lqc2 $vf19, 0x0(%1)\n"
        "vmulax.xyzw ACC, $vf8, $vf16x\n"
        "vmadday.xyzw ACC, $vf9, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf10, $vf16z\n"
        "vmaddw.xyzw $vf17, $vf11, $vf0w\n"
        "vmul.xyzw $vf17, $vf17, $vf19\n"
        "vftoi0.xyzw $vf17, $vf17\n"
        "sqc2 $vf17, 0x0(%2)"
        : : "r"(factors), "r"(colour), "r"(result));
}
