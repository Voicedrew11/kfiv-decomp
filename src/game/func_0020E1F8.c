#include "kf_collision.h"

extern const f32 D_00446FBC[];      /* -0.0f */

void func_0020E918(f32 *, f32 *, f32 *, f32 *, f32 *, f32);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* records a hit at `from`. The result is written through constant addresses (*&), not through
   a shared base register. */
#define KEEP_NEAREST(distance, from)                                                               \
    nearest_squared = (distance);                                                                  \
    *&COLLISION_RESULT->hit = 1;                                                                   \
    *&COLLISION_RESULT->point[0] = (from)[0];                                                      \
    *&COLLISION_RESULT->point[1] = (from)[1];                                                      \
    *&COLLISION_RESULT->point[2] = (from)[2]

/* Tests an upright capsule (a sphere of `radius` swept from `centre` down by `height`) against a
   triangle: its face first, as a ray from the top or the bottom of the capsule along the level
   normal of the face, then the nearest point of each edge seen from above. */
void func_0020E1F8(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *centre, f32 radius, f32 height) {
    f32 point[4];
    f32 edge[4];
    f32 flat[4];
    f32 middle[4];
    f32 normal[4];
    f32 origin[4];
    f32 start[4];
    f32 end[4];
    f32 *result_normal;
    f32 radius_squared;
    f32 nearest_squared;    /* match: first the distance of the middle of the capsule from the plane */
    f32 distance;   /* match: one variable for the distance from the plane and each squared distance */
    f32 along;
    f32 ratio;      /* match: one variable for the largest distance that can hit, each denominator and its quotient */
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
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vopmula.xyz ACC, $vf19, $vf20\n"
        "vopmsub.xyz $vf16, $vf20, $vf19\n"
        "sqc2 $vf19, 0(%3)\n"
        "sqc2 $vf20, 0(%4)\n"
        "vnop\n"
        "vmul.xyz $vf17, $vf16, $vf16\n"
        "vaddx.xyz $vf17, $vf17, $vf17x\n"
        "vaddy.xyz $vf17, $vf17, $vf17y\n"
        "vrsqrt Q, $vf0w, $vf17z\n"
        "vwaitq\n"
        "vmulq.xyz $vf16, $vf16, Q\n"
        "sqc2 $vf16, 0(%5)\n"
        "sqc2 $vf16, 0(%6)\n"
        :
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(edge), "r"(flat), "r"(normal), "r"(result_normal));

    radius_squared = radius * radius;
    middle[0] = centre[0];
    middle[1] = centre[1] - height * 0.5f;
    middle[2] = centre[2];
    distance = normal[0] * corner_b[0] + normal[1] * corner_b[1] + normal[2] * corner_b[2];
    nearest_squared = -normal[0] * middle[0] - normal[1] * middle[1] - normal[2] * middle[2] + distance;
    distance += -normal[0] * centre[0] - normal[1] * centre[1] - normal[2] * centre[2];
    ratio = radius_squared + height * height * 0.25f;
    if (ratio < nearest_squared * nearest_squared) {
        return;
    }
    ratio = -normal[1];
    if (-0.0001f < ratio && ratio < 0.0001f) {
        if (radius < distance) {
            return;
        }
        normal[1] = 0.0f;
        sceVu0Normalize(normal, normal);
        if (distance < D_00446FBC[0]) {
            normal[0] = -normal[0];
            normal[2] = -normal[2];
        }
        origin[0] = centre[0];
        origin[1] = centre[1];
        origin[2] = centre[2];
        func_0020E918(corner_a, corner_b, corner_c, origin, normal, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        origin[1] = centre[1] - height;
        func_0020E918(corner_a, corner_b, corner_c, origin, normal, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
    } else {
        ratio = distance / ratio;
        if (ratio < D_00446FBC[0]) {
            normal[1] = 0.0f;
            sceVu0Normalize(normal, normal);
            if (distance < D_00446FBC[0]) {
                normal[0] = -normal[0];
                normal[2] = -normal[2];
            }
            func_0020E918(corner_a, corner_b, corner_c, centre, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        } else if (height < ratio) {
            origin[0] = centre[0];
            origin[2] = centre[2];
            origin[1] = centre[1] - height;
            normal[1] = 0.0f;
            sceVu0Normalize(normal, normal);
            if (distance < D_00446FBC[0]) {
                normal[0] = -normal[0];
                normal[2] = -normal[2];
            }
            func_0020E918(corner_a, corner_b, corner_c, origin, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        }
    }
    nearest_squared = 16777216.0f;
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            func_00216778(start, corner_a);
            func_00216778(end, corner_b);
        } else if (i == 1) {
            func_00216778(start, corner_b);
            func_00216778(end, corner_c);
        } else {
            func_00216778(start, corner_c);
            func_00216778(end, corner_a);
        }
        if (end[1] > centre[1] && centre[1] < start[1]) {
            continue;
        }
        if (end[1] < centre[1] - height && start[1] < centre[1] - height) {
            continue;
        }
        KF_Vector_Subtract(edge, start, end);
        flat[0] = edge[0];
        flat[1] = 0.0f;
        flat[2] = edge[2];
        sceVu0Normalize(flat, flat);
        along = flat[0] * edge[0] + flat[1] * edge[1] + flat[2] * edge[2];
        if (-0.000001f < along && along < 0.000001f) {
            point[0] = end[0];
            point[1] = centre[1];
            point[2] = end[2];
            KF_Vector_Subtract(edge, centre, point);
            distance = edge[0] * edge[0] + edge[2] * edge[2];
            if (!(distance <= radius_squared)) {
                continue;
            }
            if (distance <= nearest_squared) {
                KEEP_NEAREST(distance, point);
            }
            continue;
        }
        ratio = flat[0] * centre[0] + flat[1] * centre[1] + flat[2] * centre[2];
        ratio += -flat[0] * end[0] - flat[1] * end[1] - flat[2] * end[2];
        along = 1.0f / along;
        ratio *= along;
        if (ratio < D_00446FBC[0]) {
            point[0] = end[0];
            point[1] = centre[1];
            point[2] = end[2];
            KF_Vector_Subtract(edge, centre, point);
            distance = edge[0] * edge[0] + edge[2] * edge[2];
            if (!(distance <= radius_squared)) {
                continue;
            }
            if (distance <= nearest_squared) {
                KEEP_NEAREST(distance, point);
            }
        } else if (1.00001f < ratio) {
            point[0] = start[0];
            point[1] = centre[1];
            point[2] = start[2];
            KF_Vector_Subtract(edge, centre, point);
            distance = edge[0] * edge[0] + edge[2] * edge[2];
            if (!(distance <= radius_squared)) {
                continue;
            }
            if (distance <= nearest_squared) {
                KEEP_NEAREST(distance, point);
            }
        } else {
            point[0] = end[0] + edge[0] * ratio;
            point[1] = end[1] + edge[1] * ratio;
            point[2] = end[2] + edge[2] * ratio;
            KF_Vector_Subtract(edge, centre, point);
            distance = edge[0] * edge[0] + edge[2] * edge[2];
            if (!(distance <= radius_squared)) {
                continue;
            }
            if (distance <= nearest_squared && point[1] <= centre[1] && centre[1] - height <= point[1]) {
                KEEP_NEAREST(distance, point);
            }
        }
    }
}
