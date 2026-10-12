#include "kf_script.h"

void func_001BB5C0(void);

/* Runs func_001BB5C0, then finishes. */
void func_002122F8(ScriptValueCommand *command) {
    func_001BB5C0();
    command->active = 0;
}
