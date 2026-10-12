#include "kf_collision.h"

void func_00210018(void *, void *, f32, void *, f32);

/* Tests a sphere against the four walls standing on the owner's points 0-3, as high as
   point 0x1F is above point 0. */
void func_00189D28(CollisionOwner *owner, void *centre, f32 radius) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    f32 *top;
    f32 height;

    COLLISION_RESULT->hit = 0;
    if (owner->model->get_point(owner->model, 0x1F) == 0) {
        return;
    }
    corner_a = (f32 *)owner->model->get_point(owner->model, 0);
    if (corner_a == NULL) {
        return;
    }
    corner_b = (f32 *)owner->model->get_point(owner->model, 1);
    if (corner_b == NULL) {
        return;
    }
    corner_c = (f32 *)owner->model->get_point(owner->model, 2);
    if (corner_c == NULL) {
        return;
    }
    corner_d = (f32 *)owner->model->get_point(owner->model, 3);
    if (corner_d == NULL) {
        return;
    }
    top = (f32 *)owner->model->get_point(owner->model, 0x1F);
    if (top != NULL) {
        height = corner_a[1] - top[1];
        func_00210018(corner_a, corner_b, height, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        func_00210018(corner_b, corner_c, height, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        func_00210018(corner_c, corner_d, height, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        func_00210018(corner_d, corner_a, height, centre, radius);
    }
}
