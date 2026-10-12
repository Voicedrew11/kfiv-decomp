#include "kf_script.h"

int func_001C8C80(int, int);
extern int D_00446770;

/* Runs func_001C8C80 for item `value` on the current map, then finishes. */
void func_00212450(ScriptValueCommand *command) {
    func_001C8C80(D_00446770, command->value.value);
    command->active = 0;
}
