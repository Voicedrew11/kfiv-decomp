#include "kf_script.h"
#include "kf_model.h"

extern int D_00446770;

MapObject *func_0011E1F0(int, int);
int func_0019B9F0(float, float *, int);

/* Eases an object's position toward a goal over frames.value frames, then stops. */
void func_00212B00(ScriptMoveCommand *command) {
    MapObject *object;

    object = func_0011E1F0(D_00446770, command->target_id.value);
    if (object == NULL) {
        command->active = 0;
        return;
    }
    if (command->x.value == object->model->rotation.x && command->y.value == object->model->rotation.y &&
        command->z.value == object->model->rotation.z) {
        command->active = 0;
        return;
    }
    func_0019B9F0(command->x.value, &object->model->rotation.x, command->frames.value);
    func_0019B9F0(command->y.value, &object->model->rotation.y, command->frames.value);
    func_0019B9F0(command->z.value, &object->model->rotation.z, command->frames.value);
    if (--command->frames.value < 0) {
        command->active = 0;
    }
}
