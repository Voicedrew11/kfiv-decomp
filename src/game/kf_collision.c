/* The collision tests: points, rays, spheres, capsules and upright cylinders against triangles, cylinders,
   wall edges and the arc of a swing. Each leaves its result in COLLISION_RESULT at the start of the scratchpad. */
#include "kf_collision.h"
#include "kf_entity.h"

extern const f32 D_00446FB8[];      /* -0.0f */
extern const f32 D_00446FBC[];      /* -0.0f */
extern const f32 D_00446FC0[];      /* -0.0f */
extern const f32 D_00446FC4[];      /* -0.0f */
extern const f32 D_00446FC8[];      /* -0.0f */
extern const f32 D_00446FCC[];      /* -0.0f */

void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);
void Vector_Normalize(void *, void *);
void sceVu0Normalize(float *, float *);
float sqrtf(float);
float atan2f(float, float);
float asinf(float);
float cosf(float);
float Angle_Wrap(float);

/* records a hit at `from`. The result is written through constant addresses (*&), not through
   a shared base register. */
#define KEEP_NEAREST(distance, from)                                                               \
    nearest_squared = (distance);                                                                  \
    *&COLLISION_RESULT->hit = 1;                                                                   \
    *&COLLISION_RESULT->point[0] = (from)[0];                                                      \
    *&COLLISION_RESULT->point[1] = (from)[1];                                                      \
    *&COLLISION_RESULT->point[2] = (from)[2]
/* Tests a point against a capsule: the line from `base` along the unit vector `axis` for `length`,
   within a distance whose square is `reach_squared`. Past either end the hit point is that end. Nothing calls it. */
static void Collision_PointCapsule(f32 *point, f32 *base, f32 *axis, f32 reach_squared, f32 length) {
    f32 offset[4];
    f32 from_base[4];
    f32 unused[4];
    f32 along;
    f32 distance_squared;

    COLLISION_RESULT->hit = 0;
    along = axis[0] * point[0] + axis[1] * point[1] + axis[2] * point[2];
    along += -axis[0] * base[0] - axis[1] * base[1] - axis[2] * base[2];
    if (along < 0.0f) {
        Vector_Subtract(from_base, point, base);
        distance_squared = from_base[0] * from_base[0] + from_base[1] * from_base[1] + from_base[2] * from_base[2];
        if (distance_squared < reach_squared) {
            Vector_Copy(COLLISION_RESULT->point, base);
            COLLISION_RESULT->hit = 1;
            return;
        }
    }
    if (length < along) {
        Vector_Scale(offset, axis, length);
        Vector_Add(offset, axis, offset);    // match: the far end is taken as axis + axis * length, not base + axis * length
        Vector_Subtract(offset, point, offset);
        distance_squared = offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2];
        if (distance_squared < reach_squared) {
            Vector_Copy(COLLISION_RESULT->point, offset);
            COLLISION_RESULT->hit = 1;
            return;
        }
    }
    Vector_Subtract(offset, point, base);
    distance_squared = offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2];
    distance_squared -= along * along;
    if (distance_squared < reach_squared) {
        COLLISION_RESULT->hit = 1;
    }
}

/* Tests a sphere against a sphere: a hit when the two overlap. */
void Collision_SphereSphere(void *center_a, void *center_b, float radius_a, float radius_b) {
    float offset[3];
    float distance;
    float reach;

    reach = radius_a + radius_b;
    COLLISION_RESULT->hit = 0;
    Vector_Subtract(offset, center_b, center_a);
    reach *= reach;
    distance = offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2];
    if (!(reach < distance)) {
        distance = sqrtf(distance);
        Vector_Normalize(COLLISION_RESULT->normal, offset);
        Vector_Scale(COLLISION_RESULT->point, COLLISION_RESULT->normal, distance);
        Vector_Add(COLLISION_RESULT->point, COLLISION_RESULT->point, center_a);
        COLLISION_RESULT->distance = distance;
        COLLISION_RESULT->hit = 1;
    }
}

/* Tests a point against a triangle: within `reach` of its plane, and its foot on the plane inside
   the triangle (barycentric test in the plane the normal leans to least). Leaves the foot as the
   hit point and the distance from the plane. */
