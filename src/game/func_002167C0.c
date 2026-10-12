#include "common.h"

/* Loads a 4x4 matrix into VU0 registers vf8 to vf11. */
void func_002167C0(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf8, 0x0(%0)\n"
        "lqc2 $vf9, 0x10(%0)\n"
        "lqc2 $vf10, 0x20(%0)\n"
        "lqc2 $vf11, 0x30(%0)\n"
        : : "r"(matrix));
}
