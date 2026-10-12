#include "common.h"

/* Loads a matrix into the VU0 registers vf8..vf11. */
void func_00207650(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf8, 0x0(%0)
"
        "lqc2 $vf9, 0x10(%0)
"
        "lqc2 $vf10, 0x20(%0)
"
        "lqc2 $vf11, 0x30(%0)
"
        : : "r"(matrix));
}
