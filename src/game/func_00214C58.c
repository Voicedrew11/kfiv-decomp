#include "kf_script.h"

int func_00210C48(int);
int func_00211CF8(ScriptArg *, int);

/* Script command: store func_00210C48(input) in the result. */
void func_00214C58(ScriptLookupCommand *command) {
    int value;

    value = func_00210C48(command->input.value);
    func_00211CF8(&command->result, value);
    command->active = 0;
}
