#include "kf_collision.h"

float asinf(float);
float atan2f(float, float);
float cosf(float);
float sqrtf(float);
float func_0019B838(float);
void func_00216C48(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* Tests a ball (`centre`, `radius`) against a swing from `origin`: a slab `depth` deep below the origin's
   height, reaching `range` out, `half_angle` to either side of `facing`. The ball counts with the width it
   has where the slab cuts it. Leaves the squared distance, the way from the ball to the origin as the
   normal and the point of the ball nearest the origin. */
void func_00210488(void *ball_centre, void *swing_origin, f32 radius, f32 range, f32 depth, f32 half_angle, f32 facing) {
    f32 *centre = ball_centre;
    f32 *origin = swing_origin;
    f32 delta[4];
    f32 unused[4];
    f32 bottom; // match: one variable for the ball's bottom and, later, the squared distance (both in $f12)
    f32 top; // match: one variable for the ball's top and, later, the turn to the ball (both in $f0)
    f32 floor;
    f32 width;

    COLLISION_RESULT->hit = 0;
    top = centre[1];
    bottom = top - radius;
    floor = origin[1] - depth;
    top += radius;
    if (top < floor) {
        return;
    }
    if (origin[1] < bottom) {
        return;
    }
    if (bottom < floor) {
        width = radius * cosf(asinf((floor - bottom) / radius));
    } else if (origin[1] < top) {
        width = radius * cosf(asinf((top - origin[1]) / radius));
    } else {
        width = radius;
    }
    KF_Vector_Subtract(delta, centre, origin);
    width = (width + range) * (width + range);
    bottom = delta[0] * delta[0] + delta[2] * delta[2];
    COLLISION_RESULT->distance = bottom;
    if (width < bottom) {
        return;
    }
    half_angle += atan2f(radius + radius, sqrtf(bottom));
    top = func_0019B838(-(atan2f(delta[2], delta[0]) - 1.5707964f) - facing);
    if (top < -half_angle) {
        return;
    }
    if (half_angle < top) {
        return;
    }
    KF_Vector_Subtract(delta, origin, centre);
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    KF_Vector_Scale(delta, COLLISION_RESULT->normal, radius);
    func_00216C48(COLLISION_RESULT->point, centre, delta);
    COLLISION_RESULT->hit = 1;
}
