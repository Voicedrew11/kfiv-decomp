#include "kf_script.h"

int func_0019E9E0(char);

void func_002146D0(ScriptObjectCommand *command) {
    func_0019E9E0(command->object_id.value);
    command->active = 0;
}
