/* Small vector and angle helpers: distances, heading and pitch between two points, stepping an angle, projecting
   a point to the screen, and an index of the largest of seven values. Nothing calls any of them. */
#include "core/game.h"

extern f32 D_00446FA4[];         /* -0.0f */
extern f32 world_screen_matrix[16];          /* the world-to-screen matrix */

float sqrtf(float);
float atan2f(float, float);
void Vector_Subtract(void *, void *, void *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);

/* Distance between two points. */
static float Vector_Distance(void *origin, void *target) {
    float delta[3];

    Vector_Subtract(delta, origin, target);
    return sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
}

/* Horizontal (XZ-plane) distance between two points. */
static float Vector_DistanceXZ(void *origin, void *target) {
    float delta[3];

    Vector_Subtract(delta, origin, target);
    return sqrtf(delta[0] * delta[0] + delta[2] * delta[2]);
}

/* Heading (angle in the XZ plane) of origin seen from target. */
static float Vector_Heading(void *origin, void *target) {
    float delta[3];

    Vector_Subtract(delta, origin, target);
    return atan2f(delta[0], delta[2]);
}

/* Pitch of a point seen from an origin. */
static float Vector_Pitch(void *point, void *origin) {
    float offset[3];

    Vector_Subtract(offset, point, origin);
    return atan2f(offset[1], sqrtf(offset[0] * offset[0] + offset[2] * offset[2]));
}

/* Steps *angle by 0.01 towards target. */
static void Angle_StepToward(f32 *angle, f32 target) {
    f32 current = *angle;

    if (current - target < D_00446FA4[0]) {
        *angle = current + 0.01f;
    } else {
        *angle = current - 0.01f;
    }
}

/* Projects a world-space point to the screen with the world-to-screen matrix world_screen_matrix. */
static void Vector_ProjectToScreen(void *point, int *screen) {
    Vu0_LoadMatrix(world_screen_matrix);
    Vu0_ProjectToScreen(screen, point);
}

/* Returns the index of the largest of the seven s16 values at owner->unk70. Nothing calls it. */
static int Geometry_IndexOfLargestOfSeven(Unk20D008Owner *owner) {
    s16 *values = owner->unk70;
    int largest = values[0];
    int largest_index = 0;
    int i;

    for (i = 1; i < 7; i++) {
        if (largest < values[i]) {
            largest = values[i];
            largest_index = i;
        }
    }
    return largest_index;
}
