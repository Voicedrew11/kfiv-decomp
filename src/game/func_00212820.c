#include "kf_model.h"
#include "kf_script.h"

void *func_001C8AF0(int);
int func_00211D90(void *, double);  // match: retail converts the float argument to double at the call site (fptodp), as the definition takes double

/* copies the current position of the target object into the command's x, y, z */
void func_00212820(ScriptMoveCommand *command) {
    ModelNode *node;

    node = func_001C8AF0(command->target_id.value);
    if (node != NULL) {
        func_00211D90(&command->x, node->model->position.x);
        func_00211D90(&command->y, node->model->position.y);
        func_00211D90(&command->z, node->model->position.z);
    }
    command->active = 0;
}
