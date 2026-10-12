#include "kf_script.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

int func_00211CF8(ScriptArg *, int);

/* Store the player's unkB4 and its limit unk58 in the command's two variables. */
void func_002137C8(ScriptCallCommand *command) {
    PlayerState *player = &D_00413F80;

    func_00211CF8(&command->target_id, player->unkB4);
    func_00211CF8(&command->first, player->unk58);
    command->active = 0;
}
