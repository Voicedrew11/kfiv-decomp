#include "common.h"

/* Writes the difference of two vec3s to out and returns its length (VU0 macro code). */
float func_0019C640(f32 *out, f32 *point_a, f32 *point_b) {
    f32 length[4];

    __asm__ volatile(
        "vaddw.xyz $vf31, $vf0, $vf0w\n"
        "lqc2 $vf1, 0(%1)\n"
        "lqc2 $vf2, 0(%2)\n"
        "vsub.xyzw $vf3, $vf1, $vf2\n"
        "vmul.xyzw $vf4, $vf3, $vf3\n"
        "vmulax.x ACC, $vf3, $vf3x\n"
        "vmadday.x ACC, $vf31, $vf4y\n"
        "vmaddz.x $vf5, $vf31, $vf4z\n"
        "vsqrt Q, $vf5x\n"
        "vwaitq\n"
        "vmulq.x $vf6, $vf31, Q\n"
        "sqc2 $vf3, 0(%0)\n"
        "sqc2 $vf6, 0(%3)\n"
        : : "r"(out), "r"(point_a), "r"(point_b), "r"(length) : "memory");
    return length[0];
}
