#include "kf_entity.h"
#include "kf_collision.h"

double sqrt(double);
void Vector_Subtract(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* cylinder against cylinder: is `position` (a cylinder reaching `height` up from it) inside the other one,
   and where should it stand instead */
void func_001857E8(f32 *position, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_height) {
    CollisionResult *result = COLLISION_RESULT;
    f32 delta[4];
    f32 reach;
    f32 reach_squared;
    f32 distance_squared;
    f32 other_top;
    f32 top;
    f32 below;
    f32 above;
    f32 depth;

    reach = radius + other_radius;
    result->hit = 0;
    reach_squared = reach * reach;
    Vector_Subtract(delta, position, other);
    distance_squared = delta[0] * delta[0] + delta[2] * delta[2];
    if (reach_squared < distance_squared) {
        return;
    }
    other_top = other[1] - other_height;
    if (position[1] < other_top) {
        return;
    }
    top = position[1] - height;
    if (other[1] < top) {
        return;
    }
    below = position[1] - other_top;
    result->hit = 1;
    below = ABS(below);
    above = top - other[1];
    above = ABS(above);
    depth = reach - sqrt(distance_squared);
    if (below <= above && below <= depth) {
        result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
        result->hit = 2;
        result->point[0] = position[0];
        result->point[1] = other[1] - other_height;
        result->point[2] = position[2];
    } else if (above <= below && above <= depth) {
        result->hit = 0;
    } else {
        delta[1] = 0.0f;
        sceVu0Normalize(result->normal, delta);
        result->point[0] = other[0] + result->normal[0] * reach;
        result->point[1] = position[1];
        result->point[2] = other[2] + result->normal[2] * reach;
    }
}
