#include "kf_collision.h"

float atan2f(float, float);
float sqrtf(float);
float func_0019B838(float);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* Tests an upright cylinder (`other`, between the levels `other_low` and `other_high`) against the
   part of another one (`base`, its radius and height) that a swing covers: within `half_angle` either
   side of the direction `facing`, and within `pitch_range` either side of the slope `pitch`. Leaves
   the squared distance seen from above, the way from the first to the second as the normal, and a
   point on the rim. */
void func_0020FCF8(f32 *base, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_low,
                   f32 other_high, f32 half_angle, f32 facing, f32 pitch_range, f32 pitch) {
    f32 delta[4];
    f32 unused[4];
    f32 top;
    f32 bottom;
    f32 distance;
    f32 flat;
    f32 angle;      /* match: one variable for the angle to the other cylinder and each height difference */
    f32 near;
    f32 upper;
    f32 lower;
    f32 first;
    f32 second;
    f32 lowest;
    f32 level;

    top = base[1];
    COLLISION_RESULT->hit = 0;
    bottom = top - height;
    if (other_high < bottom) {
        return;
    }
    if (top < other_low) {
        return;
    }
    KF_Vector_Subtract(delta, base, other);
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    COLLISION_RESULT->distance = distance;
    if ((radius + other_radius) * (radius + other_radius) < distance) {
        return;
    }
    flat = sqrtf(distance);
    half_angle += atan2f(radius + radius, flat);
    angle = -(atan2f(delta[2], delta[0]) - 1.5707964f);
    angle = func_0019B838(angle - facing);
    if (angle < -half_angle) {
        return;
    }
    if (half_angle < angle) {
        return;
    }
    near = flat - radius;
    lower = -pitch - pitch_range;
    upper = -pitch + pitch_range;
    angle = base[1] - height - other[1];
    first = atan2f(angle, near);
    if (other[1] < base[1] - height) {
        second = atan2f(angle, flat);
    } else {
        second = atan2f(angle, flat + radius);
    }
    if (first < second) {
        lowest = first;
    } else {
        lowest = second;
    }
    angle = base[1] - other[1];
    first = atan2f(angle, near);
    if (other[1] > base[1]) {
        second = atan2f(angle, flat);
    } else {
        second = atan2f(angle, flat + radius);
    }
    if (second < first) {
        second = first;
    }
    if (upper < lowest) {
        return;
    }
    if (second < lower) {
        return;
    }
    KF_Vector_Subtract(delta, other, base);
    delta[1] = 0.0f;
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    KF_Vector_Scale(delta, COLLISION_RESULT->normal, other_radius);
    func_00216C48(COLLISION_RESULT->point, base, delta);
    // match: `level` is never given a value; the height of the other cylinder was meant
    if (level < bottom) {
        COLLISION_RESULT->point[1] = bottom;
    } else if (top < level) {
        COLLISION_RESULT->point[1] = top;
    } else {
        COLLISION_RESULT->point[1] = level;
    }
    COLLISION_RESULT->hit = 1;
}
