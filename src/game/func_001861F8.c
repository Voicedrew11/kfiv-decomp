#include "kf_collision.h"

void Vector_Subtract(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* sphere against sphere: fills COLLISION_RESULT when the two overlap, the point on the second's surface */
void func_001861F8(void *center_a, float *center_b, float radius_a, float radius_b) {
    float offset[4];
    float reach;
    float reach_squared;

    reach_squared = radius_a + radius_b;
    COLLISION_RESULT->hit = 0;
    Vector_Subtract(offset, center_a, center_b);
    reach = reach_squared;
    reach_squared *= reach_squared;
    if (!(reach_squared <= offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2])) {
        COLLISION_RESULT->hit = 1;
        sceVu0Normalize(COLLISION_RESULT->normal, offset);
        COLLISION_RESULT->point[0] = center_b[0] + COLLISION_RESULT->normal[0] * reach;
        COLLISION_RESULT->point[1] = center_b[1] + COLLISION_RESULT->normal[1] * reach;
        COLLISION_RESULT->point[2] = center_b[2] + COLLISION_RESULT->normal[2] * reach;
    }
}
