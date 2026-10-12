#include "kf_collision.h"

/* Tests a sphere against a point: a hit when `point` is closer to `centre` than `radius`. */
void func_00188348(void *point, void *centre, f32 radius) {
    CollisionResult *result;
    f32 distance;
    int distance_bits;

    __asm__ volatile(
        "lui %1, 0x7000\n"
        "lqc2 $vf16, 0(%3)\n"
        "lqc2 $vf17, 0(%4)\n"
        "vsub.xyz $vf18, $vf17, $vf16\n"
        "vmul.xyz $vf18, $vf18, $vf18\n"
        "vaddy.x $vf18, $vf18, $vf18y\n"
        "vaddz.x $vf18, $vf18, $vf18z\n"
        "vsqrt Q, $vf18x\n"
        "vwaitq\n"
        "vaddq.x $vf18, $vf0, Q\n"
        "qmfc2.ni %2, $vf18\n"
        "mtc1 %2, %0\n"
        : "=f"(distance), "=r"(result), "=r"(distance_bits)
        : "r"(point), "r"(centre));

    result->distance = distance;
    if (!(radius <= distance)) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%0)\n"
            "lqc2 $vf17, 0(%1)\n"
            "sqc2 $vf16, 0(%2)\n"
            "vsub.xyz $vf18, $vf17, $vf16\n"
            "vmul.xyz $vf19, $vf18, $vf18\n"
            "vaddy.x $vf19, $vf19, $vf19y\n"
            "vaddz.x $vf19, $vf19, $vf19z\n"
            "vsqrt Q, $vf19x\n"
            "vwaitq\n"
            "vaddq.x $vf19, $vf0, Q\n"
            "vdiv Q, $vf0w, $vf19x\n"
            "vwaitq\n"
            "vmulq.xyz $vf18, $vf18, Q\n"
            "sqc2 $vf18, 0(%3)\n"
            : : "r"(point), "r"(centre), "r"(result), "r"(COLLISION_RESULT->normal));

        COLLISION_RESULT->hit = 1;
    }
}
