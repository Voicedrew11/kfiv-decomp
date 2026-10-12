#include "kf_collision.h"

extern const f32 D_00446FB8[];      /* -0.0f */

void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_00216778(void *, void *);
void sceVu0Normalize(float *, float *);

/* Tests a point against a triangle: within `reach` of its plane, and its foot on the plane inside
   the triangle (barycentric test in the plane the normal leans to least). Leaves the foot as the
   hit point and the distance from the plane. */
void func_0020DF38(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *point, f32 reach) {
    f32 foot[4];
    f32 edge_a[4];
    f32 edge_c[4];
    f32 from_b[4];
    f32 normal[4];
    f32 distance;
    f32 denominator;
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
    sceVu0Normalize(normal, normal);
    distance = normal[0] * corner_a[0] + normal[1] * corner_a[1] + normal[2] * corner_a[2];
    distance += -normal[0] * point[0] - normal[1] * point[1] - normal[2] * point[2];
    if (distance < D_00446FB8[0]) {
        normal[0] = -normal[0];
        normal[1] = -normal[1];
        normal[2] = -normal[2];
        distance = -distance;
    }
    if (reach <= distance) {
        return;
    }
    KF_Vector_Scale(foot, normal, distance);
    KF_Vector_Subtract(foot, point, foot);
    KF_Vector_Subtract(from_b, foot, corner_b);
    COLLISION_RESULT->distance = distance;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf16, 0(%0)\n"
        :
        : "r"(normal));
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
    if (weight_c < D_00446FB8[0]) {
        return;
    }
    if (weight_a < D_00446FB8[0]) {
        return;
    }
    if (weight_c + weight_a <= 1.0f) {
        COLLISION_RESULT->hit = 1;
        func_00216778(COLLISION_RESULT->point, foot);
    }
}
