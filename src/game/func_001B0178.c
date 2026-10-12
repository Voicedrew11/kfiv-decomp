#include "common.h"

/* Distance between two points. */
float func_001B0178(void *from, void *target) {
    f32 distance[4];

    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf17, $vf16\n"
        "vmul.xyz $vf17, $vf16, $vf16\n"
        "vaddx.z $vf17, $vf17, $vf17x\n"
        "vaddy.z $vf17, $vf17, $vf17y\n"
        "vsqrt Q, $vf17z\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(from), "r"(target), "r"(distance) : "memory");
    return distance[0];
}
