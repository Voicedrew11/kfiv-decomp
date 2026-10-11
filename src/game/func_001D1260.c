#include "common.h"

float sqrtf(float);
void KF_Vector_Subtract(float *, float *, float *);
extern float D_00446A0C[];

/* Distance from pos to target if within radius on every axis and overall, else D_00446A0C. */
float func_001D1260(float radius, float *pos, float *target) {
    float diff[4];
    float dist;

    if (pos[0] < target[0] - radius || target[0] + radius < pos[0]
        || pos[1] < target[1] - radius || target[1] + radius < pos[1]
        || pos[2] < target[2] - radius || target[2] + radius < pos[2]) {
        return D_00446A0C[0];
    }
    KF_Vector_Subtract(diff, pos, target);
    dist = sqrtf(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    if (dist <= radius) {
        return dist;
    }
    return D_00446A0C[0];
}
