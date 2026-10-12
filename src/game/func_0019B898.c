#include "common.h"

/* Wraps an angle into the range [0, 2*pi) by repeated subtraction or addition. */
float func_0019B898(float radians) {
    float angle;

    angle = radians;
    if (6.2831855f <= angle) {
        do {
            angle -= 6.2831855f;
        } while (6.2831855f <= angle);
    }
    if (angle < 0.0f) {
        do {
            angle += 6.2831855f;
        } while (angle < 0.0f);
    }
    return angle;
}
