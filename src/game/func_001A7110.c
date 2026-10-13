#include "common.h"
#include "kf_effect.h"

void Vector_Subtract(void *, void *, void *);
void func_001AF5B0(void *, void *);

/* Turns the vector from one position to another into angles at angles_out. */
void func_001A7110(void *angles_out, void *from, void *target) {
    Vector4 difference;

    Vector_Subtract(&difference, from, target);
    func_001AF5B0(angles_out, &difference);
}
