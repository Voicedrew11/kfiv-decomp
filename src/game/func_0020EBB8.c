#include "kf_collision.h"

void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);

/* Tests a move against the plane of a triangle: from `origin` by `direction` times `length`. Leaves
   the point where the move meets the plane, the plane's normal, and how far along that is, squared. */
void func_0020EBB8(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *origin, f32 *direction, f32 length) {
    f32 unused[4];
    f32 edge_a[4];
    f32 edge_c[4];
    f32 plane[4];
    f32 move[4];
    f32 normal[4];
    f32 distance;   /* match: one variable for the plane's distance and the part of the move */
    f32 facing;

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
    // match: the two edges are worked out again (the first one reversed), over what the VU0 code stored
    KF_Vector_Subtract(edge_a, corner_b, corner_a);
    KF_Vector_Subtract(edge_c, corner_c, corner_b);
    plane[0] = edge_a[1] * edge_c[2] - edge_a[2] * edge_c[1];
    plane[1] = edge_a[2] * edge_c[0] - edge_a[0] * edge_c[2];
    plane[2] = edge_a[0] * edge_c[1] - edge_a[1] * edge_c[0];
    plane[3] = 1.0f;
    distance = -(plane[0] * corner_a[0] + plane[1] * corner_a[1] + plane[2] * corner_a[2]);
    KF_Vector_Scale(move, direction, length);
    // match: the parallel test adds the plane normal and the direction and sums the components; a dot product was meant
    func_00216C48(edge_a, plane, direction);
    facing = edge_a[0] + edge_a[1] + edge_a[2];
    if (-0.00001f < facing && facing < 0.00001f) {
        return;
    }
    distance = -(plane[0] * origin[0] + plane[1] * origin[1] + plane[2] * origin[2] + distance) /
               (plane[0] * move[0] + plane[1] * move[1] + plane[2] * move[2]);
    if (distance < 0.0f) {
        return;
    }
    if (1.0f < distance) {
        return;
    }
    COLLISION_RESULT->hit = 1;
    KF_Vector_Scale(COLLISION_RESULT->point, move, distance);
    func_00216C48(COLLISION_RESULT->point, COLLISION_RESULT->point, origin);
    func_00216778(COLLISION_RESULT->normal, normal);
    COLLISION_RESULT->distance = (length * distance) * (length * distance);
}
