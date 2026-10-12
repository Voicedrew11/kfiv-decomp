#include "kf_script.h"
#include "kf_game.h"

extern GameStatusSplitBits D_0038CC50;

/* Set bits 9 and 10 of the game status, and bit 11 when the argument is not zero. */
void func_00214A20(ScriptValueCommand *command) {
    GameStatusSplitBits *status = &D_0038CC50;

    status->bit9 = TRUE;
    status->bit10 = TRUE;
    status->bit11 = command->value.value != 0;
    command->active = 0;
}
