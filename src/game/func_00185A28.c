#include "kf_entity.h"
#include "kf_collision.h"

double sqrt(double);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* cylinder against sphere: is `position` (a cylinder reaching `height` up from it) inside the sphere at `other`,
   and where should it stand instead */
void func_00185A28(f32 *position, f32 *other, f32 radius, f32 height, f32 other_radius) {
    CollisionResult *result = COLLISION_RESULT;
    f32 delta[4];
    f32 nearest[4];
    f32 reach;
    f32 distance;
    f32 below;
    f32 above;
    f32 depth;
    f32 overlap;

    reach = radius + other_radius;
    result->hit = 0;
    Vector_Subtract(delta, position, other);
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    if (reach * reach <= distance) {
        return;
    }
    if (position[1] < other[1] - other_radius) {
        return;
    }
    if (position[1] - height > other[1] + other_radius) {
        return;
    }
    result->hit = 1;
    distance = sqrt(distance);
    if ((position[1] - height <= other[1] && other[1] <= position[1]) || distance <= radius) {
        below = position[1] - (other[1] - other_radius);
        below = ABS(below);
        above = (position[1] - height) - (other[1] + other_radius);
        above = ABS(above);
        depth = reach - distance;
        if (below <= above && below <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->hit = 2;
            result->point[0] = position[0];
            result->point[1] = other[1] - other_radius;
            result->point[2] = position[2];
        } else if (above <= below && above <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->point[0] = position[0];
            result->point[1] = other[1] + other_radius + height;
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
        Vector_Scale(nearest, nearest, -radius);
        Vector_Add(nearest, nearest, position);
        if (other[1] <= position[1] - height * 0.5f) {
            nearest[1] -= height;
        }
        Vector_Subtract(delta, nearest, other);
        distance = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
        if (other_radius <= distance) {
            result->hit = 0;
            return;
        }
        sceVu0Normalize(result->normal, delta);
        overlap = other_radius - distance;
        result->point[0] = position[0] + result->normal[0] * overlap;
        result->point[1] = position[1] + result->normal[1] * overlap;
        result->point[2] = position[2] + result->normal[2] * overlap;
    }
}
