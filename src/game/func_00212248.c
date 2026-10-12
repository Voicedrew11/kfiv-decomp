#include "kf_script.h"

void func_00103858(int);

/* Runs func_00103858 on the argument, then finishes. */
void func_00212248(ScriptValueCommand *command) {
    func_00103858(command->value.value);
    command->active = 0;
}
