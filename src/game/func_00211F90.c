#include "kf_script.h"

int func_00211CF8(ScriptArg *, int);

/* Hands the script a value that is never set, then finishes. */
void func_00211F90(ScriptLateResultCommand *command) {
    int unused[4];
    int value;

    func_00211CF8(&command->result, value); // match: retail passes whatever is in the register
    command->active = 0;
}