void Collision_PointTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *point, f32 reach) {
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
    Vector_Scale(foot, normal, distance);
    Vector_Subtract(foot, point, foot);
    Vector_Subtract(from_b, foot, corner_b);
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
        Vector_Copy(COLLISION_RESULT->point, foot);
    }
}

/* Tests an upright capsule (a sphere of `radius` swept from `centre` down by `height`) against a
   triangle: its face first, as a ray from the top or the bottom of the capsule along the level
   normal of the face, then the nearest point of each edge seen from above. */
void Collision_CapsuleTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *centre, f32 radius, f32 height) {
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
        Collision_RayTriangle(corner_a, corner_b, corner_c, origin, normal, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        origin[1] = centre[1] - height;
        Collision_RayTriangle(corner_a, corner_b, corner_c, origin, normal, radius);
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
            Collision_RayTriangle(corner_a, corner_b, corner_c, centre, normal, radius);
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
            Collision_RayTriangle(corner_a, corner_b, corner_c, origin, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        }
    }
    nearest_squared = 16777216.0f;
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            Vector_Copy(start, corner_a);
            Vector_Copy(end, corner_b);
        } else if (i == 1) {
            Vector_Copy(start, corner_b);
            Vector_Copy(end, corner_c);
        } else {
            Vector_Copy(start, corner_c);
            Vector_Copy(end, corner_a);
        }
        if (end[1] > centre[1] && centre[1] < start[1]) {
            continue;
        }
        if (end[1] < centre[1] - height && start[1] < centre[1] - height) {
            continue;
        }
        Vector_Subtract(edge, start, end);
        flat[0] = edge[0];
        flat[1] = 0.0f;
        flat[2] = edge[2];
        sceVu0Normalize(flat, flat);
        along = flat[0] * edge[0] + flat[1] * edge[1] + flat[2] * edge[2];
        if (-0.000001f < along && along < 0.000001f) {
            point[0] = end[0];
            point[1] = centre[1];
            point[2] = end[2];
            Vector_Subtract(edge, centre, point);
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
            Vector_Subtract(edge, centre, point);
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
            Vector_Subtract(edge, centre, point);
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
            Vector_Subtract(edge, centre, point);
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

/* Tests a ray against a triangle: from `origin` along `direction`, no further than `reach` times the
   direction. Leaves the point where it meets the triangle and how far along the ray that is. */
void Collision_RayTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *origin, f32 *direction, f32 reach) {
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
        Vector_Copy(COLLISION_RESULT->point, foot);
    }
}

/* Tests a move against the plane of a triangle: from `origin` by `direction` times `length`. Leaves
   the point where the move meets the plane, the plane's normal, and how far along that is, squared.
   Nothing calls it. */
static void Collision_MoveTrianglePlane(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *origin, f32 *direction,
                                        f32 length) {
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
    Vector_Subtract(edge_a, corner_b, corner_a);
    Vector_Subtract(edge_c, corner_c, corner_b);
    plane[0] = edge_a[1] * edge_c[2] - edge_a[2] * edge_c[1];
    plane[1] = edge_a[2] * edge_c[0] - edge_a[0] * edge_c[2];
    plane[2] = edge_a[0] * edge_c[1] - edge_a[1] * edge_c[0];
    plane[3] = 1.0f;
    distance = -(plane[0] * corner_a[0] + plane[1] * corner_a[1] + plane[2] * corner_a[2]);
    Vector_Scale(move, direction, length);
    // match: the parallel test adds the plane normal and the direction and sums the components; a dot product was meant
    Vector_Add(edge_a, plane, direction);
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
    Vector_Scale(COLLISION_RESULT->point, move, distance);
    Vector_Add(COLLISION_RESULT->point, COLLISION_RESULT->point, origin);
    Vector_Copy(COLLISION_RESULT->normal, normal);
    COLLISION_RESULT->distance = (length * distance) * (length * distance);
}

/* Drops a point straight up onto a triangle: finds where the upright line through `point` meets the
   triangle's plane, no more than 8192 above the point, and whether that is inside the triangle seen
   from above. Leaves the height of that place and how far above the point it is. Nothing calls it. */
