#include "kf_collision.h"

extern const f32 D_00446FCC[];      /* -0.0f */

void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
void sceVu0Normalize(float *, float *);

/* Tests a sphere of radius `reach` at `centre` against an upright cylinder: `base` on its axis, its
   radius and its height. Level with the cylinder the two are compared seen from above; past either
   end the sphere is tested against the nearest point of that end's rim first. Leaves the squared
   distance between the two points. */
void func_0020F820(f32 *base, f32 *centre, f32 radius, f32 height, f32 reach) {
    f32 delta[4];
    f32 rim[4];
    CollisionResult *result = COLLISION_RESULT;
    f32 distance;   /* match: one variable for the height difference and each squared distance */

    result->hit = 0;
    distance = base[1] - centre[1];
    KF_Vector_Subtract(delta, base, centre);
    result->distance = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
    if (distance < -reach) {
        return;
    }
    if (height + reach < distance) {
        return;
    }
    // match: the code for the two ends comes after the level case and shares its last lines; only gotos give that order
    if (height < distance) {
        goto past_far_end;
    }
    if (distance < D_00446FCC[0]) {
        goto past_near_end;
    }
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    reach = (radius + reach) * (radius + reach);
    goto compare;

past_far_end:
    delta[1] = 0.0f;
    sceVu0Normalize(rim, delta);
    reach *= reach;
    KF_Vector_Scale(rim, rim, radius);
    func_00216C48(rim, rim, base);
    rim[1] -= height;
    KF_Vector_Subtract(rim, centre, rim);
    distance = rim[0] * rim[0] + rim[1] * rim[1] + rim[2] * rim[2];
    if (distance < reach) {
        COLLISION_RESULT->hit = 1;
        return;
    }
    goto level;

past_near_end:
    delta[1] = 0.0f;
    sceVu0Normalize(rim, delta);
    reach *= reach;
    KF_Vector_Scale(rim, rim, radius);
    func_00216C48(rim, rim, base);
    KF_Vector_Subtract(rim, centre, rim);
    distance = rim[0] * rim[0] + rim[1] * rim[1] + rim[2] * rim[2];
    if (distance < reach) {
        COLLISION_RESULT->hit = 1;
        return;
    }

level:
    reach = radius * radius;
    distance = delta[0] * delta[0] + delta[2] * delta[2];

compare:
    if (distance < reach) {
        COLLISION_RESULT->hit = 1;
    }
}
