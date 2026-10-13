#include "kf_map_object.h"
#include "kf_script.h"

extern int D_00446770;

/* Eases a map object of the current map toward x, y, z, a 1/frames part of the way each frame. */
void func_002129F0(ScriptMoveCommand *command) {
    MapObject *object = KF_MapObject_GetById(D_00446770, command->target_id.value);
    int frames;
    f32 divisor;
    int frames_left;

    if (object == NULL) {
        command->active = 0;
        return;
    }
    if (command->x.value == object->model->position.x && command->y.value == object->model->position.y &&
        command->z.value == object->model->position.z) {
        command->active = 0;
    } else {
        frames = command->frames.value;
        if (frames == 0) {
            command->frames.value = 1;
            frames = 1;
        }
        divisor = frames;
        frames_left = frames - 1;
        command->frames.value = frames_left;
        object->model->position.x += (command->x.value - object->model->position.x) / divisor;
        object->model->position.y += (command->y.value - object->model->position.y) / divisor;
        object->model->position.z += (command->z.value - object->model->position.z) / divisor;
        if (frames_left <= 0) {
            command->active = 0;
        }
    }
}
