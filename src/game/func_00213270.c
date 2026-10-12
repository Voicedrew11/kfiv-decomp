#include "kf_model.h"
#include "kf_script.h"

int func_0011E1F0(int, int);
int func_00211D90(void *, double);  // match: retail converts the float argument to double at the call site (fptodp), as the definition takes double
extern int D_00446770;

/* Copies the rotation of the map object into the command's x, y, z, then finishes. */
void func_00213270(ScriptMoveCommand *command) {
    MapObject *object;

    object = (MapObject *)func_0011E1F0(D_00446770, command->target_id.value);
    if (object != NULL) {
        func_00211D90(&command->x, object->model->rotation.x);
        func_00211D90(&command->y, object->model->rotation.y);
        func_00211D90(&command->z, object->model->rotation.z);
    }
    command->active = 0;
}
