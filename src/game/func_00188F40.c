#include "kf_collision.h"

void Collision_CylinderEdge(void *, void *, f32, void *, f32, f32);
void Collision_CylinderCylinder(void *, f32, f32, void *, f32, f32);

/* Pushes a sphere out of the four walls standing on the owner's points 0-3 (as high as point
   0x1F is above point 0), then out of the four corner posts. */
void func_00188F40(CollisionOwner *owner, f32 *centre, f32 radius, f32 push) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    f32 *top;
    f32 height;

    COLLISION_RESULT->hit = 0;
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
        Collision_CylinderEdge(corner_a, corner_b, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderEdge(corner_b, corner_c, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderEdge(corner_c, corner_d, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderEdge(corner_d, corner_a, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_a, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_b, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_c, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_d, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
    }
}
