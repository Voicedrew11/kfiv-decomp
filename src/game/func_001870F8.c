#include "kf_collision.h"

extern const f32 D_0044683C[];     /* -0.0f; const: only then may a store pass the read of it */

void func_00188678(void *, void *, void *, void *, void *, f32);
void Vector_Subtract(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* loads one edge of the triangle: its two ends, and start minus end into both `edge` and `flat` */
#define LOAD_EDGE(from, to)                                                                        \
    __asm__ volatile(                                                                              \
        "lqc2 $vf16, 0(%0)\n"                                                                      \
        "lqc2 $vf17, 0(%1)\n"                                                                      \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vsub.xyzw $vf18, $vf16, $vf17\n"                                                          \
        "sqc2 $vf16, 0(%2)\n"                                                                      \
        "sqc2 $vf17, 0(%3)\n"                                                                      \
        "vnop\n"                                                                                   \
        "sqc2 $vf18, 0(%4)\n"                                                                      \
        "sqc2 $vf18, 0(%5)\n"                                                                      \
        :                                                                                          \
        : "r"(from), "r"(to), "r"(start), "r"(end), "r"(edge), "r"(flat))

/* records a hit at `from`. The result is written through constant addresses (*&), not through
   a shared base register. */
#define KEEP_NEAREST(distance, from)                                                               \
    *&COLLISION_RESULT->hit = 1;                                                                   \
    nearest_sq = (distance);                                                                       \
    *&COLLISION_RESULT->point[0] = (from)[0];                                                      \
    *&COLLISION_RESULT->point[1] = (from)[1];                                                      \
    *&COLLISION_RESULT->point[2] = (from)[2]

/* Tests an upright capsule (a sphere of `radius` swept from `centre` down by `height`) against
   a triangle: its face at the top and bottom of the capsule first, then the nearest point of
   each edge. */
void func_001870F8(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *centre, f32 radius, f32 height) {
    f32 point[4];
    f32 edge[4];
    f32 flat[4];
    f32 offset[4];
    f32 unused_a[4];
    f32 normal[4];
    f32 start[4];
    f32 end[4];
    f32 origin[4];
    f32 unused_b[4];
    f32 *result_normal;
    f32 nearest_sq;
    f32 distance;   /* match: one variable for each distance, numerator and squared distance */
    f32 bottom;
    f32 along;
    f32 ratio;      /* match: one variable for each denominator and the quotient it becomes */
    s32 i;

    COLLISION_RESULT->hit = 0;
    result_normal = COLLISION_RESULT->normal;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "lqc2 $vf18, 0(%2)\n"
        "vnop\n"
        "vnop\n"
        "vsub.xyz $vf19, $vf16, $vf17\n"
        "vsub.xyz $vf20, $vf18, $vf17\n"
        "vsub.xyz $vf21, $vf18, $vf16\n"
        "vnop\n"
        "vnop\n"
        "vopmula.xyz ACC, $vf19, $vf20\n"
        "vopmsub.xyz $vf16, $vf20, $vf19\n"
        "sqc2 $vf19, 0(%3)\n"
        "sqc2 $vf20, 0(%4)\n"
        "sqc2 $vf21, 0(%5)\n"
        "vmul.xyz $vf17, $vf16, $vf16\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vaddx.xyz $vf17, $vf17, $vf17x\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vaddy.xyz $vf17, $vf17, $vf17y\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vrsqrt Q, $vf0w, $vf17z\n"
        "vwaitq\n"
        "vmulq.xyz $vf16, $vf16, Q\n"
        "sqc2 $vf16, 0(%6)\n"
        "sqc2 $vf16, 0(%7)\n"
        :
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(edge), "r"(flat), "r"(offset), "r"(normal),
          "r"(result_normal));

    nearest_sq = radius * radius;
    ratio = -normal[1];
    distance = normal[0] * corner_b[0] + normal[1] * corner_b[1] + normal[2] * corner_b[2];
    distance += -normal[0] * centre[0] - normal[1] * centre[1] - normal[2] * centre[2];
    bottom = centre[1] - height;
    if (-0.00001f < ratio && ratio < 0.00001f) {
        if (radius < distance) {
            return;
        }
        if (distance < -radius) {
            return;
        }
        normal[1] = 0.0f;
        if (distance < D_0044683C[0]) {
            normal[0] = -normal[0];
            normal[2] = -normal[2];
        }
        sceVu0Normalize(normal, normal);
        origin[0] = centre[0];
        origin[1] = centre[1];
        origin[2] = centre[2];
        func_00188678(corner_a, corner_b, corner_c, origin, normal, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        origin[1] = bottom;
        func_00188678(corner_a, corner_b, corner_c, origin, normal, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
    } else {
        ratio = distance / ratio;
        if (ratio < D_0044683C[0]) {
            normal[1] = 0.0f;
            if (distance < D_0044683C[0]) {
                normal[0] = -normal[0];
                normal[2] = -normal[2];
            }
            sceVu0Normalize(normal, normal);
            func_00188678(corner_a, corner_b, corner_c, centre, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            origin[0] = centre[0];
            origin[2] = centre[2];
            origin[1] = bottom;
            func_00188678(corner_a, corner_b, corner_c, origin, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        } else if (height < ratio) {
            normal[1] = 0.0f;
            if (distance < D_0044683C[0]) {
                normal[0] = -normal[0];
                normal[2] = -normal[2];
            }
            sceVu0Normalize(normal, normal);
            origin[0] = centre[0];
            origin[2] = centre[2];
            origin[1] = bottom;
            func_00188678(corner_a, corner_b, corner_c, origin, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            func_00188678(corner_a, corner_b, corner_c, centre, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        }
    }
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 0:
            LOAD_EDGE(corner_a, corner_b);
            break;
        case 1:
            LOAD_EDGE(corner_b, corner_c);
            break;
        case 2:
            LOAD_EDGE(corner_c, corner_a);
            break;
        }
        if (centre[1] < end[1] && centre[1] < start[1]) {
            continue;
        }
        if (end[1] < bottom && start[1] < bottom) {
            continue;
        }
        origin[0] = flat[0];
        origin[1] = 0.0f;
        origin[2] = flat[2];
        origin[3] = 1.0f;
        sceVu0Normalize(origin, origin);
        along = origin[0] * edge[0] + origin[2] * edge[2];
        if (-0.000001f < along && along < 0.000001f) {
            point[0] = end[0];
            point[1] = centre[1];
            point[2] = end[2];
            Vector_Subtract(edge, centre, point);
            distance = edge[0] * edge[0] + edge[2] * edge[2];
            if (distance <= nearest_sq) {
                KEEP_NEAREST(distance, point);
            }
            continue;
        }
        if (start[1] > centre[1] || end[1] > centre[1]) {
            distance = -centre[1];
            distance += end[1];
            ratio = -edge[1];
            ratio = distance / ratio;
            point[0] = end[0] + edge[0] * ratio;
            point[1] = end[1] + edge[1] * ratio;
            point[2] = end[2] + edge[2] * ratio;
            Vector_Subtract(offset, centre, point);
            distance = offset[0] * offset[0] + offset[2] * offset[2];
            if (distance < nearest_sq) {
                KEEP_NEAREST(distance, point);
            }
        }
        if (start[1] < bottom || end[1] < bottom) {
            distance = -bottom;
            distance += end[1];
            ratio = -edge[1];
            ratio = distance / ratio;
            point[0] = end[0] + edge[0] * ratio;
            point[1] = end[1] + edge[1] * ratio;
            point[2] = end[2] + edge[2] * ratio;
            Vector_Subtract(offset, centre, point);
            distance = offset[0] * offset[0] + offset[2] * offset[2];
            if (distance < nearest_sq) {
                KEEP_NEAREST(distance, point);
            }
        }
        if (start[1] < centre[1] && bottom < start[1]) {
            Vector_Subtract(offset, centre, start);
            along = offset[0] * offset[0] + offset[2] * offset[2];
            if (along < nearest_sq) {
                KEEP_NEAREST(along, start);
            }
        }
        ratio = origin[0] * centre[0] + origin[2] * centre[2];
        ratio += -origin[0] * end[0] - origin[2] * end[2];
        along = origin[0] * edge[0] + origin[2] * edge[2];
        ratio /= along;
        if (ratio < D_0044683C[0]) {
            continue;
        }
        if (1.0f < ratio) {
            continue;
        }
        point[0] = end[0] + edge[0] * ratio;
        point[1] = end[1] + edge[1] * ratio;
        point[2] = end[2] + edge[2] * ratio;
        Vector_Subtract(offset, centre, point);
        distance = offset[0] * offset[0] + offset[2] * offset[2];
        if (distance <= nearest_sq && point[1] <= centre[1] && bottom <= point[1]) {
            KEEP_NEAREST(distance, point);
        }
    }
}
