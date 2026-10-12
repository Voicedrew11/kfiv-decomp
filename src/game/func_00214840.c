#include "kf_script.h"

int func_0019D3A0(u8, int);

void func_00214840(ScriptCallCommand *command) {
    func_0019D3A0(command->target_id.value, command->first.value);
    command->active = 0;
}
