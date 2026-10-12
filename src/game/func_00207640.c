#include "common.h"

/* Converts a float vector to integers; the first argument is not used. */
void func_00207640(void *unused, void *source, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "vftoi0.xyzw $vf17, $vf16\n"
        "sqc2 $vf17, 0x0(%1)"
        : : "r"(source), "r"(result));
}
