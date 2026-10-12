#include "kf_script.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Add the command's amount to the player's unkB4, keeping it between 0 and unk58. */
void func_002136F8(ScriptObjectCommand *command) {
    PlayerState *player = &D_00413F80;
    s32 amount;

    amount = command->object_id.value + player->unkB4;
    if (amount < 0) {
        player->unkB4 = 0;
    } else if (player->unk58 < amount) {
        player->unkB4 = player->unk58;
    } else {
        player->unkB4 = amount;
    }
    command->active = 0;
}
