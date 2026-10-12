#include "kf_script.h"

int func_001C9B58(int, int);

/* Runs func_001C9B58 on the two arguments, then finishes. */
void func_00212480(ScriptPairCommand *command) {
    func_001C9B58(command->first.value, command->second.value);
    command->active = 0;
}
