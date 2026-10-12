#include "kf_collision.h"

extern const f32 D_00446FC0[];      /* -0.0f */

void func_00216778(void *, void *);

/* Tests a ray against a triangle: from `origin` along `direction`, no further than `reach` times the
   direction. Leaves the point where it meets the triangle and how far along the ray that is. */
void func_0020E918(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *origin, f32 *direction, f32 reach) {
    f32 foot[4];
    f32 edge_a[4];
    f32 edge_c[4];
    f32 from_b[4];
    f32 normal[4];
    f32 distance;
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
    denominator = normal[0] * direction[0] + normal[1] * direction[1] + normal[2] * direction[2];
    if (-0.00001f < denominator && denominator < 0.00001f) {
        return;
    }
    distance = normal[0] * corner_a[0] + normal[1] * corner_a[1] + normal[2] * corner_a[2];
    distance += -normal[0] * origin[0] - normal[1] * origin[1] - normal[2] * origin[2];
    denominator = distance / denominator;
    if (denominator < D_00446FC0[0]) {
        return;
    }
    if (reach < denominator) {
        return;
    }
    foot[0] = origin[0] + direction[0] * denominator;
    foot[1] = origin[1] + direction[1] * denominator;
    foot[2] = origin[2] + direction[2] * denominator;
    COLLISION_RESULT->distance = denominator;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "lqc2 $vf18, 0(%2)\n"
        "vnop\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vnop\n"
        "vsub.xyz $vf19, $vf17, $vf18\n"
        "vnop\n"
        "sqc2 $vf16, 0(%0)\n"
        "vnop\n"
        "sqc2 $vf19, 0(%3)\n"
        :
        : "r"(normal), "r"(foot), "r"(corner_b), "r"(from_b));
    if (normal[0] > normal[1]) {
        if (normal[2] < normal[0]) {
            denominator = edge_c[2] * edge_a[1] - edge_c[1] * edge_a[2];
            weight_c = from_b[2] * edge_a[1] - from_b[1] * edge_a[2];
            weight_a = from_b[1] * edge_c[2] - from_b[2] * edge_c[1];
        } else {
            denominator = edge_c[0] * edge_a[1] - edge_c[1] * edge_a[0];
            weight_c = from_b[0] * edge_a[1] - from_b[1] * edge_a[0];
            weight_a = from_b[1] * edge_c[0] - from_b[0] * edge_c[1];
        }
    } else if (normal[2] < normal[1]) {
        denominator = edge_c[0] * edge_a[2] - edge_c[2] * edge_a[0];
        weight_c = from_b[0] * edge_a[2] - from_b[2] * edge_a[0];
        weight_a = from_b[2] * edge_c[0] - from_b[0] * edge_c[2];
    } else {
        denominator = edge_c[0] * edge_a[1] - edge_c[1] * edge_a[0];
        weight_c = from_b[0] * edge_a[1] - from_b[1] * edge_a[0];
        weight_a = from_b[1] * edge_c[0] - from_b[0] * edge_c[1];
    }
    weight_c /= denominator;
    weight_a /= denominator;
    if (weight_c < D_00446FC0[0]) {
        return;
    }
    if (weight_a < D_00446FC0[0]) {
        return;
    }
    if (weight_c + weight_a <= 1.0f) {
        COLLISION_RESULT->hit = 1;
        func_00216778(COLLISION_RESULT->point, foot);
    }
}
