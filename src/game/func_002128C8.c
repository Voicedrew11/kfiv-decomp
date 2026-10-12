#include "kf_model.h"
#include "kf_script.h"

void *func_001C8AF0(int);
int func_00211D90(void *, double);  // match: retail converts the float argument to double at the call site as the definition takes float

void func_002128C8(ScriptMoveCommand *command) {
    ModelNode *node;

    node = func_001C8AF0(command->target_id.value);
    if (node != NULL) {
        func_00211D90(&command->x, node->model->rotation.x);
        func_00211D90(&command->y, node->model->rotation.y);
        func_00211D90(&command->z, node->model->rotation.z);
    }
    command->active = 0;
}
