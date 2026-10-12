#include "kf_script.h"
#include "kf_game.h"

extern CameraState D_004325A0;

/* Shake the camera: how far the shake reaches and how many frames it lasts. */
void func_00213C78(ScriptCameraShakeCommand *command) {
    CameraState *camera = &D_004325A0;

    camera->unk74 = command->frames.value;
    camera->unk70 = command->reach.value;
    command->active = 0;
}
