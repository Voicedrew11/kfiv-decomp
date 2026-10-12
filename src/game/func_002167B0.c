#include "common.h"

/* Loads three 16-byte rows into VU0 registers vf5 to vf7. */
void func_002167B0(void *rows) {
    __asm__ volatile(
        "lqc2 $vf5, 0x0(%0)\n"
        "lqc2 $vf6, 0x10(%0)\n"
        "lqc2 $vf7, 0x20(%0)\n"
        : : "r"(rows));
}
