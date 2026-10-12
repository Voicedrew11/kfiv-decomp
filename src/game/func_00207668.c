#include "common.h"

/* Loads a matrix into the VU0 registers vf5..vf7. */
void func_00207668(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf5, 0x0(%0)
"
        "lqc2 $vf6, 0x10(%0)
"
        "lqc2 $vf7, 0x20(%0)
"
        : : "r"(matrix));
}
