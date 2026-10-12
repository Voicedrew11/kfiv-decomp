#include "kf_collision.h"

void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);

/* Tests a sphere against the two triangles of the owner's four points and pushes
   `centre` out along the normal. */
void func_001898E8(CollisionOwner *owner, f32 *centre, f32 radius, f32 push) {
    int point_a, point_b, point_c, point_d;

    COLLISION_RESULT->hit = 0;
    if (owner->flags & 0x10000) {
        point_a = owner->model->get_point(owner->model, 0);
        point_b = owner->model->get_point(owner->model, 1);
        point_c = owner->model->get_point(owner->model, 2);
        point_d = owner->model->get_point(owner->model, 3);
        func_0020E1F8(point_a, point_b, point_c, (int)centre, push, radius);
        if (COLLISION_RESULT->hit == 0) {
            func_0020E1F8(point_a, point_c, point_d, (int)centre, push, radius);
            if (COLLISION_RESULT->hit == 0) {
                return;
            }
        }
        centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
        centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
    }
}
