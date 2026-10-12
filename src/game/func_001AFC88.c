#include "common.h"
#include "kf_effect.h"

void func_00216D20(void *dst, void *src);
void KF_Vector_Scale(void *, void *, float);
void func_00216C48(void *, void *, void *);

/* Turns velocity toward direction by turn, then sets its length to speed. */
void func_001AFC88(Vector4 *velocity, Vector4 *direction, float speed, float turn) {
    Vector4 pull;

    func_00216D20(&pull, direction);
    KF_Vector_Scale(&pull, &pull, turn);
    func_00216C48(velocity, velocity, &pull);
    func_00216D20(velocity, velocity);
    KF_Vector_Scale(velocity, velocity, speed);
}
