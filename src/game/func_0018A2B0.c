#include "kf_collision.h"

void func_001883E0(void *, void *, void *, void *, f32);

/* Tests a sphere against the two triangles of the owner's four points. */
void func_0018A2B0(CollisionOwner *owner, void *centre, f32 radius) {
    void *point_a, *point_b, *point_c, *point_d;

    COLLISION_RESULT->hit = 0;
    if (owner->flags & 0x10000) {
        point_a = (void *)owner->model->get_point(owner->model, 0);
        point_b = (void *)owner->model->get_point(owner->model, 1);
        point_c = (void *)owner->model->get_point(owner->model, 2);
        point_d = (void *)owner->model->get_point(owner->model, 3);
        func_001883E0(point_a, point_b, point_c, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        func_001883E0(point_a, point_c, point_d, centre, radius);
    }
}
