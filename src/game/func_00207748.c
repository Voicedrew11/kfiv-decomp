#include "common.h"

/* Transforms the point at src by the matrix held in vf12 to vf14 into screen x/y (z is set to 1). */
void func_00207748(void *src, void *dst) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "vmulax.xyzw ACC, $vf12, $vf16x\n"
        "vmadday.xyzw ACC, $vf13, $vf16y\n"
        "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
        "vaddw.xy $vf17, $vf17, $vf0w\n"
        "vmulw.xy $vf17, $vf17, $vf21w\n"
        "vaddw.z $vf17, $vf0, $vf0w\n"
        "sqc2 $vf17, 0(%1)\n"
        : : "r"(src), "r"(dst));
}
