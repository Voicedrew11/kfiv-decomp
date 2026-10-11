#include "kf_script.h"
#include "kf_model.h"

MapObject *func_0011E1F0(int, int);
extern int D_00446770;

/* Add this command's per-frame offset to the target object's position; the command ends when its step count runs out. */
void func_00213090(ScriptMoveCommand *command) {
    MapObject *object;
    ModelInstance *model;
    int frames_left;

    object = func_0011E1F0(D_00446770, command->target_id.value);
    if (object == NULL || command->frames.value <= 0) {
        command->active = 0;
        return;
    }
    frames_left = command->frames.value - 1;
    model = object->model;
    model->position.x += command->x.value;
    model->position.y += command->y.value;
    model->position.z += command->z.value;
    command->frames.value = frames_left;
    if (frames_left <= 0) {
        command->active = 0;
    }
}
