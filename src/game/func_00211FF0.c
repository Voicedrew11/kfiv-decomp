#include "kf_script.h"

extern f32 D_004325E8[];

/* Stores the float argument in D_004325E8, then finishes. */
void func_00211FF0(ScriptFloatValueCommand *command) {
    D_004325E8[0] = command->value.value;
    command->active = 0;
}
