#include "kf_player.h"
#include "kf_script.h"

extern PlayerState D_00413F80;

/* Empties the player's two gauges (unkBC and unkC0), then finishes. */
void func_00212428(ScriptValueCommand *command) {
    D_00413F80.unkC0 = D_00413F80.unkBC = 0;
    command->active = 0;
}
