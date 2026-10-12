#include "kf_script.h"

void func_001187F0(int);

void func_002149F0(ScriptObjectCommand *command) {
    func_001187F0(command->object_id.value);
    command->active = 0;
}
