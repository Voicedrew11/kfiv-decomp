#include "common.h"
#include "kf_effect.h"

void Vector_Normalize(void *dst, void *src);
void Vector_Scale(void *, void *, float);
void Vector_Add(void *, void *, void *);

/* Turns velocity toward direction by turn, then sets its length to speed. */
void func_001AFC88(Vector4 *velocity, Vector4 *direction, float speed, float turn) {
    Vector4 pull;

    Vector_Normalize(&pull, direction);
    Vector_Scale(&pull, &pull, turn);
    Vector_Add(velocity, velocity, &pull);
    Vector_Normalize(velocity, velocity);
    Vector_Scale(velocity, velocity, speed);
}
