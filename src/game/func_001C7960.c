#include "kf_collision.h"

void Vector_TriangleNormal(float *, float *, float *, float *);
void Vector_Normalize(void *, void *);
void Collision_CylinderEdge(void *, void *, f32, void *, f32, f32);

/* Pushes `position` out of the upright side of the owner's model that runs from its point 0
   to its point 3; does nothing when that side lies nearly flat. */
void func_001C7960(CollisionOwner *owner, f32 *position, f32 radius, f32 push) {
    f32 normal[4];
    f32 *point0;
    f32 *point1;
    f32 *point3;
    f32 height;

    if (owner->flags & 0x10000) {
        point0 = (f32 *)owner->model->get_point(owner->model, 0);
        point1 = (f32 *)owner->model->get_point(owner->model, 1);
        point3 = (f32 *)owner->model->get_point(owner->model, 3);
        Vector_TriangleNormal(normal, point0, point1, point3);
        Vector_Normalize(normal, normal);
        if (normal[1] < -0.93969f) {
            return;
        }
        if (normal[1] > 0.93969f) {
            return;
        }
        height = point0[1] - point1[1];
        Collision_CylinderEdge(point0, point3, height, position, radius, push);
        if (COLLISION_RESULT->hit == 0) {
            Collision_CylinderEdge(point3, point0, height, position, radius, push);
            if (COLLISION_RESULT->hit == 0) {
                return;
            }
        }
        position[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
        position[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
    }
}
