#include "kf_script.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;

void func_0019CDA0(s8);

/* Stop sound slot 2, set bit 14 of the game status and wait until something clears it. */
void func_00214AB8(ScriptWaitCommand *command) {
    switch (command->started) {
    case 0:
        func_0019CDA0(2);
        command->started = 1;
        break;
    case 1:
        if (!D_0038CC50.bit14) {
            D_0038CC50.bit14 = TRUE;
            command->started = 2;
        }
        break;
    case 2:
        if (!D_0038CC50.bit14) {
            command->active = 0;
        }
        break;
    default:
        command->active = 0;
        break;
    }
}
