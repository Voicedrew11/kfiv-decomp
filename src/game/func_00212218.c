#include "kf_script.h"

void func_00103990(u32, u32);

/* Moves the state of area `first` to area `second`, then finishes. */
void func_00212218(ScriptPairCommand *command) {
    func_00103990(command->first.value, command->second.value);
    command->active = 0;
}
