#include "kf_collision.h"
#include "kf_entity.h"

void Collision_BallEdge(void *, void *, f32, void *, f32);

/* Tests a sphere against the walls standing on the owner's points: one ring of four, or two
   rings (points 0-3 and 4-7) while the model is animating. */
void func_00189EC8(CollisionOwner *owner, void *centre, f32 radius) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    f32 *corner_e, *corner_f, *corner_g, *corner_h;
    f32 *top;
    f32 height;

    COLLISION_RESULT->hit = 0;
    if (((EntityModel *)owner->model)->unkFC->unk81 == 0 && ((EntityModel *)owner->model)->unkFC->unk82 < 2) {
        corner_a = (f32 *)owner->model->get_point(owner->model, 0);
        if (corner_a == NULL) {
            return;
        }
        corner_b = (f32 *)owner->model->get_point(owner->model, 0);
        if (corner_b == NULL) {
            return;
        }
        corner_c = (f32 *)owner->model->get_point(owner->model, 6);
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
            Collision_BallEdge(corner_a, corner_b, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_b, corner_c, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_c, corner_d, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_d, corner_a, height, centre, radius);
        }
    } else {
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
        corner_e = (f32 *)owner->model->get_point(owner->model, 4);
        if (corner_e == NULL) {
            return;
        }
        corner_f = (f32 *)owner->model->get_point(owner->model, 5);
        if (corner_f == NULL) {
            return;
        }
        corner_g = (f32 *)owner->model->get_point(owner->model, 6);
        if (corner_g == NULL) {
            return;
        }
        corner_h = (f32 *)owner->model->get_point(owner->model, 7);
        if (corner_h == NULL) {
            return;
        }
        top = (f32 *)owner->model->get_point(owner->model, 0x1F);
        if (top != NULL) {
            height = corner_a[1] - top[1];
            Collision_BallEdge(corner_a, corner_b, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_b, corner_c, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_c, corner_d, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_d, corner_a, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_e, corner_f, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_f, corner_g, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_g, corner_h, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_h, corner_e, height, centre, radius);
        }
    }
}
