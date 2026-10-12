#include "kf_script.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

int func_001A1C58(int, void *, void *, void *);

/* Start effect `object_id` at the player's position and rotation. */
void func_00213938(ScriptObjectCommand *command) {
    PlayerState *player = &D_00413F80;

    func_001A1C58(command->object_id.value, &player->position, &player->rotation, player);
    command->active = 0;
}
