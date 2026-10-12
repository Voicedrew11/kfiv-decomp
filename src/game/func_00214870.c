#include "kf_script.h"

void func_0019D7D0(char, u8, int);

void func_00214870(ScriptCallCommand *command) {
    func_0019D7D0(command->target_id.value, command->first.value, command->second.value);
    command->active = 0;
}
