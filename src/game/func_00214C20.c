#include "kf_script.h"

void func_001B57A0(u8, u8, u8);

/* Script command: shake the controller. */
void func_00214C20(ScriptShakeCommand *command) {
    func_001B57A0(command->strength.value, command->duration.value, command->delay.value);
    command->active = 0;
}
