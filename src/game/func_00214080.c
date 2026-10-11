#include "kf_script.h"
#include "kf_model.h"

MapProp *func_00116160(int, int);
float func_0019B838(float);
extern int D_00446770;

/* Step the target prop's x/y/z position by this command's per-frame deltas, ending the command when its step count runs out. */
void func_00214080(ScriptMoveCommand *command) {
    MapProp *prop;
    int steps_left;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop == NULL || command->frames.value <= 0) {
        command->active = 0;
        return;
    }
    prop->model->rotation.x = func_0019B838(prop->model->rotation.x + command->x.value);
    prop->model->rotation.y = func_0019B838(prop->model->rotation.y + command->y.value);
    prop->model->rotation.z = func_0019B838(prop->model->rotation.z + command->z.value);
    steps_left = command->frames.value - 1;
    command->frames.value = steps_left;
    if (steps_left < 0) {
        command->active = 0;
    }
}
