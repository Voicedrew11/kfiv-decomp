#include "kf_collision.h"

void func_001883E0(void *, void *, void *, void *, f32);
void func_0018A628(CollisionOwner *, void *, f32);

/* Tests a sphere against the owner's eight quads (four points each); a quad with a corner
   at the model's own position ends the test. */
void func_0018A3A8(CollisionOwner *owner, void *centre, f32 radius) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    s32 i;

    COLLISION_RESULT->hit = 0;
    if (owner->flags & 0x10000) {
        if (!(owner->flags & 0x20000)) {
            func_0018A628(owner, centre, radius);
            return;
        }
        for (i = 0; i < 0x20; i += 4) {
            corner_a = (f32 *)owner->model->get_point(owner->model, i);
            corner_b = (f32 *)owner->model->get_point(owner->model, i + 1);
            corner_c = (f32 *)owner->model->get_point(owner->model, i + 2);
            corner_d = (f32 *)owner->model->get_point(owner->model, i + 3);
            if (corner_a[0] == owner->model->position.x && corner_a[1] == owner->model->position.y
                && corner_a[2] == owner->model->position.z) {
                return;
            }
            if (corner_b[0] == owner->model->position.x && corner_b[1] == owner->model->position.y
                && corner_b[2] == owner->model->position.z) {
                return;
            }
            if (corner_c[0] == owner->model->position.x && corner_c[1] == owner->model->position.y
                && corner_c[2] == owner->model->position.z) {
                return;
            }
            if (corner_d[0] == owner->model->position.x && corner_d[1] == owner->model->position.y
                && corner_d[2] == owner->model->position.z) {
                return;
            }
            func_001883E0(corner_a, corner_b, corner_c, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            func_001883E0(corner_a, corner_c, corner_d, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        }
    }
}