static void Collision_DropOntoTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *point) {
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
    Vector_Subtract(from_b, foot, corner_b);
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

/* Returns 1 if the point lies outside the circle of `radius` around the model seen from above, else 0.
   Nothing calls it. */
static int Collision_IsOutsideCircle(EntityModel *model, f32 *point, f32 radius) {
    f32 delta_x;
    f32 delta_z;

    delta_x = point[0] - model->position[0];
    delta_z = point[2] - model->position[2];
    if (radius < delta_x) {
        return 1;
    }
    if (delta_x < -radius) {
        return 1;
    }
    if (radius < delta_z) {
        return 1;
    }
    if (delta_z < -radius) {
        return 1;
    }
    return radius * radius < delta_x * delta_x + delta_z * delta_z;
}

/* Tests two upright cylinders, each given by a point on its axis, its radius and its height (it runs
   from y - height to y).
   Leaves a point on the rim of the second one towards the first, and the way from the first to the
   second as the normal. Only cylinders that also overlap in height count as a hit. */
void Collision_CylinderCylinder(f32 *base, f32 radius, f32 height, f32 *other_base, f32 other_radius,
                                f32 other_height) {
    f32 unused[4];
    f32 delta[4];
    f32 reach;
    f32 level;
    f32 other_level;

    reach = radius + other_radius;
    COLLISION_RESULT->hit = 0;
    Vector_Subtract(delta, other_base, base);
    reach *= reach;
    if (reach <= delta[0] * delta[0] + delta[2] * delta[2]) {
        return;
    }
    delta[1] = 0.0f;
    sceVu0Normalize(delta, delta);
    COLLISION_RESULT->point[0] = other_base[0] - delta[0] * other_radius;
    COLLISION_RESULT->point[2] = other_base[2] - delta[2] * other_radius;
    level = base[1];
    other_level = other_base[1];
    if (other_level < level - height) {
        COLLISION_RESULT->point[1] = level - height;
    } else if (level < other_level - other_height) {
        COLLISION_RESULT->point[1] = other_level - other_height;
    } else {
        COLLISION_RESULT->point[1] = level;
        COLLISION_RESULT->hit = 1;
    }
    Vector_Scale(COLLISION_RESULT->normal, delta, -1.0f);
}

/* Tests a point against an upright cylinder: its base at `origin`, its radius squared and its height.
   Nothing calls it. */
static void Collision_PointCylinder(f32 *point, f32 *origin, f32 radius_squared, f32 height) {
    f32 delta[3];

    if (point[1] > origin[1]) {
        return;
    }
    if (point[1] < origin[1] - height) {
        return;
    }
    Vector_Subtract(delta, point, origin);
    if (delta[0] * delta[0] + delta[2] * delta[2] < radius_squared) {
        COLLISION_RESULT->hit = 1;
    }
}

/* Tests a ray (from `origin` along `direction` for `length`) against an upright cylinder: `base` on
   its axis, its radius and its height. Seen from above, finds the point of the ray nearest the axis;
   leaves that point, or where the ray enters the cylinder, and how far along the ray that is. */
void Collision_RayCylinder(f32 radius, f32 height, f32 length, f32 *base, f32 *origin, f32 *direction) {
    f32 start[4];
    f32 end[4];
    f32 along[4];
    f32 offset[4];
    f32 heading[4];
    f32 nearest[4];
    f32 radius_squared;
    f32 distance_squared;
    f32 facing;
    f32 part;           /* how far along the ray the nearest point is, 0 to 1 */
    f32 inside;
    f32 across;
    f32 ahead;
    f32 delta_x;
    f32 delta_z;

    radius_squared = radius * radius;
    COLLISION_RESULT->hit = 0;
    Vector_Copy(start, origin);
    end[0] = origin[0] + direction[0] * length;
    end[1] = origin[1] + direction[1] * length;
    end[2] = origin[2] + direction[2] * length;
    if (base[1] < end[1] && base[1] < start[1]) {
        return;
    }
    if (end[1] < base[1] - height && start[1] < base[1] - height) {
        return;
    }
    Vector_Subtract(along, end, start);
    heading[0] = along[0];
    heading[1] = 0.0f;
    heading[2] = along[2];
    if (radius_squared + length * length < along[0] * along[0] + along[2] * along[2]) {
        return;
    }
    sceVu0Normalize(heading, heading);
    facing = heading[0] * along[0] + heading[1] * along[1] + heading[2] * along[2];
    if (-0.000001f < facing && facing < 0.000001f) {
        nearest[0] = start[0];
        nearest[1] = base[1];
        nearest[2] = start[2];
        Vector_Subtract(along, base, nearest);
        distance_squared = along[0] * along[0] + along[2] * along[2];
        if (!(distance_squared <= radius_squared)) {
            return;
        }
        *&COLLISION_RESULT->hit = 1;
        *&COLLISION_RESULT->point[0] = nearest[0];
        *&COLLISION_RESULT->point[1] = nearest[1];
        *&COLLISION_RESULT->point[2] = nearest[2];
    } else {
        part = heading[0] * base[0] + heading[1] * base[1] + heading[2] * base[2];
        part += -heading[0] * start[0] - heading[1] * start[1] - heading[2] * start[2];
        facing = 1.0f / facing;
        part *= facing;
        nearest[0] = start[0] + along[0] * part;
        nearest[1] = start[1] + along[1] * part;
        nearest[2] = start[2] + along[2] * part;
        if (part < D_00446FC8[0]) {
            Vector_Subtract(offset, base, start);
            distance_squared = offset[0] * offset[0] + offset[2] * offset[2];
            if (!(distance_squared <= radius_squared)) {
                return;
            }
            delta_x = nearest[0] - base[0];
            delta_z = nearest[2] - base[2];
            delta_z = delta_x * delta_x + delta_z * delta_z;
            delta_x = sqrtf(radius_squared - delta_z);
            delta_x -= sqrtf(distance_squared - delta_z);
            delta_z = sqrtf(along[0] * along[0] + along[2] * along[2]);
            *&COLLISION_RESULT->hit = 1;
            *&COLLISION_RESULT->distance = length * (delta_z - delta_x) / delta_z;
            *&COLLISION_RESULT->point[0] = nearest[0];
            *&COLLISION_RESULT->point[1] = nearest[1];
            *&COLLISION_RESULT->point[2] = nearest[2];
        } else if (1.0f < part) {
            Vector_Subtract(offset, base, end);
            distance_squared = offset[0] * offset[0] + offset[2] * offset[2];
            if (!(distance_squared <= radius_squared)) {
                return;
            }
            delta_x = nearest[0] - base[0];
            delta_z = nearest[2] - base[2];
            delta_z = delta_x * delta_x + delta_z * delta_z;
            delta_x = sqrtf(radius_squared - delta_z);
            delta_x -= sqrtf(distance_squared - delta_z);
            delta_z = sqrtf(along[0] * along[0] + along[2] * along[2]);
            *&COLLISION_RESULT->hit = 1;
            *&COLLISION_RESULT->distance = length * (delta_z - delta_x) / delta_z;
            *&COLLISION_RESULT->point[0] = nearest[0];
            *&COLLISION_RESULT->point[1] = nearest[1];
            *&COLLISION_RESULT->point[2] = nearest[2];
        } else {
            Vector_Subtract(offset, base, nearest);
            distance_squared = offset[0] * offset[0] + offset[2] * offset[2];
            if (!(distance_squared <= radius_squared)) {
                return;
            }
            if (!(nearest[1] <= base[1])) {
                return;
            }
            if (!(base[1] - height <= nearest[1])) {
                return;
            }
            across = base[0] - start[0];
            ahead = base[2] - start[2];
            across = across * across + ahead * ahead;
            if (across < radius_squared) {
                *&COLLISION_RESULT->distance = 0.0f;
                *&COLLISION_RESULT->point[0] = start[0];
                *&COLLISION_RESULT->point[1] = start[1];
                *&COLLISION_RESULT->point[2] = start[2];
            } else {
                inside = sqrtf(radius_squared - distance_squared);
                Vector_Subtract(offset, nearest, start);
                *&COLLISION_RESULT->distance = sqrtf(offset[0] * offset[0] + offset[2] * offset[2]) - inside;
                *&COLLISION_RESULT->point[0] = nearest[0];
                *&COLLISION_RESULT->point[1] = nearest[1];
                *&COLLISION_RESULT->point[2] = nearest[2];
            }
            *&COLLISION_RESULT->hit = 1;
        }
    }
}

/* Tests a sphere of radius `reach` at `centre` against an upright cylinder: `base` on its axis, its
   radius and its height. Level with the cylinder the two are compared seen from above; past either
   end the sphere is tested against the nearest point of that end's rim first. Leaves the squared
   distance between the two points. */
void Collision_SphereCylinder(f32 *base, f32 *centre, f32 radius, f32 height, f32 reach) {
    f32 delta[4];
    f32 rim[4];
    CollisionResult *result = COLLISION_RESULT;
    f32 distance;   /* match: one variable for the height difference and each squared distance */

    result->hit = 0;
    distance = base[1] - centre[1];
    Vector_Subtract(delta, base, centre);
    result->distance = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
    if (distance < -reach) {
        return;
    }
    if (height + reach < distance) {
        return;
    }
    // match: the code for the two ends comes after the level case and shares its last lines; only gotos give that order
    if (height < distance) {
        goto past_far_end;
    }
    if (distance < D_00446FCC[0]) {
        goto past_near_end;
    }
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    reach = (radius + reach) * (radius + reach);
    goto compare;

past_far_end:
    delta[1] = 0.0f;
    sceVu0Normalize(rim, delta);
    reach *= reach;
    Vector_Scale(rim, rim, radius);
    Vector_Add(rim, rim, base);
    rim[1] -= height;
    Vector_Subtract(rim, centre, rim);
    distance = rim[0] * rim[0] + rim[1] * rim[1] + rim[2] * rim[2];
    if (distance < reach) {
        COLLISION_RESULT->hit = 1;
        return;
    }
    goto level;

past_near_end:
    delta[1] = 0.0f;
    sceVu0Normalize(rim, delta);
    reach *= reach;
    Vector_Scale(rim, rim, radius);
    Vector_Add(rim, rim, base);
    Vector_Subtract(rim, centre, rim);
    distance = rim[0] * rim[0] + rim[1] * rim[1] + rim[2] * rim[2];
    if (distance < reach) {
        COLLISION_RESULT->hit = 1;
        return;
    }

level:
    reach = radius * radius;
    distance = delta[0] * delta[0] + delta[2] * delta[2];

compare:
    if (distance < reach) {
        COLLISION_RESULT->hit = 1;
    }
}

/* Tests an upright cylinder (`other`) against the part of another one (`base`) that lies within
   `half_angle` either side of the direction `facing`: a swing's arc. Each cylinder is a point on
   its axis, a radius and a height. Leaves the squared distance seen from above, the way from the
   first to the second as the normal, and a point on the first one's rim. */
void Collision_CylinderArc(f32 *base, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_height,
                   f32 half_angle, f32 facing) {
    f32 delta[4];
    f32 unused[4];
    f32 other_top;
    f32 top;
    f32 other_bottom;
    f32 bottom;
    f32 distance;
    f32 angle;

    other_top = other[1];
    top = base[1];
    other_bottom = other_top - other_height;
    bottom = top - height;
    COLLISION_RESULT->hit = 0;
    if (top < other_bottom) {
        return;
    }
    if (other_top < bottom) {
        return;
    }
    Vector_Subtract(delta, base, other);
    other_radius = (radius + other_radius) * (radius + other_radius);    // match: the sum of the radii, squared, kept in the parameter
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    COLLISION_RESULT->distance = distance;
    if (other_radius < distance) {
        return;
    }
    half_angle += atan2f(radius + radius, sqrtf(distance));
    angle = -(atan2f(delta[2], delta[0]) - 1.5707964f);
    angle = Angle_Wrap(angle - facing);
    if (angle < -half_angle) {
        return;
    }
    if (half_angle < angle) {
        return;
    }
    Vector_Subtract(delta, other, base);
    delta[1] = 0.0f;
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    Vector_Scale(delta, COLLISION_RESULT->normal, radius);
    Vector_Add(COLLISION_RESULT->point, base, delta);
    // match: the height of the hit is one level divided by another, halved; the middle of the two was meant
    if (bottom <= other_bottom && other_top <= top) {
        COLLISION_RESULT->point[1] = other_top / other_bottom * 0.5f;
    }
    if (other_bottom <= bottom && top <= other_top) {
        COLLISION_RESULT->point[1] = top / bottom * 0.5f;
    }
    if (bottom <= other_top && other_top <= top) {
        COLLISION_RESULT->point[1] = bottom / other_top * 0.5f;
    }
    if (bottom <= other_bottom && other_bottom <= top) {
        COLLISION_RESULT->point[1] = top / other_bottom * 0.5f;
    }
    COLLISION_RESULT->hit = 1;
}

/* Tests an upright cylinder (`other`, between the levels `other_low` and `other_high`) against the
   part of another one (`base`, its radius and height) that a swing covers: within `half_angle` either
   side of the direction `facing`, and within `pitch_range` either side of the slope `pitch`. Leaves
   the squared distance seen from above, the way from the first to the second as the normal, and a
   point on the rim. */
void Collision_CylinderArcPitch(f32 *base, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_low,
                   f32 other_high, f32 half_angle, f32 facing, f32 pitch_range, f32 pitch) {
    f32 delta[4];
    f32 unused[4];
    f32 top;
    f32 bottom;
    f32 distance;
    f32 flat;
    f32 angle;      /* match: one variable for the angle to the other cylinder and each height difference */
    f32 near;
    f32 upper;
    f32 lower;
    f32 first;
    f32 second;
    f32 lowest;
    f32 level;

    top = base[1];
    COLLISION_RESULT->hit = 0;
    bottom = top - height;
    if (other_high < bottom) {
        return;
    }
    if (top < other_low) {
        return;
    }
    Vector_Subtract(delta, base, other);
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    COLLISION_RESULT->distance = distance;
    if ((radius + other_radius) * (radius + other_radius) < distance) {
        return;
    }
    flat = sqrtf(distance);
    half_angle += atan2f(radius + radius, flat);
    angle = -(atan2f(delta[2], delta[0]) - 1.5707964f);
    angle = Angle_Wrap(angle - facing);
    if (angle < -half_angle) {
        return;
    }
    if (half_angle < angle) {
        return;
    }
    near = flat - radius;
    lower = -pitch - pitch_range;
    upper = -pitch + pitch_range;
    angle = base[1] - height - other[1];
    first = atan2f(angle, near);
    if (other[1] < base[1] - height) {
        second = atan2f(angle, flat);
    } else {
        second = atan2f(angle, flat + radius);
    }
    if (first < second) {
        lowest = first;
    } else {
        lowest = second;
    }
    angle = base[1] - other[1];
    first = atan2f(angle, near);
    if (other[1] > base[1]) {
        second = atan2f(angle, flat);
    } else {
        second = atan2f(angle, flat + radius);
    }
    if (second < first) {
        second = first;
    }
    if (upper < lowest) {
        return;
    }
    if (second < lower) {
        return;
    }
    Vector_Subtract(delta, other, base);
    delta[1] = 0.0f;
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    Vector_Scale(delta, COLLISION_RESULT->normal, other_radius);
    Vector_Add(COLLISION_RESULT->point, base, delta);
    // match: `level` is never given a value; the height of the other cylinder was meant
    if (level < bottom) {
        COLLISION_RESULT->point[1] = bottom;
    } else if (top < level) {
        COLLISION_RESULT->point[1] = top;
    } else {
        COLLISION_RESULT->point[1] = level;
    }
    COLLISION_RESULT->hit = 1;
}

/* Collision_CylinderEdge for a ball of radius `reach` at `centre`: tested as a cylinder twice the radius high
   from the ball's top; a hit is reported at the ball's own height. */
void Collision_BallEdge(void *edge_start, void *edge_end, f32 height, void *centre, f32 reach) {
    f32 *point = centre;
    f32 top[4];

    top[0] = point[0];
    top[1] = point[1] + reach;
    top[2] = point[2];
    COLLISION_RESULT->hit = 0;
    Collision_CylinderEdge(edge_start, edge_end, height, top, reach + reach, reach);
    if (COLLISION_RESULT->hit != 0) {
        COLLISION_RESULT->point[1] = point[1];
    }
}

/* Tests an upright cylinder (axis point `centre`, `radius` high below it) against the wall edge from
   `edge_start` to `edge_end`, which is `height` high below its points: a hit when the axis is on the
   front side of the edge, beside it, and no further than `reach` from it. */
void Collision_CylinderEdge(void *edge_start, void *edge_end, f32 height, void *centre, f32 radius, f32 reach) {
    f32 *start = edge_start;
    f32 *end = edge_end;
    f32 *point = centre;
    f32 edge_x;
    f32 edge_z;
    f32 to_x;
    f32 to_z;
    f32 side;
    f32 along;
    f32 near_x;
    f32 near_z;
    f32 away_x;
    f32 away_z;
    f32 distance;

    COLLISION_RESULT->hit = 0;
    if (point[1] < start[1] - height) {
        return;
    }
    if (start[1] < point[1] - radius) {
        return;
    }
    edge_x = end[0] - start[0];
    edge_z = end[2] - start[2];
    to_z = point[2] - start[2];
    to_x = point[0] - start[0];
    side = edge_x * to_z - edge_z * to_x;
    if (side < 0.0f) {
        return;
    }
    // match: retail tests the same thing a second time
    if (side < 0.0f) {
        return;
    }
    along = to_x * edge_x + to_z * edge_z;
    along /= edge_x * edge_x + edge_z * edge_z;
    if (along < 0.0f) {
        return;
    }
    if (along > 1.0f) {
        return;
    }
    near_x = start[0] + edge_x * along;
    near_z = start[2] + edge_z * along;
    away_x = point[0] - near_x;
    away_z = point[2] - near_z;
    distance = sqrtf(away_x * away_x + away_z * away_z);
    if (distance > reach) {
        return;
    }
    COLLISION_RESULT->point[0] = near_x;
    COLLISION_RESULT->point[1] = start[1];
    COLLISION_RESULT->point[2] = near_z;
    COLLISION_RESULT->normal[0] = away_x / distance;
    COLLISION_RESULT->normal[1] = 0.0f;
    COLLISION_RESULT->normal[2] = away_z / distance;
    COLLISION_RESULT->hit = 1;
}

/* Collision_CylinderEdge with the edge made longer by `reach` at both ends: tests an upright cylinder (axis point
   `centre`, `radius` high below it) against the wall edge from `start` to `end`, `height` high below it. */
void Collision_CylinderEdgeExtended(Vector4 *start, Vector4 *end, f32 height, Vector4 *centre, f32 radius, f32 reach) {
    f32 edge_x;
    f32 edge_z;
    f32 to_x;
    f32 to_z;
    f32 side;
    f32 along;
    f32 margin;
    f32 upper;
    f32 near_x;
    f32 near_z;
    f32 away_x;
    f32 away_z;
    f32 distance;

    COLLISION_RESULT->hit = 0;
    if (centre->y < start->y - height) {
        return;
    }
    if (start->y < centre->y - radius) {
        return;
    }
    edge_x = end->x - start->x;
    edge_z = end->z - start->z;
    to_z = centre->z - start->z;
    to_x = centre->x - start->x;
    side = edge_x * to_z - edge_z * to_x;
    if (side < 0.0f) {
        return;
    }
    along = to_x * edge_x + to_z * edge_z;
    along /= edge_x * edge_x + edge_z * edge_z;
    margin = -reach / sqrtf(edge_x * edge_x + edge_z * edge_z);
    upper = 1.0 - margin;
    if (along < margin) {
        return;
    }
    if (along > upper) {
        return;
    }
    near_x = start->x + edge_x * along;
    near_z = start->z + edge_z * along;
    away_x = centre->x - near_x;
    away_z = centre->z - near_z;
    distance = sqrtf(away_x * away_x + away_z * away_z);
    if (distance > reach) {
        return;
    }
    COLLISION_RESULT->point[0] = near_x;
    COLLISION_RESULT->point[1] = start->y;
    COLLISION_RESULT->point[2] = near_z;
    COLLISION_RESULT->normal[0] = away_x / distance;
    COLLISION_RESULT->normal[1] = 0.0f;
    COLLISION_RESULT->normal[2] = away_z / distance;
    COLLISION_RESULT->hit = 1;
}

/* Tests a ball (`centre`, `radius`) against a swing from `origin`: a slab `depth` deep below the origin's
   height, reaching `range` out, `half_angle` to either side of `facing`. The ball counts with the width it
   has where the slab cuts it. Leaves the squared distance, the way from the ball to the origin as the
   normal and the point of the ball nearest the origin. */
void Collision_BallSwing(void *ball_centre, void *swing_origin, f32 radius, f32 range, f32 depth, f32 half_angle,
                         f32 facing) {
    f32 *centre = ball_centre;
    f32 *origin = swing_origin;
    f32 delta[4];
    f32 unused[4];
    f32 bottom; // match: one variable for the ball's bottom and, later, the squared distance (both in $f12)
    f32 top; // match: one variable for the ball's top and, later, the turn to the ball (both in $f0)
    f32 floor;
    f32 width;

    COLLISION_RESULT->hit = 0;
    top = centre[1];
    bottom = top - radius;
    floor = origin[1] - depth;
    top += radius;
    if (top < floor) {
        return;
    }
    if (origin[1] < bottom) {
        return;
    }
    if (bottom < floor) {
        width = radius * cosf(asinf((floor - bottom) / radius));
    } else if (origin[1] < top) {
        width = radius * cosf(asinf((top - origin[1]) / radius));
    } else {
        width = radius;
    }
    Vector_Subtract(delta, centre, origin);
    width = (width + range) * (width + range);
    bottom = delta[0] * delta[0] + delta[2] * delta[2];
    COLLISION_RESULT->distance = bottom;
    if (width < bottom) {
        return;
    }
    half_angle += atan2f(radius + radius, sqrtf(bottom));
    top = Angle_Wrap(-(atan2f(delta[2], delta[0]) - 1.5707964f) - facing);
    if (top < -half_angle) {
        return;
    }
    if (half_angle < top) {
        return;
    }
    Vector_Subtract(delta, origin, centre);
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    Vector_Scale(delta, COLLISION_RESULT->normal, radius);
    Vector_Add(COLLISION_RESULT->point, centre, delta);
    COLLISION_RESULT->hit = 1;
}

/* Collision_BallSwing for the player's swing: the slab is given by its two heights (`lowest`, `highest`), and the
   ball must also lie within `half_pitch` of the swing's `pitch` as seen from the origin. */
void Collision_BallSwingPitch(void *ball_centre, void *swing_origin, f32 radius, f32 range, f32 lowest, f32 highest,
                              f32 half_angle, f32 facing, f32 half_pitch, f32 pitch) {
    f32 *centre = ball_centre;
    f32 *origin = swing_origin;
    f32 delta[4];
    f32 unused[4];
    f32 bottom; // match: one variable for the ball's bottom and, later, the squared distance (both in $f12)
    f32 top; // match: one variable for the ball's top and, later, the turn to the ball (both in $f0)
    f32 flat_distance;
    f32 twice_radius;
    f32 pitch_low;
    f32 pitch_high;
    f32 pitch_to_ball;
    f32 width;

    COLLISION_RESULT->hit = 0;
    top = centre[1];
    bottom = top - radius;
    top += radius;
    if (top < lowest) {
        return;
    }
    if (highest < bottom) {
        return;
    }
    if (bottom < lowest) {
        width = radius * cosf(asinf((lowest - bottom) / radius));
    } else if (highest < top) {
        width = radius * cosf(asinf((top - highest) / radius));
    } else {
        width = radius;
    }
    Vector_Subtract(delta, centre, origin);
    width = (width + range) * (width + range);
    bottom = delta[0] * delta[0] + delta[2] * delta[2];
    COLLISION_RESULT->distance = bottom;
    if (width < bottom) {
        return;
    }
    flat_distance = sqrtf(bottom);
    twice_radius = radius + radius;
    half_angle += atan2f(twice_radius, flat_distance);
    top = Angle_Wrap(-(atan2f(delta[2], delta[0]) - 1.5707964f) - facing);
    if (top < -half_angle) {
        return;
    }
    if (half_angle < top) {
        return;
    }
    half_pitch += atan2f(twice_radius, sqrtf(COLLISION_RESULT->distance));
    pitch_high = -pitch + half_pitch;
    pitch_low = -pitch - half_pitch;
    pitch_to_ball = atan2f(centre[1] - origin[1], flat_distance);
    if (pitch_to_ball < pitch_low) {
        return;
    }
    if (pitch_high < pitch_to_ball) {
        return;
    }
    Vector_Subtract(delta, origin, centre);
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    Vector_Scale(delta, COLLISION_RESULT->normal, radius);
    Vector_Add(COLLISION_RESULT->point, centre, delta);
    COLLISION_RESULT->hit = 1;
}
