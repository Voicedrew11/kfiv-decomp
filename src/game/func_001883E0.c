#include "kf_collision.h"

void func_00188230(void *, void *, void *, float);
void func_00188348(void *, void *, f32);

/* Tests a sphere against a triangle: its face first, then its three edges and three corners. */
void func_001883E0(void *corner_a, void *corner_b, void *corner_c, void *centre, f32 radius) {
    CollisionResult *result = COLLISION_RESULT;
    f32 *normal;
    f32 unused[8];
    f32 distance;
    f32 side_a, side_b, side_c;

    result->hit = 0;
    normal = COLLISION_RESULT->normal;
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "lqc2 $vf17, 0(%2)\n"
        "lqc2 $vf18, 0(%3)\n"
        "lqc2 $vf19, 0(%4)\n"
        "vsub.xyz $vf21, $vf16, $vf17\n"
        "vsub.xyz $vf22, $vf18, $vf17\n"
        "vopmula.xyz ACC, $vf22, $vf21\n"
        "vopmsub.xyz $vf20, $vf21, $vf22\n"
        "vmul.xyz $vf21, $vf20, $vf20\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "vsqrt Q, $vf21x\n"
        "vwaitq\n"
        "vaddq.x $vf21, $vf0, Q\n"
        "vdiv Q, $vf0w, $vf21x\n"
        "vwaitq\n"
        "vmulq.xyz $vf20, $vf20, Q\n"
        "sqc2 $vf20, 0(%5)\n"
        "vmul.xyz $vf21, $vf20, $vf17\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "vmul.xyz $vf22, $vf20, $vf19\n"
        "vsubx.x $vf21, $vf21, $vf22x\n"
        "vsuby.x $vf21, $vf21, $vf22y\n"
        "vsubz.x $vf21, $vf21, $vf22z\n"
        "qmfc2.ni $2, $vf21\n"
        "mtc1 $2, %0\n"
        "vmulx.xyz $vf22, $vf20, $vf21x\n"
        "vadd.xyz $vf22, $vf22, $vf19\n"
        "sqc2 $vf22, 0(%6)\n"
        : "=f"(distance)
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(centre), "r"(normal), "r"(result)
        : "$2");

    result->distance = distance;
    if (radius <= distance) {
        return;
    }
    if (distance <= -radius) {
        return;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%3)\n"
        "lqc2 $vf17, 0(%4)\n"
        "lqc2 $vf18, 0(%5)\n"
        "lqc2 $vf19, 0(%6)\n"
        "vsub.xyz $vf20, $vf16, $vf19\n"
        "vsub.xyz $vf21, $vf17, $vf19\n"
        "vsub.xyz $vf22, $vf18, $vf19\n"
        "vopmula.xyz ACC, $vf20, $vf21\n"
        "vopmsub.xyz $vf16, $vf21, $vf20\n"
        "vopmula.xyz ACC, $vf21, $vf22\n"
        "vopmsub.xyz $vf17, $vf22, $vf21\n"
        "vopmula.xyz ACC, $vf22, $vf20\n"
        "vopmsub.xyz $vf18, $vf20, $vf22\n"
        "vmul.xyz $vf20, $vf16, $vf17\n"
        "vaddy.x $vf20, $vf20, $vf20y\n"
        "vaddz.x $vf20, $vf20, $vf20z\n"
        "qmfc2.ni $2, $vf20\n"
        "mtc1 $2, %0\n"
        "vmul.xyz $vf21, $vf17, $vf18\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "qmfc2.ni $2, $vf21\n"
        "mtc1 $2, %1\n"
        "vmul.xyz $vf22, $vf18, $vf16\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "qmfc2.ni $2, $vf22\n"
        "mtc1 $2, %2\n"
        : "=f"(side_a), "=f"(side_b), "=f"(side_c)
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(result)
        : "$2");

    if ((0.0f <= side_a && 0.0f <= side_b && 0.0f <= side_c) || (side_a < 0.0f && side_b < 0.0f && side_c < 0.0f)) {
        COLLISION_RESULT->hit = 1;
        return;
    }
    func_00188230(corner_a, corner_b, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00188230(corner_b, corner_c, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00188230(corner_c, corner_a, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00188348(corner_a, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00188348(corner_b, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00188348(corner_c, centre, radius);
}
