#include "kf_collision.h"

extern const f32 D_00446FC4[];      /* -0.0f */

void KF_Vector_Subtract(void *, void *, void *);

/* Drops a point straight up onto a triangle: finds where the upright line through `point` meets the
   triangle's plane, no more than 8192 above the point, and whether that is inside the triangle seen
   from above. Leaves the height of that place and how far above the point it is. */
void func_0020EE58(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *point) {
    f32 foot[4];
    f32 edge_a[4];
    f32 edge_c[4];
    f32 from_b[4];
    f32 normal[4];
    f32 denominator;    /* match: one variable for each denominator and the quotient it becomes */
    f32 weight_a;
    f32 weight_c;

    COLLISION_RESULT->hit = 0;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "lqc2 $vf18, 0(%2)\n"
        "vnop\n"
        "vnop\n"
        "vsub.xyz $vf19, $vf16, $vf17\n"
        "vsub.xyz $vf20, $vf18, $vf17\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vopmula.xyz ACC, $vf19, $vf20\n"
        "vopmsub.xyz $vf16, $vf20, $vf19\n"
        "sqc2 $vf19, 0(%3)\n"
        "sqc2 $vf20, 0(%4)\n"
        "vnop\n"
        "sqc2 $vf16, 0(%5)\n"
        :
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(edge_a), "r"(edge_c), "r"(normal));
    denominator = normal[1];
    if (-0.00001f < denominator && denominator < 0.00001f) {
        return;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "lqc2 $vf18, 0(%2)\n"
        "vnop\n"
        "vnop\n"
        "vsub.xyz $vf19, $vf16, $vf17\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmul.xyz $vf20, $vf19, $vf18\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf20, 0(%3)\n"
        :
        : "r"(corner_a), "r"(point), "r"(normal), "r"(from_b));
    denominator = (from_b[0] + from_b[1] + from_b[2]) / denominator;
    if (denominator < D_00446FC4[0]) {
        return;
    }
    if (8192.0f < denominator) {
        return;
    }
    foot[0] = point[0];
    foot[1] = point[1] + denominator;
    foot[2] = point[2];
    COLLISION_RESULT->distance = denominator;
    KF_Vector_Subtract(from_b, foot, corner_b);
    denominator = edge_c[0] * edge_a[2] - edge_c[2] * edge_a[0];
    weight_c = from_b[0] * edge_a[2] - from_b[2] * edge_a[0];
    weight_a = from_b[2] * edge_c[0] - from_b[0] * edge_c[2];
    weight_c /= denominator;
    weight_a /= denominator;
    if (weight_c < D_00446FC4[0]) {
        return;
    }
    if (weight_a < D_00446FC4[0]) {
        return;
    }
    if (weight_c + weight_a <= 1.0f) {
        COLLISION_RESULT->hit = 1;
        COLLISION_RESULT->point[1] = foot[1];
    }
}
