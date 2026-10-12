#include "common.h"

/* dst = src * scale, all four components. */
void func_00216CC0(void *dst, void *src, float scale) {
    __asm__ volatile(
        "mfc1 $2, %2\n"
        "lqc2 $vf16, 0x0(%1)\n"
        "qmtc2 $2, $vf31\n"
        "vmulx.xyzw $vf16, $vf16, $vf31x\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src), "f"(scale) : "$2", "memory");
}
