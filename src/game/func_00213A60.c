#include "kf_game.h"
#include "kf_script.h"

extern GameStatus D_0038CC50;
extern float D_004325A0[];

void func_00213A60(ScriptPointCommand *command) {
    if (D_0038CC50.unk03) {
        if (command->object_id.value >= 0) {
            if (command->object_id.value < 2) {
                D_0038CC50.unk04 = command->object_id.value;
                D_004325A0[8] = command->from_x.value;
                D_004325A0[9] = command->from_y.value;
                D_004325A0[10] = command->from_z.value;
            }
        }
    }
    command->active = 0;
}
