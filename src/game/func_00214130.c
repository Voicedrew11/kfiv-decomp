#include "common.h"
#include "kf_script.h"
#include "kf_model.h"

MapProp *func_00116160(int, int);
int func_00211D90(void *, double);  // match: retail converts the float argument to double at the call site (fptodp), as the definition takes float
extern int D_00446770;

/* Feed the target prop's x/y/z position to three func_00211D90 commands, then clear the command. */
void func_00214130(ScriptPointCommand *command) {
    MapProp *prop;

    prop = func_00116160(D_00446770, command->object_id.value);
    if (prop != NULL) {
        func_00211D90(&command->from_x, prop->model->position.x);
        func_00211D90(&command->from_y, prop->model->position.y);
        func_00211D90(&command->from_z, prop->model->position.z);
    }
    command->active = 0;
}
