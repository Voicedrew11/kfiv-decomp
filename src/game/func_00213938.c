#include "kf_script.h"
#include "kf_player.h"
#include "kf_effect.h"

extern PlayerState D_00413F80;

/* Start effect `object_id` at the player's position and rotation. */
void func_00213938(ScriptObjectCommand *command) {
    PlayerState *player = &D_00413F80;

    KF_Effect_StartSpawner(command->object_id.value, &player->position, &player->rotation, player);
    command->active = 0;
}
