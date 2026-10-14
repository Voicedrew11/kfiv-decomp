#include "kf_script.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Add the command's amount to the player's unkB4, keeping it between 0 and unk58. */
void func_002136F8(ScriptObjectCommand *command) {
    PlayerState *player = &D_00413F80;
    s32 amount;

    amount = command->object_id.value + player->hp_now;
    if (amount < 0) {
        player->hp_now = 0;
    } else if (player->hp_max < amount) {
        player->hp_now = player->hp_max;
    } else {
        player->hp_now = amount;
    }
    command->active = 0;
}
