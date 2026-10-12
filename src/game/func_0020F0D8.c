#include "kf_collision.h"

void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void sceVu0Normalize(float *, float *);

/* Tests two upright cylinders, each given by a point on its axis, its radius and its height (it runs
   from y - height to y).
   Leaves a point on the rim of the second one towards the first, and the way from the first to the
   second as the normal. Only cylinders that also overlap in height count as a hit. */
void func_0020F0D8(f32 *base, f32 radius, f32 height, f32 *other_base, f32 other_radius, f32 other_height) {
    f32 unused[4];
    f32 delta[4];
    f32 reach;
    f32 level;
    f32 other_level;

    reach = radius + other_radius;
    COLLISION_RESULT->hit = 0;
    KF_Vector_Subtract(delta, other_base, base);
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
    KF_Vector_Scale(COLLISION_RESULT->normal, delta, -1.0f);
}
