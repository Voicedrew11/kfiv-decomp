#include "kf_collision.h"

void func_0019B688(float *, float *, float *, float *);
void func_00216D20(void *, void *);
void func_00216778(void *, void *);
void func_001C0C50(void *, void *, void *, void *, f32);
extern f32 D_004469B0[];    /* the largest float; an array so that it is not read through $gp */

/* Drops a ball of this radius at `position` onto the four-cornered face of the owner's model,
   unless the face stands upright, and moves it to the lowest hit. Returns TRUE if it hit. */
s32 func_001C7C60(CollisionOwner *owner, f32 *position, f32 radius) {
    f32 centre[4];
    f32 normal[4];
    f32 *point0;
    f32 *point1;
    f32 *point2;
    f32 *point3;
    f32 lowest;

    if (owner->flags & 0x10000) {
        point0 = (f32 *)owner->model->get_point(owner->model, 0);
        point1 = (f32 *)owner->model->get_point(owner->model, 1);
        point2 = (f32 *)owner->model->get_point(owner->model, 2);
        point3 = (f32 *)owner->model->get_point(owner->model, 3);
        func_0019B688(normal, point0, point1, point3);
        func_00216D20(normal, normal);
        if (normal[1] >= -0.93969f && normal[1] <= 0.93969f) {
            return FALSE;
        }
        func_00216778(centre, position);
        lowest = D_004469B0[0];
        centre[1] = centre[1] - 1.0f - radius;
        func_001C0C50(point0, point1, point2, centre, radius);
        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
            lowest = COLLISION_RESULT->point[1];
        }
        func_001C0C50(point0, point2, point3, centre, radius);
        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
            lowest = COLLISION_RESULT->point[1];
        }
        if (lowest < D_004469B0[0]) {
            position[1] = lowest - 1.0f;
            return TRUE;
        }
    }
    return FALSE;
}
