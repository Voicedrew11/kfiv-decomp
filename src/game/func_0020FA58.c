#include "kf_collision.h"

float atan2f(float, float);
float sqrtf(float);
float func_0019B838(float);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* Tests an upright cylinder (`other`) against the part of another one (`base`) that lies within
   `half_angle` either side of the direction `facing`: a swing's arc. Each cylinder is a point on
   its axis, a radius and a height. Leaves the squared distance seen from above, the way from the
   first to the second as the normal, and a point on the first one's rim. */
void func_0020FA58(f32 *base, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_height,
                   f32 half_angle, f32 facing) {
    f32 delta[4];
    f32 unused[4];
    f32 other_top;
    f32 top;
    f32 other_bottom;
    f32 bottom;
    f32 distance;
    f32 angle;

    other_top = other[1];
    top = base[1];
    other_bottom = other_top - other_height;
    bottom = top - height;
    COLLISION_RESULT->hit = 0;
    if (top < other_bottom) {
        return;
    }
    if (other_top < bottom) {
        return;
    }
    KF_Vector_Subtract(delta, base, other);
    other_radius = (radius + other_radius) * (radius + other_radius);    // match: the sum of the radii, squared, kept in the parameter
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    COLLISION_RESULT->distance = distance;
    if (other_radius < distance) {
        return;
    }
    half_angle += atan2f(radius + radius, sqrtf(distance));
    angle = -(atan2f(delta[2], delta[0]) - 1.5707964f);
    angle = func_0019B838(angle - facing);
    if (angle < -half_angle) {
        return;
    }
    if (half_angle < angle) {
        return;
    }
    KF_Vector_Subtract(delta, other, base);
    delta[1] = 0.0f;
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    KF_Vector_Scale(delta, COLLISION_RESULT->normal, radius);
    func_00216C48(COLLISION_RESULT->point, base, delta);
    // match: the height of the hit is one level divided by another, halved; the middle of the two was meant
    if (bottom <= other_bottom && other_top <= top) {
        COLLISION_RESULT->point[1] = other_top / other_bottom * 0.5f;
    }
    if (other_bottom <= bottom && top <= other_top) {
        COLLISION_RESULT->point[1] = top / bottom * 0.5f;
    }
    if (bottom <= other_top && other_top <= top) {
        COLLISION_RESULT->point[1] = bottom / other_top * 0.5f;
    }
    if (bottom <= other_bottom && other_bottom <= top) {
        COLLISION_RESULT->point[1] = top / other_bottom * 0.5f;
    }
    COLLISION_RESULT->hit = 1;
}
