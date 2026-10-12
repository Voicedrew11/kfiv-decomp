#include "kf_collision.h"

void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);

/* Tests a point against a capsule: the line from `base` along the unit vector `axis` for `length`,
   within a distance whose square is `reach_squared`. Past either end the hit point is that end. */
void func_0020DC68(f32 *point, f32 *base, f32 *axis, f32 reach_squared, f32 length) {
    f32 offset[4];
    f32 from_base[4];
    f32 unused[4];
    f32 along;
    f32 distance_squared;

    COLLISION_RESULT->hit = 0;
    along = axis[0] * point[0] + axis[1] * point[1] + axis[2] * point[2];
    along += -axis[0] * base[0] - axis[1] * base[1] - axis[2] * base[2];
    if (along < 0.0f) {
        KF_Vector_Subtract(from_base, point, base);
        distance_squared = from_base[0] * from_base[0] + from_base[1] * from_base[1] + from_base[2] * from_base[2];
        if (distance_squared < reach_squared) {
            func_00216778(COLLISION_RESULT->point, base);
            COLLISION_RESULT->hit = 1;
            return;
        }
    }
    if (length < along) {
        KF_Vector_Scale(offset, axis, length);
        func_00216C48(offset, axis, offset);    // match: the far end is taken as axis + axis * length, not base + axis * length
        KF_Vector_Subtract(offset, point, offset);
        distance_squared = offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2];
        if (distance_squared < reach_squared) {
            func_00216778(COLLISION_RESULT->point, offset);
            COLLISION_RESULT->hit = 1;
            return;
        }
    }
    KF_Vector_Subtract(offset, point, base);
    distance_squared = offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2];
    distance_squared -= along * along;
    if (distance_squared < reach_squared) {
        COLLISION_RESULT->hit = 1;
    }
}
