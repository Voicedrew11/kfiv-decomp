#include "kf_model.h"
#include "kf_script.h"

MapObject *func_001267B0(int);

/* Move the looked-up object to the command's x/y/z, then clear the command. */
void func_00216580(ScriptPointCommand *command) {
    MapObject *object;

    object = func_001267B0(command->object_id.value);
    if (object != NULL) {
        object->model->position.x = command->from_x.value;
        object->model->position.y = command->from_y.value;
        object->model->position.z = command->from_z.value;
    }
    command->active = 0;
}
