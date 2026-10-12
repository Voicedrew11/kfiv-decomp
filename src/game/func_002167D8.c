#include "common.h"

/* Loads two vectors into VU0 registers vf12 and vf13. */
void func_002167D8(void *first, void *second) {
    __asm__ volatile(
        "lqc2 $vf12, 0x0(%0)\n"
        "lqc2 $vf13, 0x0(%1)\n"
        : : "r"(first), "r"(second));
}
