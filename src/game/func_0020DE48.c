#include "kf_collision.h"

void Vector_Subtract(void *, void *, void *);
void func_00216C48(void *, void *, void *);
void Vector_Scale(void *, void *, float);
void func_00216D20(void *, void *);
float sqrtf(float);

/* sphere against sphere: fills COLLISION_RESULT when the two overlap */
void func_0020DE48(void *center_a, void *center_b, float radius_a, float radius_b) {
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
        func_00216D20(COLLISION_RESULT->normal, offset);
        Vector_Scale(COLLISION_RESULT->point, COLLISION_RESULT->normal, distance);
        func_00216C48(COLLISION_RESULT->point, COLLISION_RESULT->point, center_a);
        COLLISION_RESULT->distance = distance;
        COLLISION_RESULT->hit = 1;
    }
}
