#include "kf_script.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

int func_00211CF8(ScriptArg *, int);

/* Store the player's unkB8 and its limit unk5C in the command's two variables. */
void func_00213818(ScriptCallCommand *command) {
    PlayerState *player = &D_00413F80;

    func_00211CF8(&command->target_id, player->unkB8);
    func_00211CF8(&command->first, player->unk5C);
    command->active = 0;
}
