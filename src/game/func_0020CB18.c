#include "common.h"

/* Rotates the three axis rows of a matrix by the rotation part of another (3x3 product) with VU0. */
void func_0020CB18(void *dest, void *rotation, void *matrix) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x10(%1)\n"
        "lqc2 $vf3, 0x20(%1)\n"
        "lqc2 $vf4, 0x30(%1)\n"
        "lqc2 $vf22, 0x0(%2)\n"
        "lqc2 $vf23, 0x10(%2)\n"
        "lqc2 $vf24, 0x20(%2)\n"
        "lqc2 $vf25, 0x30(%2)\n"
        "vmulax.xyz ACC, $vf1, $vf22x\n"
        "vmadday.xyz ACC, $vf2, $vf22y\n"
        "vmaddz.xyz $vf22, $vf3, $vf22z\n"
        "vmulax.xyz ACC, $vf1, $vf23x\n"
        "vmadday.xyz ACC, $vf2, $vf23y\n"
        "vmaddz.xyz $vf23, $vf3, $vf23z\n"
        "vmulax.xyz ACC, $vf1, $vf24x\n"
        "vmadday.xyz ACC, $vf2, $vf24y\n"
        "vmaddz.xyz $vf24, $vf3, $vf24z\n"
        "sqc2 $vf22, 0x0(%0)\n"
        "sqc2 $vf23, 0x10(%0)\n"
        "sqc2 $vf24, 0x20(%0)\n"
        : : "r"(dest), "r"(rotation), "r"(matrix));
}
