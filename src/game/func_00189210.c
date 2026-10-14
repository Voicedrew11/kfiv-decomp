#include "kf_collision.h"
#include "kf_entity.h"

float func_00194F80(float *, float *);
void Collision_CylinderEdge(void *, void *, f32, void *, f32, f32);
void Collision_CylinderCylinder(void *, f32, f32, void *, f32, f32);

#define PUSH_CENTRE(amount)                                                                     centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (amount);            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (amount)

/* Pushes a sphere out of the walls standing on the owner's points, then out of the corner posts:
   one ring of four points, or two rings (0-3 and 4-7) while the model is animating. */
void func_00189210(CollisionOwner *owner, f32 *centre, f32 radius, f32 push) {
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
        corner_b = (f32 *)owner->model->get_point(owner->model, 5);
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
            Collision_CylinderEdge(corner_a, corner_b, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_b, corner_c, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_c, corner_d, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_d, corner_a, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_a, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_b, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_c, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_d, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
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
            if (func_00194F80(corner_b, corner_e) <= push * 2.5f) {
                Collision_CylinderEdge(corner_g, corner_d, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_CENTRE(push);
                    return;
                }
                Collision_CylinderEdge(corner_b, corner_e, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_CENTRE(push);
                    return;
                }
            }
            Collision_CylinderEdge(corner_a, corner_b, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_b, corner_c, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_c, corner_d, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_d, corner_a, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_e, corner_f, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_f, corner_g, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_g, corner_h, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_h, corner_e, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_a, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_b, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_c, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_d, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_e, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_f, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_g, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_h, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
        }
    }
}
