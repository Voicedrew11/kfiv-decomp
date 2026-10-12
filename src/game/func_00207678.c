#include "common.h"

/* Loads a matrix into the VU0 registers vf1..vf4. */
void func_00207678(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%0)
"
        "lqc2 $vf2, 0x10(%0)
"
        "lqc2 $vf3, 0x20(%0)
"
        "lqc2 $vf4, 0x30(%0)
"
        : : "r"(matrix));
}
