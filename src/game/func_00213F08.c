#include "kf_script.h"
#include "kf_model.h"

MapProp *func_00116160(int, int);
int func_0019B9F0(float, float *, int);
extern int D_00446770;

/* Turn the target prop toward the x/y/z rotation of the command over `frames` frames. */
void func_00213F08(ScriptMoveCommand *command) {
    MapProp *prop;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop == NULL) {
        command->active = 0;
        return;
    }
    if (command->x.value == prop->model->rotation.x && command->y.value == prop->model->rotation.y &&
        command->z.value == prop->model->rotation.z) {
        command->active = 0;
        return;
    }
    if (command->frames.value == 0) {
        command->frames.value = 1;
    }
    func_0019B9F0(command->x.value, &prop->model->rotation.x, command->frames.value);
    func_0019B9F0(command->y.value, &prop->model->rotation.y, command->frames.value);
    func_0019B9F0(command->z.value, &prop->model->rotation.z, command->frames.value);
    if (--command->frames.value == 0) {
        command->active = 0;
    }
}
