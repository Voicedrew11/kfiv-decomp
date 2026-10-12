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

/* func_00210488 for the player's swing: the slab is given by its two heights (`lowest`, `highest`), and the
   ball must also lie within `half_pitch` of the swing's `pitch` as seen from the origin. */
void func_00210678(void *ball_centre, void *swing_origin, f32 radius, f32 range, f32 lowest, f32 highest, f32 half_angle,
                   f32 facing, f32 half_pitch, f32 pitch) {
    f32 *centre = ball_centre;
    f32 *origin = swing_origin;
    f32 delta[4];
    f32 unused[4];
    f32 bottom; // match: one variable for the ball's bottom and, later, the squared distance (both in $f12)
    f32 top; // match: one variable for the ball's top and, later, the turn to the ball (both in $f0)
    f32 flat_distance;
    f32 twice_radius;
    f32 pitch_low;
    f32 pitch_high;
    f32 pitch_to_ball;
    f32 width;

    COLLISION_RESULT->hit = 0;
    top = centre[1];
    bottom = top - radius;
    top += radius;
    if (top < lowest) {
        return;
    }
    if (highest < bottom) {
        return;
    }
    if (bottom < lowest) {
        width = radius * cosf(asinf((lowest - bottom) / radius));
    } else if (highest < top) {
        width = radius * cosf(asinf((top - highest) / radius));
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
    flat_distance = sqrtf(bottom);
    twice_radius = radius + radius;
    half_angle += atan2f(twice_radius, flat_distance);
    top = func_0019B838(-(atan2f(delta[2], delta[0]) - 1.5707964f) - facing);
    if (top < -half_angle) {
        return;
    }
    if (half_angle < top) {
        return;
    }
    half_pitch += atan2f(twice_radius, sqrtf(COLLISION_RESULT->distance));
    pitch_high = -pitch + half_pitch;
    pitch_low = -pitch - half_pitch;
    pitch_to_ball = atan2f(centre[1] - origin[1], flat_distance);
    if (pitch_to_ball < pitch_low) {
        return;
    }
    if (pitch_high < pitch_to_ball) {
        return;
    }
    KF_Vector_Subtract(delta, origin, centre);
    sceVu0Normalize(COLLISION_RESULT->normal, delta);
    KF_Vector_Scale(delta, COLLISION_RESULT->normal, radius);
    func_00216C48(COLLISION_RESULT->point, centre, delta);
    COLLISION_RESULT->hit = 1;
}
