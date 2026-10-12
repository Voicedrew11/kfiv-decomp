#include "common.h"

/* Rotates a normal by the matrix in vf5..vf7 and clamps each component at zero. */
void func_002075C0(void *normal, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "vmulax.xyzw ACC, $vf5, $vf16x\n"
        "vmadday.xyzw ACC, $vf6, $vf16y\n"
        "vmaddz.xyzw $vf17, $vf7, $vf16z\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmaxx.xyzw $vf17, $vf17, $vf0x\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf17, 0x0(%1)"
        : : "r"(normal), "r"(result));
}
