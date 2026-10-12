#include "kf_script.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;

/* Let the player run (argument nonzero) or not (zero). */
void func_00213778(ScriptObjectCommand *command) {
    D_0038CC50.unk06 = command->object_id.value == 0;
    command->active = 0;
}
