#include "kf_script.h"

void Game_MarkMessageHeard(int);

/* Runs Game_MarkMessageHeard on the argument, then finishes. */
void func_00212248(ScriptValueCommand *command) {
    Game_MarkMessageHeard(command->value.value);
    command->active = 0;
}
