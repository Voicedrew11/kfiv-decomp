#include "kf_map_object.h"
#include "kf_script.h"

int func_00211D90(void *, double); // match: retail converts the float argument to double at the call site (fptodp)
extern int D_00446770;

/* Stores the position of a map object of the current map in from_x, from_y and from_z. */
void func_002131C0(ScriptPointCommand *command) {
    MapObject *object;

    object = KF_MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        func_00211D90(&command->from_x, object->model->position.x);
        func_00211D90(&command->from_y, object->model->position.y);
        func_00211D90(&command->from_z, object->model->position.z);
    }
    command->active = 0;
}
