#include "kf_script.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

int func_00211CF8(ScriptArg *, int);

/* Store the player's step counter (unk258) in the command's variable. */
void func_00213790(ScriptObjectCommand *command) {
    func_00211CF8(&command->object_id, D_00413F80.unk258);
    command->active = 0;
}
