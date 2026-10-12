#include "common.h"

extern float D_004325A0[];

/* Sets D_004325A0[30] to float_value and D_004325A0[31] to int_value. */
void func_00195878(int float_value, int int_value) {
    D_004325A0[30] = float_value;
    *(int *)&D_004325A0[31] = int_value;
}
