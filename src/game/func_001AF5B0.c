#include "common.h"
#include "kf_effect.h"

float sqrtf(float);
float atan2f(float, float);

/* Writes the pitch and heading angles of a direction vector into angles[0] and angles[1]. */
void func_001AF5B0(float *angles, float *direction) {
    float negated_y = -direction[1];
    float horizontal = sqrtf(direction[0] * direction[0] + direction[2] * direction[2]);

    angles[0] = atan2f(negated_y, horizontal);
    angles[1] = atan2f(direction[0], direction[2]);
}
