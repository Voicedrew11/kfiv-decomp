#include "kf_collision.h"

void KF_Vector_Subtract(void *, void *, void *);
extern const f32 D_00446984[];

/* func_001C0A88 with a limit: the triangle must be above the point, by no more than `reach`. */
void func_001C0C50(void *corner_a, void *corner_b, void *corner_c, void *point, f32 reach) {
    f32 hit_point[4];
    f32 edge_a[4];
    f32 edge_c[4];
    f32 offset[4];
    f32 normal[4];
    f32 scalar; // match: one variable for the normal's y, then the rise, then the area, as retail keeps one register
    f32 along_c;
    f32 along_a;

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
        : : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(edge_a), "r"(edge_c), "r"(normal));

    scalar = normal[1];
    if (-0.00001f < scalar && scalar < 0.00001f) {
        return;
    }
    scalar = (normal[0] * ((f32 *)corner_a)[0] + scalar * ((f32 *)corner_a)[1] + normal[2] * ((f32 *)corner_a)[2] +
              (-normal[0] * ((f32 *)point)[0] - scalar * ((f32 *)point)[1] - normal[2] * ((f32 *)point)[2])) /
             scalar;
    if (scalar < D_00446984[0]) {
        return;
    }
    if (reach < scalar) {
        return;
    }
    hit_point[0] = ((f32 *)point)[0];
    hit_point[1] = ((f32 *)point)[1] + scalar;
    hit_point[2] = ((f32 *)point)[2];
    COLLISION_RESULT->distance = scalar;
    KF_Vector_Subtract(offset, hit_point, corner_b);
    scalar = edge_c[0] * edge_a[2] - edge_c[2] * edge_a[0];
    along_c = offset[0] * edge_a[2] - offset[2] * edge_a[0];
    along_a = offset[2] * edge_c[0] - offset[0] * edge_c[2];
    along_c /= scalar;
    along_a /= scalar;
    if (along_c < D_00446984[0]) {
        return;
    }
    if (along_a < D_00446984[0]) {
        return;
    }
    if (along_c + along_a <= 1.0f) {
        COLLISION_RESULT->hit = 1;
        COLLISION_RESULT->point[1] = hit_point[1];
    }
}
