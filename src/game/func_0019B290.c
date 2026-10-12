#include "kf_entity.h"

extern LinkedPoint *D_004470D0;

/* Puts a point at the head of the list at D_004470D0. */
void func_0019B290(LinkedPoint *point) {
    point->prev = NULL;
    point->next = D_004470D0;
    if (D_004470D0 != NULL) {
        D_004470D0->prev = point;
    }
    D_004470D0 = point;
}
