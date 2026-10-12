#include "kf_collision.h"

/* Tests a sphere against the segment start..end: a hit when the closest point of the segment is nearer to `centre` than `radius`. */
void func_00188230(void *start, void *end, void *centre, f32 radius) {
    f32 direction[4];
    f32 along;
    f32 distance;
    CollisionResult *result;
    int float_bits;

    __asm__ volatile(
        "lui %2, 0x7000\n"
        "lqc2 $vf16, 0(%4)\n"
        "lqc2 $vf17, 0(%5)\n"
        "lqc2 $vf18, 0(%6)\n"
        "vsub.xyz $vf19, $vf17, $vf16\n"
        "vsub.xyz $vf20, $vf18, $vf16\n"
        "vmul.xyz $vf20, $vf19, $vf20\n"
        "vaddy.x $vf20, $vf20, $vf20y\n"
        "vaddz.x $vf20, $vf20, $vf20z\n"
        "vmul.xyz $vf21, $vf19, $vf19\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "vdiv Q, $vf20x, $vf21x\n"
        "sqc2 $vf19, %1\n"
        "vwaitq\n"
        "vaddq.x $vf20, $vf0, Q\n"
        "qmfc2 %3, $vf20\n"
        "mtc1 %3, %0\n"
        : "=f"(along), "=m"(direction), "=r"(result), "=r"(float_bits)
        : "r"(start), "r"(end), "r"(centre));

    if (0.0f <= along && along <= 1.0f) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%3)\n"
            "lqc2 $vf17, 0(%4)\n"
            "lqc2 $vf18, %6\n"
            "mfc1 %1, %2\n"
            "qmtc2 %1, $vf20\n"
            "vmulx.xyzw $vf19, $vf18, $vf20x\n"
            "vadd.xyz $vf19, $vf19, $vf16\n"
            "vsub.xyz $vf20, $vf17, $vf19\n"
            "vmul.xyz $vf20, $vf20, $vf20\n"
            "vaddy.x $vf20, $vf20, $vf20y\n"
            "vaddz.x $vf20, $vf20, $vf20z\n"
            "vsqrt Q, $vf20x\n"
            "sqc2 $vf19, 0(%5)\n"
            "vwaitq\n"
            "vaddq.x $vf20, $vf0, Q\n"
            "qmfc2 %1, $vf20\n"
            "mtc1 %1, %0\n"
            : "=f"(distance), "=&r"(float_bits)
            : "f"(along), "r"(start), "r"(centre), "r"(result), "m"(direction[0]));

        COLLISION_RESULT->distance = distance;
        if (!(radius <= distance)) {
            __asm__ volatile(
                "lqc2 $vf16, 0(%0)\n"
                "lqc2 $vf17, 0(%2)\n"
                "vsub.xyz $vf18, $vf16, $vf17\n"
                "vmul.xyz $vf19, $vf18, $vf18\n"
                "vaddy.x $vf19, $vf19, $vf19y\n"
                "vaddz.x $vf19, $vf19, $vf19z\n"
                "vsqrt Q, $vf19x\n"
                "vwaitq\n"
                "vaddq.x $vf19, $vf0, Q\n"
                "vdiv Q, $vf0w, $vf19x\n"
                "vwaitq\n"
                "vmulq.xyz $vf18, $vf18, Q\n"
                "sqc2 $vf18, 0(%1)\n"
                : : "r"(centre), "r"(COLLISION_RESULT->normal), "r"(result));

            COLLISION_RESULT->hit = 1;
        }
    }
}
