#include "kf_script.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;

/* Set GameStatus.unk03 to 0 or 1. */
void func_00213A40(ScriptObjectCommand *command) {
    if ((u32)command->object_id.value < 2) {
        D_0038CC50.unk03 = command->object_id.value;
    }
    command->active = 0;
}
