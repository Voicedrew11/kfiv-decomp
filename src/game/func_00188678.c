#include "kf_collision.h"

void Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);

/* Tests a ray (origin, unit direction, length) against a triangle. */
void func_00188678(void *corner_a, void *corner_b, void *corner_c, void *origin, void *direction, f32 length) {
    f32 offset[4];
    f32 unused[12];
    CollisionResult *result = COLLISION_RESULT;
    f32 *normal;
    f32 facing;
    f32 distance;
    f32 side_a, side_b, side_c;

    result->hit = 0;
    normal = COLLISION_RESULT->normal;
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "lqc2 $vf17, 0(%2)\n"
        "lqc2 $vf18, 0(%3)\n"
        "lqc2 $vf19, 0(%4)\n"
        "lqc2 $vf20, 0(%5)\n"
        "vsub.xyz $vf22, $vf16, $vf17\n"
        "vsub.xyz $vf23, $vf18, $vf17\n"
        "vopmula.xyz ACC, $vf23, $vf22\n"
        "vopmsub.xyz $vf21, $vf22, $vf23\n"
        "vmul.xyz $vf22, $vf21, $vf21\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "vsqrt Q, $vf22x\n"
        "vwaitq\n"
        "vaddq.x $vf22, $vf0, Q\n"
        "vdiv Q, $vf0w, $vf22x\n"
        "vwaitq\n"
        "vmulq.xyz $vf21, $vf21, Q\n"
        "sqc2 $vf21, 0(%6)\n"
        "vmul.xyz $vf22, $vf21, $vf20\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "qmfc2.ni $2, $vf22\n"
        "mtc1 $2, %0\n"
        : "=f"(facing)
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(origin), "r"(direction), "r"(normal)
        : "$2");

    if (facing < 0.02f && -0.02f < facing) {
        return;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%2)\n"
        "lqc2 $vf17, 0(%3)\n"
        "lqc2 $vf18, 0(%4)\n"
        "mfc1 $2, %1\n"
        "qmtc2.ni $2, $vf19\n"
        "vsub.xyz $vf21, $vf16, $vf17\n"
        "vmul.xyz $vf22, $vf21, $vf18\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "vdiv Q, $vf22x, $vf19x\n"
        "vwaitq\n"
        "vaddq.x $vf20, $vf0, Q\n"
        "qmfc2.ni $2, $vf20\n"
        "mtc1 $2, %0\n"
        : "=f"(distance)
        : "f"(facing), "r"(corner_b), "r"(origin), "r"(normal)
        : "$2");

    if (length < distance) {
        return;
    }
    if (distance < 0.0f) {
        return;
    }
    COLLISION_RESULT->distance = distance;
    Vector_Scale(offset, direction, distance);
    func_00216C48(COLLISION_RESULT, offset, origin);
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
    }
}
