#include "kf_script.h"

void Game_MoveRecollectionPage(u32, u32);

/* Moves the state of area `first` to area `second`, then finishes. */
void func_00212218(ScriptPairCommand *command) {
    Game_MoveRecollectionPage(command->first.value, command->second.value);
    command->active = 0;
}
