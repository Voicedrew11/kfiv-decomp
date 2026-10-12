#include "kf_collision.h"

void KF_Vector_Subtract(void *, void *, void *);

/* Hit test of a point against an upright cylinder: base at origin, given radius squared and height. */
void func_0020F230(f32 *point, f32 *origin, f32 radius_squared, f32 height) {
    f32 delta[3];

    if (point[1] > origin[1]) {
        return;
    }
    if (point[1] < origin[1] - height) {
        return;
    }
    KF_Vector_Subtract(delta, point, origin);
    if (delta[0] * delta[0] + delta[2] * delta[2] < radius_squared) {
        COLLISION_RESULT->hit = 1;
    }
}
