#include "common.h"

/* Transforms the vector already in vf16 by the matrix in vf12..vf14 into texture coordinates. */
void func_00207700(void *result) {
    __asm__ volatile(
        "vmulax.xyzw ACC, $vf12, $vf16x\n"
        "vmadday.xyzw ACC, $vf13, $vf16y\n"
        "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
        "vaddw.xy $vf17, $vf17, $vf0w\n"
        "vmulw.xy $vf17, $vf17, $vf21w\n"
        "vaddw.z $vf17, $vf0, $vf0w\n"
        "sqc2 $vf17, 0x0(%0)"
        : : "r"(result));
}
