#include "kf_entity.h"
#include "kf_collision.h"

double sqrt(double);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* sphere against cylinder: is the sphere at `position` inside the cylinder reaching `other_height` up from `other`,
   and where should it be instead */
void func_00185E28(f32 *position, f32 *other, f32 radius, f32 other_radius, f32 other_height) {
    CollisionResult *result = COLLISION_RESULT;
    f32 delta[4];
    f32 nearest[4];
    f32 reach;
    f32 reach_squared;
    f32 distance;
    f32 below;
    f32 above;
    f32 depth;

    reach_squared = radius + other_radius;
    result->hit = 0;
    Vector_Subtract(delta, position, other);
    reach = reach_squared;
    reach_squared *= reach_squared;
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    if (reach_squared <= distance) {
        return;
    }
    if (position[1] + radius < other[1] - other_height) {
        return;
    }
    if (other[1] < position[1] - radius) {
        return;
    }
    result->hit = 1;
    distance = sqrt(distance);
    if ((other[1] - other_height <= position[1] && position[1] <= other[1]) || distance <= other_radius) {
        below = (position[1] + radius) - (other[1] - other_height);
        below = ABS(below);
        above = position[1] - radius - other[1];
        above = ABS(above);
        depth = reach - distance;
        if (below <= above && below <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->hit = 2;
            result->point[0] = position[0];
            result->point[1] = other[1] - other_height - radius;
            result->point[2] = position[2];
        } else if (above <= below && above <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->point[0] = position[0];
            result->point[1] = other[1] + radius;
            result->point[2] = position[2];
        } else {
            delta[1] = 0.0f;
            sceVu0Normalize(result->normal, delta);
            result->point[0] = other[0] + result->normal[0] * reach;
            result->point[1] = position[1];
            result->point[2] = other[2] + result->normal[2] * reach;
        }
    } else {
        delta[1] = 0.0f;
        sceVu0Normalize(nearest, delta);
        Vector_Scale(nearest, nearest, other_radius);
        Vector_Add(nearest, nearest, other);
        if (position[1] <= other[1] - other_height * 0.5f) {
            nearest[1] -= other_height;
        }
        reach_squared = radius * radius;
        Vector_Subtract(delta, position, nearest);
        distance = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
        if (reach_squared <= distance) {
            result->hit = 0;
            return;
        }
        sceVu0Normalize(result->normal, delta);
        result->point[0] = nearest[0] + result->normal[0] * radius;
        result->point[1] = nearest[1] + result->normal[1] * radius;
        result->point[2] = nearest[2] + result->normal[2] * radius;
    }
}
